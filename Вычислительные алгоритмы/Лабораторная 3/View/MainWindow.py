import sys
from copy import deepcopy
from PyQt6.QtWidgets import (QApplication, QMainWindow, QWidget, QLabel,
                            QLineEdit, QPushButton, QVBoxLayout, QHBoxLayout,
                            QMessageBox, QGroupBox, QFrame, QGridLayout)
from PyQt6.QtGui import QPixmap
from PyQt6.QtCore import Qt

#from View.CoefficientsWindow import CoefficientsDialog
from ViewModel.SolverViewModel import SolverViewModel

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.viewmodel = SolverViewModel()
        self.setup_ui()
        self.connect_signals()

    def setup_ui(self):
        self.setWindowTitle("Лабораторная работа 3 - Барышев")
        self.setGeometry(100, 100, 900, 600)

        central_widget = QWidget()
        self.setCentralWidget(central_widget)

        main_layout = QHBoxLayout()
        central_widget.setLayout(main_layout)

        # Левая панель - ввод коэффициентов
        input_panel = QGroupBox("Коэффициенты системы")
        input_layout = QVBoxLayout()

        # Матрица 4x4 как двумерный список
        matrix_group = QGroupBox("Матрица A (4x4)")
        matrix_layout = QGridLayout()

        # Создаем двумерный список полей ввода
        self.matrix_inputs = [[None for _ in range(4)] for _ in range(4)]
        for row in range(4):
            for col in range(4):
                input_field = QLineEdit("0")
                input_field.setMaximumWidth(60)
                input_field.setAlignment(Qt.AlignmentFlag.AlignRight)
                matrix_layout.addWidget(input_field, row, col)
                self.matrix_inputs[row][col] = input_field  # Сохраняем как матрицу

        matrix_group.setLayout(matrix_layout)
        input_layout.addWidget(matrix_group)

        # Вектор B
        vector_group = QGroupBox("Вектор B (4 значения)")
        vector_layout = QGridLayout()

        self.vector_inputs = []
        for i in range(4):
            input_field = QLineEdit("0")
            input_field.setMaximumWidth(60)
            input_field.setAlignment(Qt.AlignmentFlag.AlignRight)
            vector_layout.addWidget(input_field, 0, i)
            self.vector_inputs.append(input_field)

        vector_group.setLayout(vector_layout)
        input_layout.addWidget(vector_group)

        # Кнопка решения
        self.solve_button = QPushButton("Решить систему")
        input_layout.addWidget(self.solve_button)

        input_panel.setLayout(input_layout)
        main_layout.addWidget(input_panel, stretch=2)

        # Правая панель - результаты
        result_panel = self.create_results_panel()
        main_layout.addWidget(result_panel, stretch=1)

    def create_results_panel(self):
        panel = QGroupBox("Результаты")
        layout = QVBoxLayout()

        # Вектор решений
        solution_group = QGroupBox("Вектор решений X")
        solution_layout = QVBoxLayout()

        self.solution_labels = []
        for i in range(4):
            hbox = QHBoxLayout()
            label = QLabel(f"x{i + 1} = ")
            value_label = QLabel("-")
            value_label.setAlignment(Qt.AlignmentFlag.AlignRight)
            hbox.addWidget(label)
            hbox.addWidget(value_label)
            solution_layout.addLayout(hbox)
            self.solution_labels.append(value_label)

        solution_group.setLayout(solution_layout)
        layout.addWidget(solution_group)

        # Определитель
        det_group = QGroupBox("Определитель матрицы")
        det_layout = QHBoxLayout()
        self.det_label = QLabel("не вычислен")
        self.det_label.setAlignment(Qt.AlignmentFlag.AlignRight)
        det_layout.addWidget(self.det_label)
        det_group.setLayout(det_layout)
        layout.addWidget(det_group)

        # Число обусловленности
        cond_group = QGroupBox("Число обусловленности")
        cond_layout = QHBoxLayout()
        self.cond_label = QLabel("не вычислено")
        self.cond_label.setAlignment(Qt.AlignmentFlag.AlignRight)
        cond_layout.addWidget(self.cond_label)
        cond_group.setLayout(cond_layout)
        layout.addWidget(cond_group)

        panel.setLayout(layout)
        return panel

    def connect_signals(self):
        self.solve_button.clicked.connect(self.on_solve_clicked)

        self.viewmodel.solution_changed.connect(self.update_solution)
        self.viewmodel.determinant_changed.connect(self.update_determinant)
        self.viewmodel.condition_changed.connect(self.update_condition)
        self.viewmodel.error_occurred.connect(self.show_error)

    def on_solve_clicked(self):
        try:
            # Собираем матрицу как двумерный список
            matrix_data = []
            for row in range(4):
                matrix_row = []
                for col in range(4):
                    matrix_row.append(float(self.matrix_inputs[row][col].text()))
                matrix_data.append(matrix_row)

            # Собираем вектор
            vector_data = [float(field.text()) for field in self.vector_inputs]

            if self.viewmodel.set_coefficients(matrix_data, vector_data):
                self.viewmodel.solve()
        except ValueError:
            self.show_error("Все поля должны содержать числовые значения")

    def update_solution(self, solution):
        for label, value in zip(self.solution_labels, solution):
            label.setText(f"{value:.6f}")

    def update_determinant(self, det):
        self.det_label.setText(f"{det:.6f}")

    def update_condition(self, cond):
        self.cond_label.setText(f"{cond:.6f}")

    def show_error(self, message):
        QMessageBox.warning(self, "Ошибка", message)