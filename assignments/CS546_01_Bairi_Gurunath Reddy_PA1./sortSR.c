
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define No_of_Elements 50

// compare method imported from source code shared in class
static int compare_int(const void *a, const void *b) {
    return (*(int *) a - *(int *) b);
}

/* Merge sorted arrays a[] and b[] into a[].
 * Length of a[] must be sum of lengths of a[] and b[] */
static void merge(int *a, int numel_a, int *b, int numel_b) {
    int *sorted = (int *) malloc((numel_a + numel_b) * sizeof *a);
    int i, a_i = 0, b_i = 0;
    /* merge a[] and b[] into sorted[] */
    for (i = 0; i < (numel_a + numel_b); i++) {
        if (a_i < numel_a && b_i < numel_b) {
            if (a[a_i] < b[b_i]) {
                sorted[i] = a[a_i];
                a_i++;
            } else {
                sorted[i] = b[b_i];
                b_i++;
            }
        } else {
            if (a_i < numel_a) {
                sorted[i] = a[a_i];
                a_i++;
            } else {
                sorted[i] = b[b_i];
                b_i++;
            }
        }
    }
    /* copy sorted[] into a[] */
    memcpy(a, sorted, (numel_a + numel_b) * sizeof *sorted);
    free(sorted);
}

int main(int argc, char** argv) {
  
  int arr_rank, arr_size, input_data[No_of_Elements];

  // Initialize the MPI environment
  MPI_Init(NULL, NULL);
  
  // Find out rank, size
  MPI_Comm_rank(MPI_COMM_WORLD, &arr_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &arr_size);

  //== Array to hold at how many location of data this array will handle
  int arr_partition[arr_size];
  int merge_array[No_of_Elements];
  int rem = No_of_Elements % arr_size;
  
  for (int i = 0; i < arr_size; i++){
    arr_partition[i] = No_of_Elements / arr_size;
    if (rem > 0 ){
        arr_partition[i] = arr_partition[i] + 1;
        rem = rem - 1;
    }
  }
  srand(0);

  // === Master process 
  if(arr_rank == 0) {
    
    printf("Unsorted Array: \t");
    for (int i = 0; i < No_of_Elements; i++) {
      input_data[i] = rand() % No_of_Elements;
      printf("%d ", input_data[i]);
    }
    printf("\n");

    // Use MPI_SEND to send data to processors for sorting
    int starting_loc = arr_partition[arr_rank];
    for(int i = 1; i < arr_size; i++) {
        MPI_Send( &input_data[starting_loc], arr_partition[i], MPI_INT, i, i, MPI_COMM_WORLD);
        starting_loc = starting_loc + arr_partition[i];
    }
    int temp_array[arr_partition[arr_rank]];
    
    // Before sorting
    for(int i=0;i<arr_partition[arr_rank];i++)
      temp_array[i] = input_data[i];

    // Sorting the array in the master process
    qsort(temp_array, arr_partition[arr_rank], sizeof(int), compare_int);
    
    // Receive data 
    for(int i=0; i<arr_partition[arr_rank]; i++)
      merge_array[i] = temp_array[i];

    starting_loc = arr_partition[arr_rank];
    for(int i = 1; i < arr_size; i++) {
      //MPI_Recv( void* data, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm communicator, MPI_Status* status);
      MPI_Recv(&merge_array[starting_loc], arr_partition[i], MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        starting_loc = starting_loc + arr_partition[i];
    }

    starting_loc = arr_partition[0];
    for(int i=0; i< arr_size-1; i++){
      //printf("Mergind %d and %d processor data\n", i, i+1);
      //printf("Merging arrays with location \nArray1: start-%d end-%d \nArray2 : start-%d end-%d\n",0, starting_loc, starting_loc, data_partition[i+1] );
      merge(&merge_array[0], starting_loc, &merge_array[starting_loc], arr_partition[i+1]);
      starting_loc = starting_loc + arr_partition[i];
    }

    printf("Sorted Array:\t");
    for(int i=0;i<No_of_Elements;i++)
      printf("%d ",merge_array[i]);
    printf("\n");

  } else{
    // === Worker process
    int subset_array_size = arr_partition[arr_rank];
    int subset_array[subset_array_size];

    // Receive the data from master process
    MPI_Recv(subset_array, subset_array_size, MPI_INT, 0, arr_rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    // Quick sort to sort the array
    qsort(subset_array, subset_array_size, sizeof(int), compare_int);

    // Send data back to master node
    MPI_Send(subset_array, subset_array_size, MPI_INT, 0, 0, MPI_COMM_WORLD);
  }

  MPI_Finalize();
  return 0;
}