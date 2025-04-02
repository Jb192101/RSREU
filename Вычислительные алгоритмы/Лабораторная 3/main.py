import sys
from PyQt6.QtWidgets import QApplication
from ViewModel.SolverViewModel import SolverViewModel
from View.MainWindow import MainWindow
import Model

def main():
    app = QApplication(sys.argv)

    window = MainWindow()

    # Настройка стилей (опционально)
    app.setStyle('Fusion')

    window.show()
    sys.exit(app.exec())


if __name__ == "__main__":
    main()