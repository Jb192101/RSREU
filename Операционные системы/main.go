package main

// Файл с логикой программы

// Для получения общей инфы - /proc/cpuinfo

// Статусы
// 1. "running" - запущенный процесс
// 2.1 "sleeping s" - прерывный сон процесса
// 2.2 "sleeping d" - непрерывный сон процесса
// 3. "stopped" - остановленный процесс
// 4. "zombie" - зомби-процесс

// Структура процесса
type ProcessInfo struct {
	ProcessID int
	Command   string
	Status    string
}

// Структура для диспетчера
type TaskDispatcher struct {
	mapOfProcesses map[int]*ProcessInfo
	mapOfWorkLoad  map[int]int
}

func main() {

}

func printInfo() {

}

// Ф-ия для получения данных первоначально
func takeDataFromProc() TaskDispatcher {
	return nil
}

// Ф-ия для изменения данных
func updateData(dispatcher TaskDispatcher) {

}
