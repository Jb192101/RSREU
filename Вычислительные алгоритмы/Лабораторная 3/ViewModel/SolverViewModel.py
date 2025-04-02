from PyQt6.QtCore import QObject, pyqtSignal
from Model.MModel import SLAUModel


class SolverViewModel(QObject):
    # Сигналы с параметрами
    solution_changed = pyqtSignal(list, int)  # [значения], точность
    determinant_changed = pyqtSignal(float, int)  # значение, точность
    condition_changed = pyqtSignal(float, int)  # значение, точность
    error_occurred = pyqtSignal(str, str)  # заголовок, сообщение

    def __init__(self, precision=6):
        super().__init__()
        self.model = SLAUModel()
        self.precision = precision

    def set_coefficients(self, matrix_data, vector_data):
        try:
            if len(matrix_data) != 4 or any(len(row) != 4 for row in matrix_data):
                raise ValueError("Матрица должна быть 4x4")
            if len(vector_data) != 4:
                raise ValueError("Вектор должен содержать 4 элемента")

            self.model.set_coefficients(matrix_data, vector_data)
            return True
        except Exception as e:
            self.error_occurred.emit(
                "Ошибка ввода",
                f"Некорректные данные: {str(e)}"
            )
            return False

    def solve(self, show_progress=False):
        if show_progress:
            print("Вычисление начато...")

        if self.model.solve_system():
            self.solution_changed.emit(
                self.model.solution.tolist(),
                self.precision
            )
            self.determinant_changed.emit(
                self.model.determinant,
                self.precision
            )
            self.condition_changed.emit(
                self.model.condition_number,
                self.precision
            )
        else:
            self.error_occurred.emit(
                "Ошибка решения",
                "Система не имеет решения"
            )
