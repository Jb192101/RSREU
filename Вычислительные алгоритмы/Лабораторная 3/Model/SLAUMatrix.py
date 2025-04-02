from copy import deepcopy
import math

class MatrixModel:
    def __init__(self):
        self.matrix = [[0.0, 0.0, 0.0, 0.0],
                       [0.0, 0.0, 0.0, 0.0],
                       [0.0, 0.0, 0.0, 0.0],
                       [0.0, 0.0, 0.0, 0.0]]
        self.vector = [0.0, 0.0, 0.0, 0.0]
        self.solution = [0.0, 0.0, 0.0, 0.0]
        self.determinant = 0.0
        self.condition_number = 0.0

    def set_coefficients(self, _matrixData, _vectorData):
        self.matrix = _matrixData
        self.vector = _vectorData

    # Метод для всех методов #

    def solve_system(self):
        self.solution, acc = self.solve(deepcopy(self.matrix), deepcopy(self.vector))
        self.determinant = self.determinator(self.matrix)
        self.condition = self.cond(self.matrix)
        print(self.solution)
        print(self.determinant)
        print(self.condition_number)
        return True

    # ПУБЛИЧНЫЕ МЕТОДЫ #

    def solve(self, matrix2, vec_b2):
        matrix = deepcopy(matrix2)
        vec_b = deepcopy(vec_b2)
        matrix, vec_b = self.gauss(matrix, vec_b)
        print(matrix)
        print(vec_b)

        alpha = [[0.0, 0.0, 0.0, 0.0],
                 [0.0, 0.0, 0.0, 0.0],
                 [0.0, 0.0, 0.0, 0.0],
                 [0.0, 0.0, 0.0, 0.0]]
        beta = [0.0, 0.0, 0.0, 0.0]
        for i in range(4):
            print(vec_b[i])
            print(matrix[i][i])
            beta[i] = round(vec_b[i] / matrix[i][i], 10)
            print(beta[i])

        for i in range(4):
            for j in range(4):
                if i != j:
                    alpha[i][j] = -1* round(matrix[i][j] / matrix[i][i], 10)
                else:
                    alpha[i][j] = 0

        x = [0.0, 0.0, 0.0, 0.0]
        inaccuracy = float("inf")
        while inaccuracy >= 10**(-2):
            x_prev = deepcopy(x)
            x = self.vectors_sum(beta, self.matrix_vector_multiply(alpha, x))
            inaccuracy = self.difference_between_vectors(x, x_prev)
        return x, inaccuracy

    def determinator(self, matrix):
        v = [0.0, 0.0, 0.0, 0.0]
        matrix, bVector = self.gauss(matrix, v)
        res = 1
        for i in range(len(matrix)):
            res *= self.matrix[i][i]
        return round(res, 5)

    def cond(self, matrix):
        inverseMat = self.gauss_jordan(deepcopy(matrix))
        aNorm = float("-inf")
        for row in matrix:
            s = 0
            for item in row:
                s += abs(item)
            aNorm = max(aNorm, s)

        inverseNorm = float("-inf")
        for row in inverseMat:
            s = 0
            for item in row:
                s += abs(item)
            inverseNorm = max(inverseNorm, s)
        cond = aNorm * inverseNorm
        return round(cond, 5)

    # ПРИВАТНЫЕ МЕТОДЫ #
    def difference_between_vectors(self, x1, x2):
        if len(x1) != len(x2):
            raise ValueError("Размеры векторов не совпадают.")
        diff = 0
        for i in range(len(x1)):
            diff += abs(x1[i] - x2[i])
        return diff

    def vectors_sum(self, x1, x2):
        if len(x1) != len(x2):
            raise ValueError("Размеры векторов не совпадают.")
        res = [x1[i] + x2[i] for i in range(len(x1))]
        return res

    def vectors_dif(self, x1, x2):
        if len(x1) != len(x2):
            raise ValueError("Размеры векторов не совпадают.")
        res = [x1[i] - x2[i] for i in range(len(x1))]
        return res

    def vector_num_mul(self, v, n):
        return [round(t * n, 9) for t in v]

    def matrix_vector_multiply(self, A, v):
        if len(A[0]) != len(v):
            raise ValueError("Размеры матрицы и вектора не позволяют их перемножить.")

        result = [sum(A[i][j] * v[j] for j in range(len(v))) for i in range(len(A))]
        return result

    def gauss(self, matrix, vec_b):
        for i in range(len(matrix)):
            #matrix[i] += [vec_b[i]]
            matrix[i].append(vec_b[i])

        for i in range(len(matrix)):
            max_el = abs(matrix[i][i])
            max_row = i
            for k in range(i + 1, len(matrix)):
                if abs(matrix[k][i]) > max_el:
                    max_el = abs(matrix[k][i])
                    max_row = k
            matrix[i], matrix[max_row] = matrix[max_row], matrix[i]

            mat_str = [x for x in matrix[i]]
            for j in range(i + 1, len(matrix)):
                if matrix[i][i] != 0:
                    cur = self.vector_num_mul(mat_str, matrix[j][i] / matrix[i][i])
                    matrix[j] = self.vectors_dif(matrix[j], cur)

        for i in range(len(matrix)):
            vec_b[i] = matrix[i].pop()

        return matrix, vec_b

    def gauss_jordan(self, matrix):
        n = len(matrix)

        E = [[0] * len(matrix[0]) for _ in range(n)]
        for i in range(len(E)):
            E[i][i] = 1

        for i in range(n):
            matrix[i] += E[i]

        for i in range(n):
            max_el = abs(matrix[i][i])
            max_row = i
            for k in range(i + 1, n):
                if abs(matrix[k][i]) > max_el:
                    max_el = abs(matrix[k][i])
                    max_row = k

            matrix[i], matrix[max_row] = matrix[max_row], matrix[i]

            pivot = matrix[i][i]
            if pivot == 0:
                raise ValueError("Матрица необратима.")
            matrix[i] = [element / pivot for element in matrix[i]]

            for j in range(n):
                if j != i:
                    factor = matrix[j][i]
                    matrix[j] = [matrix[j][k] - factor * matrix[i][k] for k in range(2 * n)]

        inverse_matrix = [row[n:] for row in matrix]
        return inverse_matrix