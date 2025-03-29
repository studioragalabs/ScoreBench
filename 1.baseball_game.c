/* 
 * Problem Statement (Description of Baseball Game):
 * -------------------------------------------------
   
   You're keeping track of scores during a baseball game. Operations consist of integers or special characters:

   An integer x: Adds a new score of x.

   "+": Adds a new score equal to the sum of the previous two scores.
   "D": Adds a new score equal to double the previous score.
   "C": Removes (invalidates) the previous score.

   After processing all operations, compute and return the total sum of the valid scores.

   Explanation of the Inputs:
   --------------------------
   
   In this program, the inputs are arrays of strings representing operations:
   
   Input Set 1 (ops1): {"5", "2", "C", "D", "+"}
   Input Set 2 (ops2): {"5", "-2", "4", "C", "D", "9", "+", "+"}
   Input Set 3 (ops3): {"1"}

   Expected Outputs Explanation:
   -----------------------------
   
   Input Set 1: {"5", "2", "C", "D", "+"}
   
   Operation sequence:
   "5" → records [5]
   "2" → records [5, 2]
   "C" → remove 2, records [5]
   "D" → double last (5), records [5, 10]
   "+" → sum last two (5+10=15), records [5, 10, 15]

   Total sum: 5 + 10 + 15 = 30

   Input Set 2: {"5", "-2", "4", "C", "D", "9", "+", "+"}

   Operation sequence:
   "5" → [5]
   "-2" → [5, -2]
   "4" → [5, -2, 4]
   "C" → remove 4, [5, -2]
   "D" → double -2, [5, -2, -4]
   "9" → [5, -2, -4, 9]
   "+" → sum last two (-4+9=5), [5, -2, -4, 9, 5]
   "+" → sum last two (9+5=14), [5, -2, -4, 9, 5, 14]

   Total sum: 5 + (-2) + (-4) + 9 + 5 + 14 = 27

   Input Set 3: {"1"}
   
   Operation sequence:
   "1" → [1]

   Total sum: 1

   Final Program Behavior:
   ----------------------

   This program accurately calculates the sum of baseball game points based on provided rules and inputs, 
   demonstrating clear processing logic for each operation and expected results.
*/

#include <stdio.h>     // Standard Input/Output library
#include <stdlib.h>    // Library for atoi (string to integer conversion)
#include <string.h>    // Library for strcmp (string comparison)
#include <ctype.h>     // Library for isdigit (check if character is digit)

#define MAX_OPERATIONS 1000  // Defines maximum number of operations that can be handled


// Function to compute the sum of records: 
// function calPoints which takes an array of strings (ops) and its size (size) as arguments 
// and returns an integer (sum of valid points)

int calPoints(char *ops[], int size) {
    int records[MAX_OPERATIONS];  // Array to store valid scores
    int index = 0;                // Tracks the index of records array. Points to the next free spot in `records` array

    // Checks if the operation begins with a digit (positive number) or 
    // a minus sign followed by a digit (negative number).
    // Converts the numeric string to integer using atoi() and stores it in records.
    // Moves the index forward (index++).

    for (int i = 0; i < size; i++) {
        if (isdigit(ops[i][0]) || (ops[i][0] == '-' && isdigit(ops[i][1]))) {
            // Convert string to integer and store
            records[index++] = atoi(ops[i]);
        } else if (strcmp(ops[i], "C") == 0) { // "C" removes the last recorded valid score.
            // Remove last valid score
            if (index > 0) {
                index--;                      // Decreases index by 1, effectively removing the last entry.
            }
        } else if (strcmp(ops[i], "D") == 0) { // "D" doubles the last recorded valid score and stores it.
            // Double last valid score
            if (index > 0) {
                records[index] = 2 * records[index - 1]; // Adds the doubled value at the next index and increments index.
                index++;
            }
        } else if (strcmp(ops[i], "+") == 0) {
            // Add last two valid scores
            if (index > 1) {
                records[index] = records[index - 1] + records[index - 2]; // "+" calculates the sum of the last two valid scores.
                index++;    // Adds the sum at the next index and increments index.
            }
        }
    }

    // Compute the total sum of valid records
    // Iterates over all valid recorded scores stored in records array up to index, summing them to calculate the final result.
    int result = 0;
    for (int i = 0; i < index; i++) {
        result += records[i];
    }
    return result;
}

// Main function
int main() {

    // three test cases (ops1, ops2, ops3) representing different scenarios.	
    char *ops1[] = {"5", "2", "C", "D", "+"};
    char *ops2[] = {"5", "-2", "4", "C", "D", "9", "+", "+"};
    char *ops3[] = {"1"};

    // Calculates each array size using sizeof() for processing.
    int size1 = sizeof(ops1) / sizeof(ops1[0]);
    int size2 = sizeof(ops2) / sizeof(ops2[0]);
    int size3 = sizeof(ops3) / sizeof(ops3[0]);

    // Executes calPoints function on each test case and prints results.
    printf("Output 1: %d\n", calPoints(ops1, size1)); // Expected: 30
    printf("Output 2: %d\n", calPoints(ops2, size2)); // Expected: 27
    printf("Output 3: %d\n", calPoints(ops3, size3)); // Expected: 1

    return 0;
}

