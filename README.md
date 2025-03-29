# ScoreBench
  Repos illustrate baseball scoring optimizations: from 
  simple parsing (ScoreKeeper), 
  flexible test cases (BaseballScorer), 
  pthread multi-threading (ThreadedScorer, ScoreBench), 
  SIMD acceleration (SimdScoreFast), to 
  NUMA-aware performance (NUMAScoreOpt).

# Detailing all problem statement

 1. Problem Statement (Description of Baseball Game):
     -------------------------------------------------

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

2. Problem Statement:
   ------------------
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

3. Problem Statement
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

4. Problem Statement:
   The provided C program implements a simulation of baseball scoring rules, efficiently handling
   a large number of operations. Each operation is a string, interpreted as follows:

   Integer ("x"): Record a new score of x points.

   "+": Record a new score equal to the sum of the previous two scores.

   "D": Record a new score equal to double the previous score.

   "C": Remove the previously recorded score.

   Given a potentially large set of such operations (up to 1,000,000), the program computes
   the total score efficiently using parallel processing (multi-threading).

   Input Explanation:
   ------------------
   The program generates a large number of operations to benchmark performance:

   generate 1,000,000 operations ("10", "D", "10", "D", ..., repeating 500,000 times).

   Expected Outputs (Benchmark Results):
   -------------------------------------

   Operations: "10", "D" repeated 500,000 times.

   Final Scores: Alternating between 10 and double previous repeatedly:
   → 10, 20, 10, 20, ..., repeated 500,000 times.

   Total Score Calculation per Pair (10 + 20 = 30):
   → 500,000 pairs × 30 points each = 15,000,000 points.

   Single-threaded: [time_ms] ms, Result: 15000000
   Multi-threaded: [faster_time_ms] ms, Result: 15000000

   Single-threaded: 7 ms, Result: 15000000
   Multi-threaded: 6 ms, Result: 15000000

   The multi-threaded execution should be faster, leveraging CPU parallelism.

   Performance Advantage:
   ---------------------
   By dividing the summation into parallel tasks, the multi-threaded version significantly
   reduces computation time, especially with large data sets.

   This program effectively illustrates parallel computation using pthreads to efficiently
   handle large-scale data processing, benchmarking clearly the performance advantages
   of parallel over single-threaded computations in C.

5.  Problem Statement:
 * ------------------
   The provided C program efficiently computes the total score for a baseball game simulation.
   It interprets an array of string operations with the following rules:

   Numeric String ("10", "-5", etc.):** Adds the number as a score.

   "+": Adds the sum of the previous two scores.

   "D": Adds double the previous score.

   "C": Removes the previous score.

   Given potentially 1,000,000 operations, the program calculates the total efficiently using
   parallelism (multi-threading) and vectorized SIMD instructions (AVX).

   Input Explained:
   ----------------
   The program generates a large test case with 1,000,000 operations:

   Operations generated: "10", "D", "10", "D", ... repeated 500,000 times.

   Expected Output (Benchmark Result):
   -----------------------------------
   Step-by-step Calculation:

   Operations: "10", "D" repeated 500,000 times.

   Scores generated: 10, 20, 10, 20, ..., repeating.

   Sum per pair: (10+20) = 30.

   Total sum: 500,000 pairs × 30 points each = 15,000,000.

   Multi-threaded SIMD: [time_ms] ms, Result: 15000000

   Multi-threaded SIMD: 3 ms, Result: 15000000

   [time_ms] will reflect execution time (fast due to SIMD parallelism).


   Performance Considerations:
   ---------------------------
   Multi-threading reduces overall execution time by splitting work across CPU cores.

   AVX SIMD provides vectorized computation, summing multiple integers simultaneously in
   fewer CPU cycles.

   Combining both optimizations yields superior performance for large-scale datasets.

   This program exemplifies how to effectively combine SIMD vectorization with multi-threading in C,
   dramatically accelerating large-scale numeric computations for real-world scenarios like this
   baseball scoring system.

   My CPU supports:

   gcc -mavx2 -pthread -O3 5.baseball_parallel_optimization_simd_avx_sse.c -o baseball_simd

6. Optimizing Multi-threaded SIMD for NUMA (Non-Uniform Memory Access)

   Why Optimize for NUMA?

   Most modern multi-core CPUs are NUMA-based (Non-Uniform Memory Access). In a NUMA system:

   Each CPU core has its own memory node (NUMA node).

   Accessing local memory is faster than accessing remote memory.

   Improper NUMA optimization leads to high memory latency and performance bottlenecks.

   NUMA Bottlenecks in Multi-threading

   Threads may access remote NUMA nodes → Increased memory latency.

   Cache coherence traffic increases → Reduced CPU efficiency.

   Memory interleaving inefficiencies → Poor memory bandwidth utilization.

   NUMA Optimization Goals
   -----------------------
   - Ensure each thread accesses memory from its local NUMA node.
   - Reduce unnecessary memory transfers across NUMA nodes.
   - Utilize NUMA-aware memory allocation strategies.

   #sudo apt-get install numactl libnuma-dev
   #gcc -mavx2 -pthread -O3 10.baseball_optimizing_multithreaded_simd_for_numa.c -lnuma -o baseball_simd_numa

   Problem Statement
   -----------------

   This C program computes the total score of a baseball game, efficiently handling a large number of scoring operations with advanced optimizations:

   Numeric strings ("10", "-2") represent adding the given score.

   "D" adds double the previous score.

   "+" adds the sum of the two previous scores.

   "C" removes the previous score.

   The program is optimized for handling 1,000,000 operations using:
   - Parallel processing (multi-threading)
   - SIMD vectorization (AVX)
   - NUMA-awareness (memory and CPU affinity optimization)

   Input Explained
   ----------------
   Generate large test input (1,000,000 operations):
   - Input pattern: "10", "D", "10", "D", ... repeated 500,000 times.

   1. NUMA Hardware Check: #
   -----------------------
   available: 1 nodes (0)
   node 0 cpus: 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
   node 0 size: 23889 MB
   node 0 free: 22584 MB
   node distances:
   node   0
   0:  10

   Observations:

   Your system has only 1 NUMA node (node 0).
   Node 0 has 32 CPUs (logical cores).
   Memory available: ~23.9 GB total, ~22.6 GB free.
   Node distance to itself is minimal (10), indicating no NUMA latency overhead.

   Conclusion: This confirms your system is a single-node NUMA system, effectively a non-NUMA configuration.

   2.Benchmarking Results with Explicit Binding (numactl --membind=0):
   ------------------------------------------------------------------
   Running benchmark without NUMA-awareness:
   Non-NUMA Execution: 7 ms, Result: 5000000

   Running benchmark with explicit NUMA-awareness:
   NUMA-Aware Execution (Node 0 preferred): 4 ms, Result: 5000000
   Memory allowed on nodes: 0

   Observations:
   ------------
   - Execution without NUMA-awareness took 7 ms.
   - Explicit NUMA-aware execution was faster at 4 ms.
   - Results (5000000) match exactly, validating correctness.
   - Memory bound explicitly to node 0 (no other nodes present).

   Reason for minor performance improvement:
   - Using explicit NUMA API (numa_set_preferred(0)), even on single-node NUMA, can sometimes
     improve cache-locality and memory access patterns slightly, resulting in minor
     performance boosts (here, from 7 ms → 4 ms).

   srrmk@mars:~/MyWorkSpace/turing-code$ numactl --hardware
   available: 1 nodes (0)
   node 0 cpus: 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
   node 0 size: 23889 MB
   node 0 free: 22584 MB
   node distances:
   node   0
   0:  10

   srrmk@mars:~/MyWorkSpace/turing-code$ numactl --membind=0 ./baseball_simd_numa
   Running benchmark without NUMA-awareness:
   Non-NUMA Execution: 7 ms, Result: 5000000

   Running benchmark with explicit NUMA-awareness:
   NUMA-Aware Execution (Node 0 preferred): 4 ms, Result: 5000000
   Memory allowed on nodes: 0
   srrmk@mars:~/MyWorkSpace/turing-code$

   EXAMPLES
       numactl --physcpubind=+0-4,8-12 myapplic arguments Run myapplic on cpus 0-4 and 8-12 of the current cpuset.

       numactl --interleave=all bigdatabase arguments Run big database with its memory interleaved on all CPUs.

       numactl --cpunodebind=0 --membind=0,1 process Run process on node 0 with memory allocated on node 0 and 1.

       numactl --cpunodebind=0 --membind=0,1 -- process -l Run process as above, but with an option (-l) that would be confused with a numactl option.

       numactl --cpunodebind=0 --balancing --membind=0,1 process Run process on node 0 with memory allocated on node 0 and 1.  Optimize the page placement with Linux kernel NUMA  balancing  mechanism  if
       possible.

       numactl --cpunodebind=netdev:eth0 --membind=netdev:eth0 network-server Run network-server on the node of network device eth0 with its memory also in the same node.

       numactl --preferred=1 numactl --show Set preferred node 1 and show the resulting state.

       numactl --preferred-many=0x3 numactl --show Set preferred nodes 1 and 2, and show the resulting state.

       numactl --length 1g --shm /tmp/shmkey --interleave=all Interleave all of the sysv shared memory region of size 1g specified by /tmp/shmkey over all nodes.

       Place a tmpfs file on 2 nodes:
         numactl --membind=2 dd if=/dev/zero of=/dev/shm/A bs=1M count=1024
         numactl --membind=3 dd if=/dev/zero of=/dev/shm/A seek=1024 bs=1M count=1024

       numactl --localalloc /dev/shm/file Reset the policy for the shared memory file file to the default localalloc policy.

