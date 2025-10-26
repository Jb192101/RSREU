package org.jedi_bachelor.orders.model;

import org.jedi_bachelor.orders.math.FuzzyLogic;
import org.jedi_bachelor.orders.math.MArray;

import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;

public class MainLogic {
    private List<PairedComparionsMatrix> matrixList;
    private Scanner scanner;

    public MainLogic() {
        this.matrixList = new ArrayList<>();
        this.scanner = new Scanner(System.in);
    }

    public void startApp() {
        System.out.println("=== Система нечетких попарных сравнений ===");

        while(true) {
            showMenu();
            int choice = getIntInput("Выберите действие: ");

            switch(choice) {
                case 1:
                    createNewMatrix();
                    break;
                case 2:
                    showAllMatrices();
                    break;
                case 3:
                    performLogicalOperations();
                    break;
                case 4:
                    editMatrix();
                    break;
                case 5:
                    calculateHeight();
                    break;
                case 6:
                    calculateSymmetricalDifference();
                    break;
                case 0:
                    System.out.println("Выход из программы...");
                    return;
                default:
                    System.out.println("Неверный выбор!");
            }
        }
    }

    private void showMenu() {
        System.out.println("\n--- Меню ---");
        System.out.println("1. Создать новую матрицу");
        System.out.println("2. Показать все матрицы");
        System.out.println("3. Выполнить логические операции");
        System.out.println("4. Редактировать матрицу");
        System.out.println("5. Вычислить высоту матрицы");
        System.out.println("6. Симметричная разность (сложение по модулю 2)");
        System.out.println("0. Выход");
    }

    private void calculateHeight() {
        if (matrixList.isEmpty()) {
            System.out.println("Нет созданных матриц!");
            return;
        }

        System.out.println("\n--- Вычисление высоты матрицы ---");
        showMatrixList();

        int index = getIntInput("Выберите матрицу для вычисления высоты: ") - 1;

        if (index < 0 || index >= matrixList.size()) {
            System.out.println("Неверный индекс!");
            return;
        }

        PairedComparionsMatrix matrix = matrixList.get(index);
        Float[][] mainMatrix = matrix.getMainMatrix();

        float height = FuzzyLogic.getHeightOfMatrix(mainMatrix);

        System.out.println("\n--- Результат вычисления высоты ---");
        System.out.println("Матрица: " + matrix.getNameOfMatrix());
        System.out.printf("Высота матрицы: %.4f\n", height);

        System.out.println("\nMFC значения матрицы:");
        for (int i = 0; i < matrix.getTitles().length; i++) {
            System.out.printf("%s: %.4f\n", matrix.getTitles()[i], matrix.getMfc().getValue(i));
        }
    }

    private void calculateSymmetricalDifference() {
        if (matrixList.size() < 2) {
            System.out.println("Нужно как минимум 2 матрицы для операции!");
            return;
        }

        System.out.println("\n--- Симметричная разность (сложение по модулю 2) ---");
        showMatrixList();

        int index1 = getIntInput("Выберите первую матрицу: ") - 1;
        int index2 = getIntInput("Выберите вторую матрицу: ") - 1;

        if (index1 < 0 || index1 >= matrixList.size() ||
                index2 < 0 || index2 >= matrixList.size()) {
            System.out.println("Неверные индексы!");
            return;
        }

        PairedComparionsMatrix matrix1 = matrixList.get(index1);
        PairedComparionsMatrix matrix2 = matrixList.get(index2);

        if (matrix1.getMfc().getSize() != matrix2.getMfc().getSize()) {
            System.out.println("Матрицы должны быть одного размера!");
            return;
        }

        MArray mfc1 = matrix1.getMfc();
        MArray mfc2 = matrix2.getMfc();

        MArray symmetricalDiff = FuzzyLogic.symmetricalDifference(mfc1, mfc2);

        System.out.println("\n--- Результат симметричной разности ---");
        System.out.println("Матрица 1: " + matrix1.getNameOfMatrix());
        System.out.println("Матрица 2: " + matrix2.getNameOfMatrix());
        System.out.println("\nСимметричная разность (A ⊕ B):");
        printMArrayResult(symmetricalDiff, matrix1.getTitles());

        System.out.println("\nИсходные MFC значения:");
        System.out.println("Матрица 1:");
        printMArrayResult(mfc1, matrix1.getTitles());
        System.out.println("\nМатрица 2:");
        printMArrayResult(mfc2, matrix1.getTitles());
    }

    private void createNewMatrix() {
        System.out.println("\n--- Создание новой матрицы ---");

        String name = getStringInput("Введите название матрицы: ");
        int size = getIntInput("Введите размер матрицы: ");

        if (size <= 0) {
            System.out.println("Размер матрицы должен быть положительным!");
            return;
        }

        String[] titles = new String[size];
        for (int i = 0; i < size; i++) {
            titles[i] = getStringInput("Введите название элемента " + (i + 1) + ": ");
        }

        PairedComparionsMatrix matrix = new PairedComparionsMatrix(size, titles, name);

        fillMatrixValues(matrix);

        matrixList.add(matrix);
        System.out.println("Матрица успешно создана!");
    }

    private void fillMatrixValues(PairedComparionsMatrix matrix) {
        String[] titles = matrix.getTitles();
        int size = titles.length;

        System.out.println("\nЗаполнение матрицы значений (введите 0 чтобы пропустить):");

        for (int i = 0; i < size; i++) {
            for (int j = i + 1; j < size; j++) {
                System.out.printf("\nСравнение: %s vs %s\n", titles[i], titles[j]);
                System.out.println("Введите значение от 1 до 10 (насколько первый важнее второго):");
                float value = getFloatInput("Значение: ");

                if (value > 0) {
                    matrix.setValue(i, j, value);
                    System.out.printf("Установлено: %s -> %s = %.2f, %s -> %s = %.2f\n",
                            titles[i], titles[j], value,
                            titles[j], titles[i], 1/value);
                }
            }
        }
    }

    private void showAllMatrices() {
        if (matrixList.isEmpty()) {
            System.out.println("Нет созданных матриц!");
            return;
        }

        System.out.println("\n--- Все матрицы ---");
        for (int i = 0; i < matrixList.size(); i++) {
            System.out.println("\n" + (i + 1) + ". " + matrixList.get(i));
        }
    }

    private void editMatrix() {
        if (matrixList.isEmpty()) {
            System.out.println("Нет созданных матриц для редактирования!");
            return;
        }

        showMatrixList();
        int index = getIntInput("Выберите матрицу для редактирования: ") - 1;

        if (index < 0 || index >= matrixList.size()) {
            System.out.println("Неверный индекс!");
            return;
        }

        PairedComparionsMatrix matrix = matrixList.get(index);
        fillMatrixValues(matrix);
        System.out.println("Матрица обновлена!");
    }

    private void performLogicalOperations() {
        if (matrixList.size() < 2) {
            System.out.println("Нужно как минимум 2 матрицы для операций!");
            return;
        }

        System.out.println("\n--- Логические операции ---");
        showMatrixList();

        int index1 = getIntInput("Выберите первую матрицу: ") - 1;
        int index2 = getIntInput("Выберите вторую матрицу: ") - 1;

        if (index1 < 0 || index1 >= matrixList.size() ||
                index2 < 0 || index2 >= matrixList.size()) {
            System.out.println("Неверные индексы!");
            return;
        }

        PairedComparionsMatrix matrix1 = matrixList.get(index1);
        PairedComparionsMatrix matrix2 = matrixList.get(index2);

        if (matrix1.getMfc().getSize() != matrix2.getMfc().getSize()) {
            System.out.println("Матрицы должны быть одного размера!");
            return;
        }

        MArray mfc1 = matrix1.getMfc();
        MArray mfc2 = matrix2.getMfc();

        System.out.println("\n--- Результаты операций ---");

        // Логическое И
        MArray andResult = FuzzyLogic.logicalAnd(mfc1, mfc2);
        System.out.println("Логическое И (AND):");
        printMArrayResult(andResult, matrix1.getTitles());

        // Логическое ИЛИ
        MArray orResult = FuzzyLogic.logicalOr(mfc1, mfc2);
        System.out.println("\nЛогическое ИЛИ (OR):");
        printMArrayResult(orResult, matrix1.getTitles());

        // Логическое НЕ для первой матрицы
        MArray notResult = FuzzyLogic.logicalNot(mfc1);
        System.out.println("\nЛогическое НЕ (NOT) для первой матрицы:");
        printMArrayResult(notResult, matrix1.getTitles());
    }

    private void printMArrayResult(MArray mArray, String[] titles) {
        for (int i = 0; i < mArray.getSize(); i++) {
            System.out.printf("%s: %.4f\n", titles[i], mArray.getValue(i));
        }
    }

    private void showMatrixList() {
        System.out.println("\n--- Список матриц ---");
        for (int i = 0; i < matrixList.size(); i++) {
            System.out.println((i + 1) + ". " + matrixList.get(i).getNameOfMatrix() +
                    " (размер: " + matrixList.get(i).getTitles().length + ")");
        }
    }

    // Вспомогательные методы для ввода
    private int getIntInput(String prompt) {
        while (true) {
            try {
                System.out.print(prompt);
                return Integer.parseInt(scanner.nextLine());
            } catch (NumberFormatException e) {
                System.out.println("Ошибка! Введите целое число.");
            }
        }
    }

    private float getFloatInput(String prompt) {
        while (true) {
            try {
                System.out.print(prompt);
                return Float.parseFloat(scanner.nextLine());
            } catch (NumberFormatException e) {
                System.out.println("Ошибка! Введите число.");
            }
        }
    }

    private String getStringInput(String prompt) {
        System.out.print(prompt);
        return scanner.nextLine();
    }
}