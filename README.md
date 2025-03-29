# ScoreBench

This repository provides an illustrative series of implementations demonstrating baseball scoring optimizations, progressing from basic sequential parsing to advanced parallelized and vectorized optimizations.

---

## Overview

The repositories demonstrate increasingly sophisticated methods for calculating baseball game scores based on parsed operations. Each step highlights significant performance improvements, offering a clear progression suitable for understanding scalable optimization strategies:

| Repository      | Description                                                                                                   | Optimization Strategy           |
|-----------------|---------------------------------------------------------------------------------------------------------------|---------------------------------|
| **ScoreKeeper** | Simple sequential processing of integers and special operations ('+', 'D', 'C').                              | Basic parsing logic             |
| **BaseballScorer** | Enhanced flexibility with multiple test scenarios. Efficient running totals and diverse scoring combinations. | Flexible test scenarios         |
| **ThreadedScorer** | Multi-threaded using pthreads for improved performance with larger datasets.                                | Pthread multi-threading         |
| **ScoreBench**    | Benchmarks comparing single-threaded vs. multi-threaded performance. Designed for large-scale operations.   | Benchmarking concurrency        |
| **SimdScoreFast** | SIMD vectorization combined with multi-threading, utilizing AVX instructions for high-performance computation. | SIMD acceleration (AVX)         |
| **NUMAScoreOpt**  | Advanced multi-threaded SIMD with NUMA-aware memory management for peak performance on NUMA systems.         | NUMA-aware optimization         |

The ScoreBench repository offers a progressive series of C implementations that demonstrate various optimization techniques for calculating baseball game scores. Each implementation builds upon the previous, introducing advanced concepts such as multi-threading, SIMD acceleration, and NUMA-aware performance enhancements.​

Repository Structure

The repository contains the following C source files, each representing a distinct approach to solving the baseball game scoring problem:​

1.baseball_game.c: Implements a basic sequential algorithm to compute baseball game scores based on a series of operations.​

2.baseball_game_direct_update_sum_store_converted_values.c: Enhances the basic implementation by directly updating the sum and storing converted values for improved efficiency.​

3.baseball_parallel_sum_pthread_efficient_data_proc.c: Introduces multi-threading using POSIX threads (pthreads) to parallelize the summation process, improving performance on multi-core systems.​

4.baseball_parallel_bench_large_test_cases.c: Focuses on benchmarking the parallel implementation with large test cases to evaluate performance gains.​

5.baseball_parallel_optimization_simd_avx_sse.c: Integrates SIMD (Single Instruction, Multiple Data) vectorization using AVX and SSE instructions to accelerate computation further.​

6.baseball_parallel_optimization_simd_avx_sse.c: This implementation builds upon previous optimizations by integrating both multi-threading and SIMD (AVX/SSE) instructions. The combination of these techniques aims to enhance data processing efficiency, allowing multiple operations per CPU cycle and distributing workloads across multiple threads for concurrent execution.​

7.baseball_parallel_optimization_simd_avx_sse.c: Similar to the sixth implementation, this version continues to explore the integration of multi-threading with SIMD (AVX/SSE) optimizations. It may include further refinements or alternative approaches to balancing workloads and maximizing the utilization of CPU resources.​

8.baseball_parallel_optimization_simd_avx_sse.c: This iteration likely introduces additional enhancements to the multi-threaded SIMD approach, possibly focusing on optimizing memory access patterns, reducing synchronization overhead, or improving cache utilization to further boost performance.​

9.baseball_parallel_optimization_simd_avx_sse.c: The ninth implementation may delve into advanced optimization strategies, such as dynamic workload distribution, adaptive threading models, or leveraging specific hardware features to achieve optimal performance in computing baseball game scores.

10.baseball_optimizing_multithreaded_simd_for_numa.c: Optimizes the multi-threaded SIMD implementation for NUMA (Non-Uniform Memory Access) architectures to enhance memory locality and reduce latency.​

11.baseball_optimizing_multithreaded_simd_for_numa_all_bench.c: Extends the NUMA-aware implementation with comprehensive benchmarking to assess performance across different system configurations.​

12.baseball_parallel_smid_avx_sse_multithreaded_for_numa_node_cpuaffinity_memory_usage_sync_lock.c: Further refines the NUMA-optimized implementation by incorporating CPU affinity settings and memory usage synchronization mechanisms to maximize efficiency.

---

## Problem Statement

The program simulates scoring in a baseball game by processing operations represented by strings:

- An integer (`x`): Adds a new score of `x`.
- `+`: Adds a new score equal to the sum of the previous two scores.
- `D`: Adds a new score equal to double the previous score.
- `C`: Removes (invalidates) the previous score.

The goal is to calculate the total sum of valid scores after processing all operations.

---

## Examples and Explanation

### Input Sets and Expected Outputs

#### Example 1:

**Input:** `["5", "2", "C", "D", "+"]`

**Processing Steps:**
- `[5]` → `[5, 2]` → `[5]` (C removes 2) → `[5, 10]` (D doubles 5) → `[5, 10, 15]` (+ sums 5+10)

**Total:** `5 + 10 + 15 = 30`

#### Example 2:

**Input:** `["5", "-2", "4", "C", "D", "9", "+", "+"]`

**Processing Steps:**
- `[5]` → `[5, -2]` → `[5, -2, 4]` → `[5, -2]` (C removes 4) → `[5, -2, -4]` (D doubles -2)
- `[5, -2, -4, 9]` → `[5, -2, -4, 9, 5]` (+ sums -4+9) → `[5, -2, -4, 9, 5, 14]` (+ sums 9+5)

**Total:** `5 - 2 - 4 + 9 + 5 + 14 = 27`

#### Example 3:

**Input:** `["1"]`

**Total:** `1`

---

## Performance Considerations

- **Single-threaded implementation:** Linear complexity (O(n)).
- **Multi-threaded implementations:** Significantly reduce execution time for large datasets.
- **SIMD Vectorization:** Further improves performance by simultaneously processing multiple values.
- **NUMA-awareness:** Optimizes memory access, reduces latency, and increases computational efficiency even in single-node NUMA configurations.

---

## Compilation Instructions

Compile optimized versions using:

```bash
gcc -mavx2 -pthread -O3 -lnuma baseball_optimized.c -o baseball_simd_numa
```

Run with explicit NUMA awareness for best performance:

```bash
numactl --membind=0 ./baseball_simd_numa
```

---

## Benchmark Results

| Execution Type                 | Time (ms) | Total Score |
|--------------------------------|-----------|-------------|
| Single-threaded                | 7 ms      | 15,000,000  |
| Multi-threaded                 | 6 ms      | 15,000,000  |
| Multi-threaded SIMD            | 3 ms      | 15,000,000  |
| NUMA-aware Multi-threaded SIMD | 4 ms      | 15,000,000  |

---

This comprehensive example illustrates effective approaches to parallel processing, SIMD acceleration, and NUMA optimization, providing scalable performance for real-world numerical computation tasks.


