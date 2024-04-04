#include <iostream>
#include <mpi.h>
#include <cstdlib>
#include <ctime>
const int N = 10;

void GaussMPI(double matrix[N][N + 1], double solution[N], int rank, int size) {
	
	int rowProcess = N / size;
	int extraRows = N % size;
	int startRow, endRow;

	if (size >= N) {
		if (rank < N) {
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
	// Прямой ход
	for (int k = 0; k < N - 1; k++) {
		for (int i = startRow; i < endRow; i++) {
			if (i > k) {
				double factor = matrix[i][k] / matrix[k][k];
				for (int j = k; j < N + 1; j++) {
					matrix[i][j] -= factor * matrix[k][j];
				}
				std::cout << "Thread " << rank << " row #" << i << " factor: " << factor << std::endl;
				for (int m = 0; m < N; m++) {
					for (int n = 0; n < N + 1; ++n) {
						std::cout << matrix[m][n] << " ";
					}
					std::cout << std::endl;
				}
			}

		}
		MPI_Allgather(MPI_IN_PLACE, 0, MPI_DATATYPE_NULL, matrix, rowProcess * (N + 1), MPI_DOUBLE, MPI_COMM_WORLD);
	}

	// Обратный ход
	if (rank == 0) {
		std::cout << std::endl;
		std::cout << std::endl;

		for (int m = 0; m < N; m++) {
			for (int n = 0; n < N + 1; ++n) {
				std::cout << matrix[m][n] << " ";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;

		for (int i = N - 1; i >= 0; i--) {
			solution[i] = matrix[i][N];
			for (int j = i + 1; j < N; j++) {
				solution[i] -= matrix[i][j] * solution[j];
			}
			solution[i] /= matrix[i][i];
		}
	}
}




//double matrix[N][N + 1] = { {1, 2, -1, 2}, {2, -3, 2, 2}, {3, 1, 1, 8} };
//double matrix[N][N + 1] = { {1, 2, -1, 2,1}, {2, -3, 2, 2,4}, {3, 1, 1, 8,1}, {2, 3, 4, 1,6} };

int main(int argc, char** argv) {
	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (size > N) {
		if (rank == 0) {
			std::cout << "Error" << std::endl;
		}
		MPI_Finalize();
		return 0;
	}

	double matrix[N][N + 1];

	if (rank == 0) {
		srand(static_cast<unsigned int>(time(nullptr)));
		 for (int i = 0; i < N; i++) {
			 for (int j = 0; j < N + 1; ++j) {
				 matrix[i][j] = rand() % 10 + 1;
			 }
		 }

		std::cout << "Matrix:" << std::endl;
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N + 1; ++j) {
				std::cout << matrix[i][j] << " ";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}

	for (int i = 0; i < N; i++) {
		MPI_Bcast(matrix[i], N + 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	}

	double solution[N];
	GaussMPI(matrix, solution, rank, size);
	if (rank == 0) {
		std::cout << "Solution:" << std::endl;
		for (int i = 0; i < N; i++) {
			std::cout << "x[" << i << "] = " << solution[i] << std::endl;
		}
		std::cout << std::endl;
	}

	MPI_Finalize();
	return 0;
}

