package main

import (
	"fmt"
	"os"
	"os/exec"
	"os/user"
	"runtime"
	"sort"
	"strconv"
	"strings"
	"time"

	"github.com/gdamore/tcell/v2"
	"github.com/rivo/tview"
	"golang.org/x/sys/unix"
)

type ProcessInfo struct {
	PID     int
	Name    string
	User    string
	CPU     float64
	Memory  float64
	Status  string
	Command string
}

type SystemInfo struct {
	TotalMemory      uint64
	FreeMemory       uint64
	UsedMemory       uint64
	MemoryUsage      float64
	TotalSwap        uint64
	FreeSwap         uint64
	UsedSwap         uint64
	SwapUsage        float64
	CPUUsage         float64
	RunningProcesses int
	TotalProcesses   int
	Uptime           string
	Load1            string
	Load5            string
	Load15           string
	IdleTime         string
}

type TaskManager struct {
	app         *tview.Application
	processes   []ProcessInfo
	systemInfo  SystemInfo
	processList *tview.List
	infoView    *tview.TextView
	flex        *tview.Flex
	sortBy      string
	sortDesc    bool
	cpuStats    CPUStats
}

type ProcessCPU struct {
	PID       int
	UTime     uint64
	STime     uint64
	StartTime uint64
}

type CPUStats struct {
	TotalTime  uint64
	ProcessCPU map[int]*ProcessCPU
}

func (tm *TaskManager) calculateCPUUsage() {
	if tm.cpuStats.ProcessCPU == nil {
		tm.cpuStats.ProcessCPU = make(map[int]*ProcessCPU)
	}

	// Чтение общего времени CPU из /proc/stat
	if data, err := os.ReadFile("/proc/stat"); err == nil {
		lines := strings.Split(string(data), "\n")
		for _, line := range lines {
			if strings.HasPrefix(line, "cpu ") {
				fields := strings.Fields(line)
				var total uint64
				for i := 1; i < len(fields); i++ {
					val, _ := strconv.ParseUint(fields[i], 10, 64)
					total += val
				}
				tm.cpuStats.TotalTime = total
				break
			}
		}
	}

	// Обновление данных для каждого процесса
	for i := range tm.processes {
		pid := tm.processes[i].PID
		statFile := fmt.Sprintf("/proc/%d/stat", pid)
		data, err := os.ReadFile(statFile)
		if err != nil {
			continue
		}

		fields := strings.Fields(string(data))
		if len(fields) < 22 {
			continue
		}

		utime, _ := strconv.ParseUint(fields[13], 10, 64)
		stime, _ := strconv.ParseUint(fields[14], 10, 64)
		starttime, _ := strconv.ParseUint(fields[21], 10, 64)

		if prev, exists := tm.cpuStats.ProcessCPU[pid]; exists {
			// Расчет использования CPU
			totalTimeDiff := tm.cpuStats.TotalTime - prev.TotalTime
			if totalTimeDiff > 0 {
				processTimeDiff := (utime + stime) - (prev.UTime + prev.STime)
				tm.processes[i].CPU = (float64(processTimeDiff) / float64(totalTimeDiff)) * 100.0 * float64(runtime.NumCPU())
			}
		}

		tm.cpuStats.ProcessCPU[pid] = &ProcessCPU{
			PID:       pid,
			UTime:     utime,
			STime:     stime,
			StartTime: starttime,
			TotalTime: tm.cpuStats.TotalTime,
		}
	}
}

func main() {
	app := tview.NewApplication()

	tm := &TaskManager{
		app:      app,
		sortBy:   "cpu",
		sortDesc: true,
	}

	tm.createUI()

	tm.refreshSystemInfo()
	tm.refreshProcesses()
	tm.updateUI()

	go tm.updateData()

	if err := app.Run(); err != nil {
		fmt.Fprintf(os.Stderr, "Error: %v\n", err)
		os.Exit(1)
	}
}

func (tm *TaskManager) createUI() {
	tm.processList = tview.NewList().
		ShowSecondaryText(true).
		SetHighlightFullLine(true)

	tm.processList.SetBorder(true).SetTitle(" Processes (Sort: CPU ↓) ")

	tm.infoView = tview.NewTextView().
		SetDynamicColors(true).
		SetRegions(true).
		SetWordWrap(true)

	tm.infoView.SetBorder(true).SetTitle(" System Information ")

	tm.flex = tview.NewFlex().
		SetDirection(tview.FlexRow).
		AddItem(tm.infoView, 8, 1, false).
		AddItem(tm.processList, 0, 1, true)

	tm.processList.SetInputCapture(func(event *tcell.EventKey) *tcell.EventKey {
		switch event.Key() {
		case tcell.KeyF1: // CPU
			tm.sortBy = "cpu"
			tm.sortDesc = !tm.sortDesc
		case tcell.KeyF2: // Memory
			tm.sortBy = "memory"
			tm.sortDesc = !tm.sortDesc
		case tcell.KeyF3: // PID
			tm.sortBy = "pid"
			tm.sortDesc = !tm.sortDesc
		case tcell.KeyF4: // Name
			tm.sortBy = "name"
			tm.sortDesc = !tm.sortDesc
		case tcell.KeyF5: // Refresh
			// уже есть
		case tcell.KeyF6: // User
			tm.sortBy = "user"
			tm.sortDesc = !tm.sortDesc
		case tcell.KeyF7: // State
			tm.sortBy = "state"
			tm.sortDesc = !tm.sortDesc
		case tcell.KeyF9: // Kill
			tm.killProcess()
		case tcell.KeyF10: // Quit
			tm.app.Stop()
		case tcell.KeyRune:
			switch event.Rune() {
			case 'k', 'K': // Альтернативная клавиша для kill
				tm.killProcess()
			case 'q', 'Q':
				tm.app.Stop()
			case '+': // Увеличить приоритет
				tm.niceProcess(1)
			case '-': // Уменьшить приоритет
				tm.niceProcess(-1)
			}
		}
		tm.refreshProcesses()
		tm.updateUI()
		return event
	})

	tm.app.SetRoot(tm.flex, true)
}

func (tm *TaskManager) niceProcess(delta int) {
	currentItem := tm.processList.GetCurrentItem()
	if currentItem >= 0 && currentItem < len(tm.processes) && currentItem < 25 {
		pid := tm.processes[currentItem].PID

		cmd := exec.Command("renice", fmt.Sprintf("%d", delta), strconv.Itoa(pid))
		if err := cmd.Run(); err != nil {
			fmt.Println("Error of execution!")
		}
	}
}

func (tm *TaskManager) updateData() {
	ticker := time.NewTicker(2 * time.Second)
	defer ticker.Stop()

	for {
		select {
		case <-ticker.C:
			tm.app.QueueUpdateDraw(func() {
				tm.refreshSystemInfo()
				tm.refreshProcesses()
				tm.updateUI()
			})
		}
	}
}

func (tm *TaskManager) refreshSystemInfo() {
	var sysinfo unix.Sysinfo_t
	if err := unix.Sysinfo(&sysinfo); err == nil {
		tm.systemInfo.TotalMemory = sysinfo.Totalram * uint64(sysinfo.Unit) / 1024 / 1024
		tm.systemInfo.FreeMemory = sysinfo.Freeram * uint64(sysinfo.Unit) / 1024 / 1024
		tm.systemInfo.UsedMemory = tm.systemInfo.TotalMemory - tm.systemInfo.FreeMemory
		if tm.systemInfo.TotalMemory > 0 {
			tm.systemInfo.MemoryUsage = float64(tm.systemInfo.UsedMemory) / float64(tm.systemInfo.TotalMemory) * 100
		}
	}

	// Получение информации о процессах
	files, _ := os.ReadDir("/proc")
	count := 0
	for _, file := range files {
		if _, err := strconv.Atoi(file.Name()); err == nil {
			count++
		}
	}
	tm.systemInfo.TotalProcesses = count
	tm.systemInfo.RunningProcesses = count

	if uptime, err := os.ReadFile("/proc/uptime"); err == nil {
		fields := strings.Fields(string(uptime))
		if len(fields) > 0 {
			seconds, _ := strconv.ParseFloat(fields[0], 64)
			tm.systemInfo.Uptime = formatUptime(seconds)
		}
	}

	if loadavg, err := os.ReadFile("/proc/loadavg"); err == nil {
		fields := strings.Fields(string(loadavg))
		if len(fields) >= 3 {
			tm.systemInfo.Load1 = fields[0]
			tm.systemInfo.Load5 = fields[1]
			tm.systemInfo.Load15 = fields[2]
		}
	}

	// Получение информации о времени работы
	if stat, err := os.ReadFile("/proc/uptime"); err == nil {
		fields := strings.Fields(string(stat))
		if len(fields) >= 2 {
			uptime, _ := strconv.ParseFloat(fields[0], 64)
			idle, _ := strconv.ParseFloat(fields[1], 64)
			tm.systemInfo.Uptime = formatUptime(uptime)
			tm.systemInfo.IdleTime = formatUptime(idle)
		}
	}
}

func (tm *TaskManager) refreshProcesses() {
	var processes []ProcessInfo

	files, err := os.ReadDir("/proc")
	if err != nil {
		return
	}

	for _, file := range files {
		pid, err := strconv.Atoi(file.Name())
		if err != nil {
			continue
		}

		statusFile := fmt.Sprintf("/proc/%d/status", pid)
		data, err := os.ReadFile(statusFile)
		if err != nil {
			continue
		}

		process := ProcessInfo{PID: pid}
		lines := strings.Split(string(data), "\n")

		for _, line := range lines {
			if strings.HasPrefix(line, "Name:") {
				process.Name = strings.TrimSpace(strings.TrimPrefix(line, "Name:"))
			} else if strings.HasPrefix(line, "Uid:") {
				fields := strings.Fields(line)
				if len(fields) >= 2 {
					uid := fields[1]
					process.User = getUserName(uid)
				}
			} else if strings.HasPrefix(line, "VmRSS:") {
				fields := strings.Fields(line)
				if len(fields) >= 2 {
					kb, _ := strconv.ParseFloat(fields[1], 64)
					process.Memory = kb / 1024 // Конвертация в MB
				}
			} else if strings.HasPrefix(line, "State:") {
				// Добавляем состояние процесса
				state := strings.TrimSpace(strings.TrimPrefix(line, "State:"))
				process.Status = getProcessState(state)
			}
		}

		cmdlineFile := fmt.Sprintf("/proc/%d/cmdline", pid)
		cmdData, err := os.ReadFile(cmdlineFile)
		if err == nil {
			process.Command = strings.ReplaceAll(string(cmdData), "\x00", " ")
			if process.Command == "" {
				process.Command = process.Name
			}
		} else {
			process.Command = process.Name
		}

		processes = append(processes, process)
	}

	tm.sortProcesses(processes)
	tm.processes = processes
}

func getProcessState(state string) string {
	switch state {
	case "R":
		return "Running"
	case "S":
		return "Sleeping"
	case "D":
		return "Disk Sleep"
	case "Z":
		return "Zombie"
	case "T":
		return "Stopped"
	case "t":
		return "Tracing stop"
	default:
		return state
	}
}

func (tm *TaskManager) sortProcesses(processes []ProcessInfo) {
	switch tm.sortBy {
	case "cpu":
		sort.Slice(processes, func(i, j int) bool {
			if tm.sortDesc {
				return processes[i].CPU > processes[j].CPU
			}
			return processes[i].CPU < processes[j].CPU
		})
	case "memory":
		sort.Slice(processes, func(i, j int) bool {
			if tm.sortDesc {
				return processes[i].Memory > processes[j].Memory
			}
			return processes[i].Memory < processes[j].Memory
		})
	case "pid":
		sort.Slice(processes, func(i, j int) bool {
			if tm.sortDesc {
				return processes[i].PID > processes[j].PID
			}
			return processes[i].PID < processes[j].PID
		})
	}
}

func (tm *TaskManager) updateUI() {
	infoText := fmt.Sprintf("[yellow]Memory: [white]%.1f/%.1f MB (%.1f%%) | "+
		"[yellow]CPU: [white]%.1f%% | "+
		"[yellow]Load: [white]%s | "+
		"[yellow]Processes: [white]%d/%d | "+
		"[yellow]Uptime: [white]%s",
		tm.systemInfo.UsedMemory, tm.systemInfo.TotalMemory, tm.systemInfo.MemoryUsage,
		tm.systemInfo.CPUUsage,
		tm.systemInfo.Load1,
		tm.systemInfo.RunningProcesses, tm.systemInfo.TotalProcesses,
		tm.systemInfo.Uptime)

	tm.infoView.SetText(infoText)

	tm.processList.Clear()

	sortIndicator := "↓"
	if !tm.sortDesc {
		sortIndicator = "↑"
	}

	tm.processList.SetTitle(fmt.Sprintf(" Processes (Sort: %s %s) - %d total ", strings.ToUpper(tm.sortBy), sortIndicator, len(tm.processes)))

	for i, proc := range tm.processes {
		if i >= 25 { // Показываем только топ-25 процессов
			break
		}

		mainText := fmt.Sprintf("[white]%6d [yellow]%-20s [green]%-8s",
			proc.PID, truncateString(proc.Name, 20), truncateString(proc.User, 8))

		secondaryText := fmt.Sprintf("[gray]CPU: %.1f%% | Mem: %.1f MB | %s",
			proc.CPU, proc.Memory, truncateString(proc.Command, 40))

		tm.processList.AddItem(mainText, secondaryText, 0, nil)
	}
}

func (tm *TaskManager) killProcess() {
	currentItem := tm.processList.GetCurrentItem()
	if currentItem >= 0 && currentItem < len(tm.processes) && currentItem < 25 {
		pid := tm.processes[currentItem].PID
		processName := tm.processes[currentItem].Name

		modal := tview.NewModal().
			SetText(fmt.Sprintf("Kill process %d (%s)?", pid, processName)).
			AddButtons([]string{"SIGTERM", "SIGKILL", "Cancel"}).
			SetDoneFunc(func(buttonIndex int, buttonLabel string) {
				if buttonLabel != "Cancel" {
					var signal string
					if buttonLabel == "SIGTERM" {
						signal = "15"
					} else {
						signal = "9"
					}

					cmd := exec.Command("kill", "-"+signal, strconv.Itoa(pid))
					if err := cmd.Run(); err != nil {
						// Можно добавить вывод ошибки
					}
				}
				tm.app.SetRoot(tm.flex, true)
			})

		tm.app.SetRoot(modal, false)
	}
}

func getUserName(uid string) string {
	u, err := user.LookupId(uid)
	if err != nil {
		return "uid:" + uid
	}
	return u.Username
}

func formatUptime(seconds float64) string {
	days := int(seconds) / 86400
	hours := (int(seconds) % 86400) / 3600
	minutes := (int(seconds) % 3600) / 60

	return fmt.Sprintf("%dd %dh %dm", days, hours, minutes)
}

func truncateString(s string, maxLength int) string {
	if len(s) <= maxLength {
		return s
	}
	return s[:maxLength-3] + "..."
}
