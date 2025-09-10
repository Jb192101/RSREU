package main

import (
	"bufio"
	"fmt"
	"os"
	"strings"
)

func main() {
	procFiles := map[string]string{
		"/proc/cpuinfo": "Информация о процессоре (cpuinfo)",
		"/proc/stat":    "Статистика процессора (stat)",
		"/proc/loadavg": "Средняя загрузка системы (loadavg)",
		"/proc/version": "Версия ядра Linux (version)",
		"/proc/meminfo": "Информация о памяти (meminfo)",
		"/proc/uptime":  "Время работы системы (uptime)",
	}

	fmt.Println("=== ИНФОРМАЦИЯ О СИСТЕМЕ И ПРОЦЕССОРЕ ИЗ /proc ===\n")

	for filepath, description := range procFiles {
		fmt.Printf("--- %s ---\n", description)

		content, err := readProcFile(filepath)
		if err != nil {
			fmt.Printf("Ошибка чтения %s: %v\n\n", filepath, err)
			continue
		}

		if filepath == "/proc/cpuinfo" {
			displayFirstCPUInfo(content)
		} else {
			fmt.Println(content)
		}
		fmt.Println()
	}
}

func readProcFile(filename string) (string, error) {
	file, err := os.Open(filename)
	if err != nil {
		return "", err
	}
	defer file.Close()

	var content strings.Builder
	scanner := bufio.NewScanner(file)

	for scanner.Scan() {
		content.WriteString(scanner.Text() + "\n")
	}

	if err := scanner.Err(); err != nil {
		return "", err
	}

	return content.String(), nil
}

func displayFirstCPUInfo(content string) {
	lines := strings.Split(content, "\n")
	var firstCPU []string
	inFirstCPU := false

	for _, line := range lines {
		if strings.TrimSpace(line) == "" {
			if inFirstCPU {
				break
			}
			continue
		}

		if !inFirstCPU && strings.Contains(line, "processor") {
			inFirstCPU = true
		}

		if inFirstCPU {
			firstCPU = append(firstCPU, line)
		}
	}

	fmt.Println(strings.Join(firstCPU, "\n"))
}

func displayAllCPUInfo() {
	file, err := os.Open("/proc/cpuinfo")
	if err != nil {
		fmt.Printf("Ошибка открытия cpuinfo: %v\n", err)
		return
	}
	defer file.Close()

	fmt.Println("=== ДЕТАЛЬНАЯ ИНФОРМАЦИЯ О ВСЕХ ПРОЦЕССОРАХ ===")

	scanner := bufio.NewScanner(file)
	processorCount := 0

	for scanner.Scan() {
		line := scanner.Text()
		fmt.Println(line)

		if strings.HasPrefix(line, "processor") {
			processorCount++
		}
	}

	fmt.Printf("\nОбщее количество процессорных ядер/потоков: %d\n", processorCount)

	if err := scanner.Err(); err != nil {
		fmt.Printf("Ошибка чтения cpuinfo: %v\n", err)
	}
}
