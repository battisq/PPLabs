// Вариант - 11
// Выбрать левый нижний треугольник матрицы.	

#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

void printMatrix(int* matrix, int rows, int cols);

int main(int argc, char** argv) {
	int rank, size;
	MPI_Status status;
	int i;

	int* arr1;
	int* arr2;

	int rows = strtoul(argv[1], NULL, 10), cols = rows;
	int* indices = (int*)calloc (rows, sizeof(int));	// Смещение каждого блока от начала типа
    int* blocklens = (int*)calloc (cols, sizeof(int));	// Кол-во элементов в каждом блоке
	int j, temp = 1;

	MPI_Datatype myType;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	printf("Numbers of proccesses %d. \nElements in matrix %d.\n", size,  rows * cols);

	for (int i = rows - 1, j = 0; i >= 0; i--, j++) {
		indices[j] = i * rows + j;
		blocklens[j] = cols - j;
	}

	int lengthResultArray = 0;

	for (int i = 0; i < rows; i++) {
		lengthResultArray += blocklens[i];
		// printf("indices[%d] = %d\t", i, indices[i]);
		// printf("blocklens[%d] = %d\t", i, blocklens[i]);
		// printf("\n");
	}

	// Идексный конструктор данных
	MPI_Type_indexed(
		rows, 		// Кол-во блоков
		blocklens, 	// Кол-во элементов в каждом блоке
		indices, 	// Смещение каждого блока от начала типа (в количестве элементов исходного типа)
		MPI_INT, 	// Исходный тип данных
		&myType		// Новый определяемый тип данных
	);	
	
	// Регистрация типа
	MPI_Type_commit(&myType);								

	arr1 = (int*)calloc (rows * cols, sizeof(int));
	arr2 = (int*)calloc (rows * cols, sizeof(int));

	int* vector = (int*)calloc(lengthResultArray, sizeof(int));

	if (rank == 0) {
		// Заполнение стартовой матрицы
		for (i = 0; i < rows; i++) {
			for (j = 0; j < cols; j++, temp++) {
				arr1[i * cols + j] = temp;
			}
        }

		MPI_Send(arr1, 1, myType, 1, 1, MPI_COMM_WORLD);
		MPI_Send(arr1, 1, myType, 1, 2, MPI_COMM_WORLD);
		
		MPI_Recv(arr2, 1, myType, 1, 0, MPI_COMM_WORLD, &status);

		printf("\nResult matrix\n");
		printf("Rank = %d\n", rank);
		printMatrix(arr2, rows, cols);
	}
	
    if (rank == 1) {
		for (i = 0; i < rows; i++) {
			for (j = 0; j < cols; j++) {
				arr1[i * cols + j] = 0;
			}
        }

		MPI_Recv(arr2, 1, myType, 0, 1, MPI_COMM_WORLD, &status);
		MPI_Recv(arr1, (rows * cols), MPI_INT, 0, 2, MPI_COMM_WORLD, &status);

		printf("\nRank = %d\n", rank);
		printMatrix(arr2, rows, cols);

		printf("vector of elements: \n");
		
		int k = 0;

		for (i = 0; i < rows; i++) {
			for (j = 0; j < cols; j++, k++) {
				if (arr1[i * cols + j] != 0) {
					vector[k] = arr1[i * cols + j];
					printf("%d ", arr1[i * cols + j]);
				}
			}
		}

		printf("\n");
		printf("\n");

		MPI_Send(vector, lengthResultArray, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}

	free(indices); 
	free(blocklens);
	free(arr1);	
	free(arr2);

	MPI_Type_free(&myType);
	MPI_Finalize();
	return 0;
}

void printMatrix(int* matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d\t", matrix[i * rows + j]);
        }
    
        printf("\n");
    }
    
    printf("\n");
}