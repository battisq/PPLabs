/*
 * Example program to calculate of integral
 * f(x)=log (1/a) 
 */

#include <stdio.h>
#include <math.h>
#include "mpi.h"




/* Prototype the function that we'll use below. */

static double f(double);
static double fi(double);

int
main(int argc, char *argv[])
{
  double a = 0.5, b = 4.0, Eps;
  int done = 0;
  int i, n, rank, size, Ierr, namelen;
  double Sum, Gsum, Isum, time1, time2,a1, b1, x, F;
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
    if (rank == 0)  {
    printf("Enter Eps ( > 0.1 - quit) "); fflush(stdout);
    scanf("%lf",&Eps); printf("%10.9lf\n",Eps);
      time1 = MPI_Wtime();
    /* Sending Eps to over nodes */

    for (i=1; i < size; i++)
            {
    MPI_Send (&Eps,                /* buffer               */
               1,                  /* one data            */
               MPI_DOUBLE,         /* type                */
               i,                  /* to which node       */
               1,                  /* number of message   */
               MPI_COMM_WORLD);    /* common communicator  */
            }
        }

        else
        {
    MPI_Recv  (&Eps,                 /* buffer               */
               1,                 /* one data            */
               MPI_DOUBLE,           /* type                */
               0,                 /* to which node       */
               1,                 /* number of message   */
               MPI_COMM_WORLD,   /* common communicator  */
               &stats);
        }
    if(Eps > 0.1) done = 1;
     else
       {

 /* Each process define his integration limit 
    and a number of intervals */
    
    a1 = a+(b-a)*rank/size;
    b1 = a1+(b-a)/size;
//    n = 0.35/(Eps*size);
    n = 1.0/(Eps*size);
 /* Calculating of a local sum by each process */
 
    Sum = 0.0;
    for (i=1; i<=n; i++)
       { x = a1+(b1-a1)*(i-0.5)/n;
         Sum+= f(x);  }
  printf("Process %d SUMM  %.16f \n", rank, Sum/(n*size)*(b-a));    

    /* Sending the local sum to node 0 */
    if (rank != 0)    
    MPI_Send (&Sum,                /* buffer               */
               1,                  /* one data            */
	       MPI_DOUBLE,         /* type                */
	       0,                  /* to which node       */
	       1,                  /* number of message   */
	       MPI_COMM_WORLD);    /* common communicator  */
    
    if (rank == 0) {
       Gsum = Sum;
       for (i=1; i<size; i++)
         { MPI_Recv (&Isum,
	             1,
		     MPI_DOUBLE,
		     i,
		     1,
		     MPI_COMM_WORLD,
		     &stats);
	   Gsum += Isum;  };

       Gsum = Gsum/(n*size)*(b-a);
    	   	     
      printf("\nIntegral of function ln(1/x) from %5.2f to %5.2f.\n",
               a, b);  
      printf("%d point. Integral = %18.16f, error = %18.16f.\n\n",
	     n*size, Gsum, fi(b) - fi(a) - Gsum);
       time2 = MPI_Wtime();
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
  return (log(1/a));
}

static double fi(double a)
{
  return ((log(1/a)+1)*a);
}
