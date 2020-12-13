#include "mpi.h"
#include <stdio.h>
#include <math.h>


static double f(double a);
static double fi(double a);

void main(int argc, char *argv[])
{
    int done = 0, n, myid, numprocs, i;
    double myfunk, funk, h, sum, x;
    double xl = -0.5,	// low  border
	   xh =  0.8;	// high border
    double startwtime, endwtime;
    int  namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Status stats;
    	
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);
    MPI_Get_processor_name(processor_name,&namelen);
	
    fprintf(stderr,"Process %d on %s\n",
		myid, processor_name);
	fflush(stderr);
	
    n = 0;
    while (!done)
    {
        if (myid == 0)
        {
	    printf("Enter the number of intervals (0 quit) ");
	    fflush(stdout);
	    scanf("%d",&n);
	    
	    startwtime = MPI_Wtime();
        
    /* Sending the number of intervals to other nodes */

    for (i=1; i < numprocs; i++)
            {
    MPI_Send (&n,                /* buffer               */
               1,                  /* one data            */
	       MPI_INT,         /* type                */
	       i,                  /* to which node       */
	       1,                  /* number of message   */
	       MPI_COMM_WORLD);    /* common communicator  */
	    }
	}

	else
	{    
    MPI_Recv  (&n,                 /* buffer               */
               1,                 /* one data            */
	       MPI_INT,           /* type                */
	       0,                 /* to which node       */
	       1,                 /* number of message   */
	       MPI_COMM_WORLD,   /* common communicator  */
	       &stats);
	}       
        if (n == 0)
            done = 1;
        else
        {
            h   = (xh-xl) / (double) n;
            sum = 0.0;
            for (i = myid + 1; i <= n; i += numprocs)
            {
                x = xl + h * ((double)i - 0.5);
                sum += f(x);
            }
            myfunk = h * sum;
    printf("Process %d SUMM %.16f\n", myid, myfunk);
    
    /* Sending the local sum to node 0 */
	    if (myid !=0) 
	    {
    MPI_Send (&myfunk,                /* buffer               */
               1,                  /* one data            */
	       MPI_DOUBLE,         /* type                */
	       0,                  /* to which node       */
	       1,                  /* number of message   */
	       MPI_COMM_WORLD);    /* common communicator  */
	    }
			
            if (myid == 0)
	    {

       funk = myfunk;
       for (i=1; i< numprocs; i++)
         { MPI_Recv (&myfunk,
	             1,
		     MPI_DOUBLE,
		     i,
		     1,
		     MPI_COMM_WORLD,
		     &stats);
	   funk += myfunk;  
	 };

                printf("Integral is approximately  %.16f, Error   %.16f\n",
		    funk, funk - fi(xh) + fi(xl));
		endwtime = MPI_Wtime();
		printf("Time of calculation = %f\n", endwtime-startwtime);	       
	    }
        }
    }
    MPI_Finalize();
}


static double f(double a)
{
      return cos(a);
}

static double fi(double a)
{
	return sin(a);
}

