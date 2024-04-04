#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <chrono>

#include <tbb/tbb.h>

using namespace std;
using namespace std::chrono;

const int N =6;

void printMatrix(double matrix[N][N + 1], int n) {
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n + 1; ++j) {
			cout << matrix[i][j] << " ";
		}
		cout << endl;
	}
}

void gaussEliminationTBB(double matrix[N][N + 1], int n) {
	for (int i = 0; i < n - 1; ++i) {
		tbb::parallel_for(tbb::blocked_range<int>(i + 1, n), [&](const tbb::blocked_range<int>& range) {
			cout << "begin index: " << range.begin() << ",  end index: " << range.end() << endl;
			for (int j = range.begin(); j < range.end(); ++j) {
				double koef = matrix[j][i] / matrix[i][i];
				for (int k = i; k < n + 1; ++k) {
					matrix[j][k] -= koef * matrix[i][k];
				}
			}
			});
	}

	for (int i = n - 1; i >= 0; --i) {
		matrix[i][n] /= matrix[i][i]; 
		matrix[i][i] = 1.0; 
		tbb::parallel_for(tbb::blocked_range<int>(0, i), [&](const tbb::blocked_range<int>& range) {
			for (int j = range.begin(); j < range.end(); ++j) {
				matrix[j][n] -= matrix[j][i] * matrix[i][n]; 
				matrix[j][i] = 0.0; 
			}
			});
	}

	cout << "\nSolution  OneTBB:" << endl;
	for (int i = 0; i < n; ++i) {
		cout << "x" << i + 1 << " = " << matrix[i][n] << endl;
	}
}


int main() {
	int n = N;
	double matrix[N][N + 1];
	srand(time(NULL));
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n + 1; ++j) {
			matrix[i][j] = rand() % 10 + 1;
		}
	}	

	auto startTBB = high_resolution_clock::now();
	gaussEliminationTBB(matrix, n);
	auto stopTBB = high_resolution_clock::now();
	auto durationTBB = duration_cast<microseconds>(stopTBB - startTBB);
	std::cout << std::endl << std::endl;
	cout << "Time Gauss  with OneTBB: " << durationTBB.count() << " microseconds" << endl;

	return 0;
}
