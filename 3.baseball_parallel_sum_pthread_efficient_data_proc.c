/* 
 * Problem Statement
   -----------------
   This program simulates scoring for a baseball game, handling specific operations defined by strings. Each string operation can be:

   A numeric string (e.g., "5", "-2"): add this integer to the record.

   "+": add a score equal to the sum of the previous two scores.

   "D": add a score equal to double the previous score.

   "C": remove (invalidate) the previous score.

   After processing all operations, the goal is to calculate the sum of all valid scores efficiently, leveraging parallel computation with threads.

   Inputs Explained:
   ----------------

   Example input provided to main() function:

   char *ops[] = {"5", "2", "C", "D", "+", "10", "C", "-2", "4", "D", "9", "+", "+"};

   This input simulates a series of baseball operations described above.

   Expected Output:
   ---------------

   Operations: 5 2 C D + 10 C -2 4 D 9 + +

   Step-by-step Records:
   - 5        → [5]
   - 2        → [5, 2]
   - C        → [5]                 (removed 2)
   - D        → [5, 10]             (double 5)
   - +        → [5, 10, 15]         (5 + 10)
   - 10       → [5, 10, 15, 10]
   - C        → [5, 10, 15]         (remove last 10)
   - -2       → [5, 10, 15, -2]
   - 4        → [5, 10, 15, -2, 4]
   - D        → [5, 10, 15, -2, 4, 8]  (double last: 4×2=8)
   - 9        → [5, 10, 15, -2, 4, 8, 9]
   - +        → [5, 10, 15, -2, 4, 8, 9, 17]  (8+9=17)
   - +        → [5, 10, 15, -2, 4, 8, 9, 17, 26]  (9+17=26)

   Final Scores: [5, 10, 15, -2, 4, 8, 9, 17, 26]

   Total Sum: 5 + 10 + 15 - 2 + 4 + 8 + 9 + 17 + 26 = **92**

   Performance Note:
   ----------------
   The program utilizes pthreads for parallel summation, improving performance in processing large score arrays.

   The complexity remains O(n), with parallelism providing practical speedup for large datasets.

   The provided program clearly demonstrates efficient parallel data processing using threads in C 
   to quickly compute baseball game scores based on given operations.

*/   
   
#include <stdio.h>      // Standard Input/Output library
#include <stdlib.h>     // atoi() for converting strings to integers
#include <string.h>     // strcmp() for comparing strings
#include <pthread.h>    // For parallel multi-threading
#include <ctype.h>      // isdigit() to check numeric characters

#define MAX_OPERATIONS 1000  // Max number of operations
#define NUM_THREADS 2        // Number of threads to use for parallel computation

typedef struct {
    int *records;   // Pointer to scores array
    int start, end; // Start and end index for partial sum computation
    int result;     // Store the partial sum calculated by each thread
} ThreadData;

// Function for thread to compute partial sum
// Each thread calculates a partial sum of the scores from data->start to data->end.
void *parallelSum(void *arg) {
    ThreadData *data = (ThreadData *)arg;  // Cast void pointer to ThreadData pointer
    int sum = 0;
    for (int i = data->start; i < data->end; i++) {
        sum += data->records[i];           // Sum elements from start to end
    }
    data->result = sum;                    // Store partial sum result
    pthread_exit(NULL);                    // Thread exits
}

// Function to process operations and compute sum
// Initializes an array to store valid scores and an index tracker.
int calPoints(char *ops[], int size) {
    int records[MAX_OPERATIONS];    // Store valid scores
    int index = 0;                  // Current index in the scores array

    // Parsing Operations and Filling Records
    // Processes each operation according to rules, filling the records array.
    for (int i = 0; i < size; i++) {
        if (isdigit(ops[i][0]) || (ops[i][0] == '-' && isdigit(ops[i][1]))) {
            records[index++] = atoi(ops[i]);  // Numeric operation, convert and store
        } else if (strcmp(ops[i], "C") == 0 && index > 0) {
            index--;                          // "C": Remove last valid score
        } else if (strcmp(ops[i], "D") == 0 && index > 0) {
            records[index] = 2 * records[index - 1];  // "D": Double previous score
            index++;
        } else if (strcmp(ops[i], "+") == 0 && index > 1) {
            records[index] = records[index - 1] + records[index - 2];  // "+": Sum previous two
            index++;
        }
    }

    // Multi-threaded sum calculation
    // Parallel Sum Computation (Multi-threading) . 
    // Prepares thread structures for parallel computation.
    // Splits records array approximately equally for each thread.

    pthread_t threads[NUM_THREADS];    // Declare threads
    ThreadData data[NUM_THREADS];      // Declare data for threads
    int mid = index / NUM_THREADS;     // Calculate midpoint to divide array among threads

    // Creating Threads and Assigning Work
    // Threads are created to process separate parts of the array concurrently.
    // Last thread takes any remaining elements.
    for (int i = 0; i < NUM_THREADS; i++) {
        data[i].records = records;                         // Assign records pointer
        data[i].start = i * mid;                           // Compute start index
        data[i].end = (i == NUM_THREADS - 1) ? index : (i + 1) * mid; // Compute end index
        pthread_create(&threads[i], NULL, parallelSum, &data[i]); // Launch thread
    }

    // Collecting Results from Threads
    // Joins threads after completion and combines their partial sums.
    int totalSum = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);    // Wait for each thread to finish
        totalSum += data[i].result;        // Combine partial results
    }

    return totalSum;
}

int main() {

    // Provides a test case.	
    char *ops[] = {"5", "2", "C", "D", "+", "10", "C", "-2", "4", "D", "9", "+", "+"};

    int size = sizeof(ops) / sizeof(ops[0]);

    // Calls calPoints and prints the resulting total score.
    printf("Total Score: %d\n", calPoints(ops, size));

    // Defines multiple test cases as arrays of strings.	
    char *testCases[][MAX_OPERATIONS] = {
        {"5", "2", "C", "D", "+"},
        {"5", "-2", "4", "C", "D", "9", "+", "+"},
        {"1"},
        {"0"},
        {"10", "C"},
        {"-10", "D", "D", "C", "+"},
        {"5", "10", "+", "D", "+", "C"}
    };

    // Provides lengths for each corresponding t123
    // est case.
    int sizes[] = {5, 8, 1, 1, 2, 5, 6};  // Number of operations in each case

    // Iterates over test cases, calls calPoints, and prints the resulting sum for each case.
    for (size_t i = 0; i < sizeof(sizes) / sizeof(sizes[0]); i++) {
        printf("Test %lu: %d\n", i + 1, calPoints(testCases[i], sizes[i]));
    }

    return 0;
}

