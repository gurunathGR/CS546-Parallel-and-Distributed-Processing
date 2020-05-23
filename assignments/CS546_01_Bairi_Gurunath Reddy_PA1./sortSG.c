// #include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "math.h"
#include "mpi.h"
#include <time.h>


#define N 20
//#define INT_MAX 20


int num_Of_Proc, id_proc;


int compare(const void * a, const void * b) {

   const int *ia = (const int *)a;
    const int *ib = (const int *)b;
    return *ia  - *ib; 
}



int main(int argc,char ** argv) {

   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &num_Of_Proc );
   MPI_Comm_rank(MPI_COMM_WORLD, &id_proc);

   int * buf, * send_buf, * receive_buf, * sorted_buf, *displ; //*sendcnts, *recvcnts;
   int count = N/num_Of_Proc;
   int size, i;
   int temp, index;


   displ=(int*)malloc(num_Of_Proc*sizeof(int));
   
  // sendcnts=(int*)malloc(numOfProc*sizeof(int));

  // recvcnts=(int*)malloc(numOfProc*sizeof(int));

   buf=(int*)malloc(count*sizeof(int));

   for(i=0; i<num_Of_Proc; i++){
       displ[i]=i*count;
       //sendcnts[i]=count;
      // recvcnts[i]=count;
   }

    if(N % num_Of_Proc !=0)
    {
    printf("Data size is %d, data cannot be divide equally among them. Change input \n", N);
    MPI_Finalize();
    
    }

	
   if(id_proc == 0) {
      size=N;
      send_buf=(int*)malloc(size*sizeof(int));
      receive_buf=(int*)malloc(size*sizeof(int));
	
	  printf("\nInput\t");
      for (i=0;i<size;i++) {
         send_buf[i] = rand()% N;
         printf("%d ",send_buf[i]);
      }
      printf("\n\n");
      fflush(stdout);
   }

//   MPI_Scatterv(send_buf, sendcnts, displ, MPI_INT, buf, count, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatter(send_buf,
				count,
				MPI_INT,
				buf,
				count,
				MPI_INT,
				0,
				MPI_COMM_WORLD);
				
   printf("proces %d: ", id_proc);
   qsort(buf, count, sizeof(int), compare);
   for (int i = 0; i < count; i++) printf("%d ", buf[i]);
   printf("\n\n");
   fflush(stdout);

  // MPI_Gatherv(buf, count, MPI_INT, receive_buf, recvcnts, displ, MPI_INT, 0, MPI_COMM_WORLD);
  	MPI_Gather(buf,
  				count,
  				MPI_INT,
  				receive_buf,
  				count,
  				MPI_INT,
  				0,
  				MPI_COMM_WORLD);

   if (id_proc == 0) {
       //merge
       temp=N;
       for(i=0; i<size; i++){
           for(int j=0; j<num_Of_Proc; j++){

               if(temp>receive_buf[displ[j]]){
                   temp=receive_buf[displ[j]];
                   receive_buf[displ[j]]=receive_buf[i];
                   receive_buf[i]=temp;
               }

           }

           printf("%d ", receive_buf[i]);
       }


      printf("\n");
      fflush(stdout);
   }
  // wait(100);
   MPI_Finalize();
}