package main

import (
  "fmt"
  "os"
  "os/exec"
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
    case tcell.KeyF1:
      tm.sortBy = "cpu"
      tm.sortDesc = !tm.sortDesc
      tm.refreshProcesses()
      tm.updateUI()
    case tcell.KeyF2:
      tm.sortBy = "memory"
      tm.sortDesc = !tm.sortDesc
      tm.refreshProcesses()
      tm.updateUI()
    case tcell.KeyF3:
      tm.sortBy = "pid"
      tm.sortDesc = !tm.sortDesc
      tm.refreshProcesses()
      tm.updateUI()
    case tcell.KeyF5:
      tm.refreshSystemInfo()
      tm.refreshProcesses()
      tm.updateUI()
    case tcell.KeyF9:
      tm.killProcess()
    case tcell.KeyF10:
      tm.app.Stop()
    case tcell.KeyRune:
      switch event.Rune() {
      case 'q', 'Q':
        tm.app.Stop()
      }
    }
    return event
  })

  tm.app.SetRoot(tm.flex, true)
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
    "[yellow]Processes: [white]%d | "+
    "[yellow]Uptime: [white]%s",
    tm.systemInfo.UsedMemory, tm.systemInfo.TotalMemory, tm.systemInfo.MemoryUsage,
    tm.systemInfo.TotalProcesses,
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
  return "uid:" + uid
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