/*
 * 
 * Problem Statement:
 * ------------------
   This program simulates scoring rules in a baseball game based on specific operations represented as strings:

   Integer ("x"): Directly adds this value as a new score.

   "+": Adds the sum of the previous two scores.

   "D": Adds double the previous score.

   "C": Removes the last recorded score.

   The goal is to process a sequence of such operations and compute the final sum of valid scores.


   Inputs Explained:
   ----------------
   The program contains multiple test cases:

   {"5", "2", "C", "D", "+"}

   {"5", "-2", "4", "C", "D", "9", "+", "+"}

   {"1"}

   {"0"}

   {"10", "C"}

   {"-10", "D", "D", "C", "+"}

   {"5", "10", "+", "D", "+", "C"}

   Each array of strings represents one sequence of operations.

   Expected Outputs and Explanation:
   ---------------------------------

   Test 1: {"5", "2", "C", "D", "+"}
   Operations: [5], [5,2], [5] (C removes 2), [5,10] (D=5×2), [5,10,15] (+=5+10)
   Output: 5+10+15 = 30

   Test 2: {"5", "-2", "4", "C", "D", "9", "+", "+"}
   Operations: [5], [5,-2], [5,-2,4], [5,-2], [5,-2,-4], [5,-2,-4,9], [5,-2,-4,9,5], [5,-2,-4,9,5,14]
   Output: 5-2-4+9+5+14 = 27

   Test 3: {"1"}
   Output: 1

   Test 4: {"0"}
   Output: 0

   Test 5: {"10", "C"}
   Operations: [10], [] (C removes 10)
   Output: 0

   Test 6: {"-10", "D", "D", "C", "+"}
   Operations: [-10], [-10,-20], [-10,-20,-40], [-10,-20], [-10,-20,-30]
   Output: -10-20-30 = -60

   Test 7: {"5", "10", "+", "D", "+", "C"}
   Operations: [5], [5,10], [5,10,15], [5,10,15,30], [5,10,15,30,45], [5,10,15,30]
   Output: 5+10+15+30 = 60


   Final Notes on the Code:
   -----------------------

   This program efficiently maintains a running sum directly updated after each operation.
   The complexity is linear (O(n)) where n is the number of operations.
   
   It clearly demonstrates how string operations are parsed, validated, converted to numbers, 
   and tracked in C.
   
   Thus, the provided code is a concise and clear implementation to calculate scores for 
   a baseball game as per given rules.


*/

#include <stdio.h>   // Input-output library for printf()
#include <stdlib.h>  // Library to use atoi() function to convert strings to integers
#include <string.h>  // String library for strcmp()
#include <ctype.h>   // Character handling library for isdigit()

#define MAX_OPERATIONS 1000

// function that takes an array of string operations and their count (size) and returns an integer sum of scores.
//
int calPoints(char *ops[], int size) {

    // Declares an integer array to keep track of valid scores and initializes tracking variables.	
    int records[MAX_OPERATIONS];  // Store scores
    int index = 0, sum = 0;       // Store index and running sum

    // Iterates through each operation string in the provided array.
    //
    for (int i = 0; i < size; i++) {

	// Case 1: Numeric Input (Positive or Negative Integer):
	// Checks if the current operation is a number (including negative numbers).
	// Converts it to integer, stores in the array, updates index and running sum.
        if (isdigit(ops[i][0]) || (ops[i][0] == '-' && isdigit(ops[i][1]))) {
            int num = atoi(ops[i]);
            records[index++] = num;
            sum += num;

	    // Case 2: Operation "C" (Cancel last score):
	    // Checks for "C".
	    // If found, removes the last recorded score (records[index-1]) and updates the running sum.
	    // Decrements index to effectively erase the last record.
        } else if (strcmp(ops[i], "C") == 0 && index > 0) {
            sum -= records[--index];  // Remove last score and update sum
	  
	  // Case 3: Operation "D" (Double previous score):
	  // Handles "D", doubling the previous score, updating both the score array and running sum.
	} else if (strcmp(ops[i], "D") == 0 && index > 0) {
            int doublePrev = 2 * records[index - 1];
            records[index++] = doublePrev;
            sum += doublePrev;

	  // Case 4: Operation "+" (Sum of previous two scores):
	  // Processes "+" by calculating the sum of the two most recent scores and updating records 
	  // and total sum accordingly.
        } else if (strcmp(ops[i], "+") == 0 && index > 1) {
            int sumLastTwo = records[index - 1] + records[index - 2];
            records[index++] = sumLastTwo;
            sum += sumLastTwo;
        }
    }

    return sum;
}

int main() {

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

    // Provides lengths for each corresponding test case.
    int sizes[] = {5, 8, 1, 1, 2, 5, 6};  // Number of operations in each case

    // Iterates over test cases, calls calPoints, and prints the resulting sum for each case.
    for (size_t i = 0; i < sizeof(sizes) / sizeof(sizes[0]); i++) {
        printf("Test %lu: %d\n", i + 1, calPoints(testCases[i], sizes[i]));
    }

    return 0;
}

