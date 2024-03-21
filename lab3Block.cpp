#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>


const int n = 3000;
double matrix[n][n + 1], matrixB[n][n + 1];

void Gauss(double matrix[n][n + 1]) {
	//Прямой обход
	for (int i = 0; i < n; ++i) {
		for (int j = i + 1; j < n; ++j) {
			double koef = matrix[j][i] / matrix[i][i];
			for (int k = i; k < n + 1; ++k) {
				matrix[j][k] -= koef * matrix[i][k];
			}
		}
	}
	//Обратный обход
	for (int i = n - 1; i >= 0; --i) {
		matrix[i][n] /= matrix[i][i];
		matrix[i][i] = 1.0;
		for (int j = 0; j < i; ++j) {
			matrix[j][n] -= matrix[j][i] * matrix[i][n];
			matrix[j][i] = 0.0;
		}
	}
}

void GaussOMP(double matrix[n][n + 1]) {
	//Прямой обход
	for (int i = 0; i < n; ++i) {
#pragma omp parallel for 
		for (int j = i + 1; j < n; ++j) {
			double koef = matrix[j][i] / matrix[i][i];
			for (int k = i; k < n + 1; ++k) {
				matrix[j][k] -= koef * matrix[i][k];
			}
		}
	}
	//Обратный обход
	for (int i = n - 1; i >= 0; --i) {
		matrix[i][n] /= matrix[i][i];
		matrix[i][i] = 1.0;
#pragma omp parallel for
		for (int j = 0; j < i; ++j) {
			matrix[j][n] -= matrix[j][i] * matrix[i][n];
			matrix[j][i] = 0.0;
		}
	}
}


void printMatrix(int matrix[n][n]) {
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j) {
			std::cout << matrix[i][j] << " ";
		}
		std::cout << std::endl;
	}
}


int main() {
	setlocale(LC_ALL, "RUSSIAN");
	//  std::cout << "Размерность матрицы: " << n << std::endl;

	srand(time(0));
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n + 1; ++j) {
			matrix[i][j] = rand() % 10 + 1;
		    matrixB[i][j] = matrix[i][j];
		}
	}


	auto firstTimePoint = std::chrono::high_resolution_clock::now();
	Gauss(matrix);
	auto secondTimePoint = std::chrono::high_resolution_clock::now();
	auto result = std::chrono::duration_cast<std::chrono::microseconds>(secondTimePoint - firstTimePoint);
	std::cout << "Без OpenMP: " << result.count() << " mcrSec" << std::endl;


	firstTimePoint = std::chrono::high_resolution_clock::now();
	GaussOMP(matrixB);
	secondTimePoint = std::chrono::high_resolution_clock::now();
	auto  resultOMP = std::chrono::duration_cast<std::chrono::microseconds>(secondTimePoint - firstTimePoint);
	std::cout << "С OpenMP: " << resultOMP.count() << " mcrSec" << std::endl;
	std::cout << "Разница: " << result.count() - resultOMP.count() << " mcrSec" << std::endl << std::endl;

	return 0;
}
