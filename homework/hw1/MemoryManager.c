#include <stdio.h>
#include <stdlib.h>

/**
 * Compares two numbers and returns a positive integer, 0 or 
 * a negative integer if the first number is greater than, equal to
 * or less than the second number.
 * Assumes the numbers are of the type int.
 * Used when invoking the quicksort function.  
 */
int compare (const void * a, const void * b)
{
    return (*(int*)a - *(int*)b);
}


/**
 * Takes in an array of integers and the number of elements in the array 
 * as arguments.
 * Then it will sort and return the median of the sorted array. 
 * To keep things simple, it always returns
 * the element at index arr_size/2 even though the 
 * array size is even.
 */
int return_median(int *arr, int size)
{
    qsort(arr, size, sizeof(int), compare);
    return arr[size/2];
}


/**
 * Takes in a random number and maps it to the
 * range [100000 - 120000)
 */
int get_iteration_count(int rand)
{
    return (rand % 20000) + 100000;     
}

/**
 * Takes in a random number and maps it to the 
 * range [1000 - 1500)
 */
int get_arr_size(int rand)
{
    return (rand % 500) + 1000;
}

int get_running_count()
{
    // 1. generate number of iterations
    int num_iterations = get_iteration_count(rand());
    printf("[MemoryManager] Number of iterations: %d\n", num_iterations);
    int thirteen_divides_how_many_averages = 0;
    // 2. for number of iterations...
    for (int i = 0; i < num_iterations; i++) {
        // 2a. generate an array size
        int array_size = get_arr_size(rand());
        // 2b. allocate memory for array in the heap
        int *array = (int*)  malloc(array_size * sizeof(int));
        // 2c. populate array with random numbers
        for (int i = 0; i < array_size; i++) {
            int r = rand();
            array[i] = r;
        }
        // 2d. get median of array
        int median = return_median(array, array_size);
        // 2e. check if median divisible by 13
        if (median % 13 == 0) {
            thirteen_divides_how_many_averages++;
        }
        free(array);
    }
    // 2f. return number of medians divisble by 13
    return thirteen_divides_how_many_averages;
}
