#include <stdio.h>
#include <math.h>
#include "mpi.h"




/* Prototype the function that we'll use below. */

static double f(double);
static double fi(double);
const double c = 0.8;

int main(int argc, char* argv[])
{
    double a = -0.5, b = 1.5, Eps;
    int done = 0;
    int i, n, rank, size, Ierr, namelen;
    double Sum, Gsum, Isum, time1, time2, a1, b1, x, F, res;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Status  stats;
    /* Normal MPI startup */

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Get_processor_name(processor_name, &namelen);

    printf("Process %d of %d on %s\n", rank, size, processor_name);

    Eps = 1.0;
    while (!done)
    {
        if (rank == 0) 
        {
            printf("Enter Eps ( > 0.1 - quit) "); fflush(stdout);
            scanf("%lf", &Eps); printf("%10.9lf\n", Eps);
            time1 = MPI_Wtime();
		}
            /* Sending Eps to over nodes */
            MPI_Bcast(&Eps, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD); 

        if (Eps > 0.1) 
        	done = 1;
        else
        {

            /* Each process define his integration limit
               and a number of intervals */

            a1 = a + (b - a) * rank / size;
            b1 = a1 + (b - a) / size;
            n = 1.0 / (Eps * size);
            /* Calculating of a local sum by each process */

            Sum = 0.0;
            for (i = 1; i <= n; i++)
            {
                x = a1 + (b1 - a1) * (i - 0.5) / n;
                Sum += f(x);
            }

            /* Sending the local sum to node 0 */
            MPI_Reduce(&Sum, &Gsum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
            

            if (rank == 0) {
		        Gsum = Gsum / (n * size) * (b - a);
                time2 = MPI_Wtime();
                printf("\nIntegral of function cos(c * a)^2 from %5.2f to %5.2f.\n",
                    a, b);
                printf("%d point. Integral = %18.16f, error = %18.16f.\n\n",
                    n * size, Gsum, fi(b) - fi(a) - Gsum);
                printf("Time calculation = %f seconds.\n", time2 - time1);
                fflush(stdout);
            }
        }
    }
    /* All done */

    MPI_Finalize();
    return 0;
}

/* Define function  */
static double f(double a)
{
    return pow(cos(c * a), 2);
}

static double fi(double a)
{
    return 0.5 * a + (0.25 / c) * sin(2 * c * a);
}