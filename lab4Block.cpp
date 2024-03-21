#include <iostream>
#include <mpi.h>
#include <chrono>

const int n = 3;
int firstMatrix[n][n];
int secondMatrix[n][n];
int resultMatrix[n][n];
int receivedResultMatrix[n][n];
std::chrono::high_resolution_clock::time_point firstTimePoint, secondTimePoint;


void printMatrix(int matrix[n][n]) {
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j) {
			std::cout << matrix[i][j] << " ";
		}
		std::cout << std::endl;
	}
}

int main(int argc, char* argv[]) {
	setlocale(LC_ALL, "RUSSIAN");

	int rank, size;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (rank == 0) {
		firstTimePoint = std::chrono::high_resolution_clock::now();
	}

	srand(time(NULL));
	if (rank == 0) {
		for (int i = 0; i < n; ++i) {
			for (int j = 0; j < n; ++j) {
				firstMatrix[i][j] = rand() % 100;
				secondMatrix[i][j] = rand() % 100;
				resultMatrix[i][j] = 0;
				receivedResultMatrix[i][j] = 0;
			}
		}

		//printMatrix(firstMatrix);
		//std::cout << std::endl;
		//printMatrix(secondMatrix);
	}

	MPI_Bcast(firstMatrix, n * n, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(secondMatrix, n * n, MPI_INT, 0, MPI_COMM_WORLD);

	int rowProcess = n / size;
	int extraRows = n % size;
	int startRow, endRow;

	if (size >= n) {
		if (rank < n) {
			rowProcess = 1;
			startRow = rank;
			endRow = rank + 1;
		}
		else {
			rowProcess = 0;
			startRow = 0;
			endRow = 0;
		}
	}
	else {
		if (rank < extraRows) {
			rowProcess++;
			startRow = rank * rowProcess;
		}
		else {
			startRow = rank * rowProcess + extraRows;
		}
		endRow = startRow + rowProcess;
	}

	for (int i = startRow; i < endRow; ++i) {
		for (int j = 0; j < n; ++j) {
			for (int k = 0; k < n; ++k) {
				resultMatrix[i][j] += firstMatrix[i][k] * secondMatrix[k][j];
				//std::cout << "Rank: " << rank << " Element: " << i << ":" << j << " Result: " << resultMatrix[i][j] << std::endl;
			}

		}
	}
	MPI_Reduce(resultMatrix, receivedResultMatrix, n * n, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		secondTimePoint = std::chrono::high_resolution_clock::now();
		std::cout << "Multiplication time: " << std::chrono::duration_cast<std::chrono::microseconds>(secondTimePoint - firstTimePoint).count() << " microseconds" << std::endl;
		std::cout << "Result matrix:" << std::endl;
		std::cout << std::endl;
		//printMatrix(receivedResultMatrix);
	}

	MPI_Finalize();
	return 0;
}





