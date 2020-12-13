// Вариант - 11
// Вычислить поэлементное умножение элементов двух матриц Y = X * Y

#include "mpi.h"
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

void mul(double* a, double* b, int sproc, double* out) {
	for (int i = 0; i < sproc; i++) {
		out[i] = a[i] * b[i];
	}
}

double* genMatrix(int rows, int cols, int min, int max) {
	srand(time(0));

    double* matrix = (double*)malloc(rows * cols * sizeof(double));

    for (int i = 0; i < rows * cols; i++) {
		matrix[i] = (double)rand() / RAND_MAX * (max - min) + min * 0.36;
	}

    return matrix;
}

void printMatrix(double* matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%8.2lf  ", matrix[i * rows + j]);
        }
    
        printf("\n");
    }
    
    printf("\n");
}

void print(int* arr, int size) {
    for (int i = 0; i < size; i++) {
        printf("%8d", arr[i]);
    }
}

void main(int argc, char* argv[]) {
	if(argc < 3) {
        return;
    }
    
    int rows, cols, myid, size;
	double startwtime, endwtime;
	int part, ost;
	double *a, *b;
	double *temp1, *temp2, *res, *out;
	int *displs, *rcounts;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	MPI_Status status;

	MPI_Init(&argc, &argv);                 //инициализация МПИ программ
	MPI_Comm_size(MPI_COMM_WORLD, &size);   //опредление количества процессов
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);   //опредление ранга процесса

    rows = strtoul(argv[1], NULL, 10);      // Кол-во строк из char в int
    cols = strtoul(argv[2], NULL, 10);      // Кол-во столбцов из char в int

	part = (rows * cols) / size;            // Опеделени сколько целочисленно будет в каждом из процессов
	ost = (rows * cols) % size;             // Остаток после целочисленного деления (меньше чем в других процессах)

    if (myid == 0) {

        printf("Numbers of proccesses %d. \nElements in matrix %d x %d.\n", size, rows, cols);

        a = genMatrix(rows, cols, -2, 11);
        b = genMatrix(rows, cols, 2, 20);

        printMatrix(a, rows, cols);
        printMatrix(b, rows, cols);

        rcounts = (int*)malloc(size * sizeof(int));     // Кол-во элементов в конкретном процессе
        displs = (int*)malloc(size * sizeof(int));      // Сдвиги для каждого из процессов

        int sproc, pointer;

        for (int i = 0; i < size; i++) {
            sproc = i < ost ? part + 1 : part;          // Определение кол-ва элементов в процессах
            rcounts[i] = sproc;
            
            pointer = i * sproc + (i >= ost ? ost : 0); // Определение сдвига
            displs[i] = pointer;
        }

        print(rcounts, size);
        printf(" rcounts \n");
        
        print(displs, size);
        printf(" displs \n");
    }

    int sproc, pointer;

    sproc = myid < ost ? part + 1 : part;
    pointer = myid * sproc + (myid >= ost ? ost : 0);
    
    temp1 = (double*)malloc(sproc * sizeof(double));
    temp2 = (double*)malloc(sproc * sizeof(double));
    out = (double*)malloc(sproc * sizeof(double));
    res = (double*)malloc(rows * cols * sizeof(double));

    // Передача данных от одного процесса всем процессам
    MPI_Scatterv(
        a,                  // Массив данных
        rcounts,            // Кол-во элементов передаваемых каждому процессу
        displs,             // Смещение относительно начала для каждого процесса
        MPI_DOUBLE,         // Рассылаемый тип
        temp1,              // Массив, принимающий порцию данных в i-ом процессе
        sproc,              // Размер порции, принимаемой в ранге адресата
        MPI_DOUBLE,         // Принимаемый тип
        0,                  // Ранг процесса, выполняемого рассылку
        MPI_COMM_WORLD      // Коммуникатор, в рамках которого выполняется передача данных (глобальный коммуникатор, содержащий все процессы)
    );
    
    MPI_Scatterv(b, rcounts, displs, MPI_DOUBLE, temp2, sproc, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    if (myid == 0) {
        startwtime = MPI_Wtime();
    }

    printf("Process %d sproc %d pointer %d\n", myid, sproc, pointer);
    
    mul(temp1, temp2, sproc, out);
    
    // Сборка различного количества данных со всех процессов в один процесс
    MPI_Gatherv(
        out,                // Адрес рассылаемых данных в процессе отправителе (адрес в i-ом процессе)
        sproc,              // Число рассылаемых данных
        MPI_DOUBLE,         // Рассылаемый тип
        res,                // Адрес буфера в принимающем процессе (в 0-ом)
        rcounts,            // Кол-во элементов ппринимаемых от каждого процесса 
        displs,             // Смещение относительно начала для каждого процесса
        MPI_DOUBLE,         // Принимаемый тип
        0,                  // Ранг принимающего процесса
        MPI_COMM_WORLD      // Глобальный коммуникатор, содержащий все процессы
    );

    if (myid == 0) {
        endwtime = MPI_Wtime();
        printf("\nTime parallel calculation = %f s.\n", endwtime - startwtime);
        
        printMatrix(res, rows, cols);      
        free(res);      
    }

    free(temp1);
    free(temp2);
    free(out);
				
	MPI_Finalize(); //завершение МПИ программ
}