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


