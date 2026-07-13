# C++ Performance, Optimization and Concurrency

## Topics Covered

- Cache Friendliness
- Cache Lines
- `std::vector` vs `std::list`
- Array of Structs (AoS) vs Struct of Arrays (SoA)
- Performance Beyond Big-O
- Constant Factors
- Branch Prediction
- False Sharing
- Compiler Optimizations (`-O2`, `-O3`)
- Function Inlining
- Profiling Basics
- `perf` (Linux)
- Visual Studio Profiler
- Flame Graphs
- Multithreading
- `std::thread`
- `std::mutex`
- `std::lock_guard`
- `std::atomic`
- Lock-Free Programming (Conceptual)
- Floating Point Pitfalls
- Fixed-Point and Decimal Types
- Associativity Issues
- Time Complexity
- Performance Considerations
- Common Mistakes
- Best Practices
- Interview Revision Points

---

# 1. Cache Friendliness

## Why Cache Matters

CPU accesses cache much faster than RAM.

Memory hierarchy

```
CPU Registers
      ↓
L1 Cache
      ↓
L2 Cache
      ↓
L3 Cache
      ↓
RAM
```

Keeping frequently accessed data close together improves performance.

---

## Cache Line

CPU loads memory in blocks called **cache lines**.

Typical size

```
64 Bytes
```

Accessing one element usually loads nearby elements into cache.

Sequential memory access is significantly faster than random access.

---

## std::vector vs std::list

### std::vector

Memory is contiguous.

```
100 101 102 103 104
```

Benefits

- Excellent cache locality
- Faster iteration
- Better compiler optimization

---

### std::list

Memory is scattered.

```
100

400

900

1200

50
```

Drawbacks

- Poor cache locality
- Frequent cache misses
- Slower iteration

---

## Complexity

| Container | Iteration | Cache Friendly |
|-----------|-----------|----------------|
| `std::vector` | O(n) | High |
| `std::list` | O(n) | Low |

Although both have O(n) iteration, `std::vector` is usually much faster.

---

# 2. Array of Structs vs Struct of Arrays

## Array of Structs (AoS)

```cpp
struct Trade
{
    double price;
    double quantity;
    double pnl;
};

std::vector<Trade> trades;
```

Memory

```
Price Quantity PnL

Price Quantity PnL

Price Quantity PnL
```

Suitable for object-oriented processing.

---

## Struct of Arrays (SoA)

```cpp
struct Trades
{
    std::vector<double> prices;
    std::vector<double> quantities;
    std::vector<double> pnl;
};
```

Memory

```
Prices

100
101
102
103
```

Suitable for numerical computations.

Improves cache utilization when processing one field at a time.

---

## Comparison

| Pattern | Best For |
|----------|----------|
| Array of Structs | Individual objects |
| Struct of Arrays | Numeric processing |

---

# 3. Big-O is Not Enough

Two algorithms with identical Big-O complexity can have very different performance.

---

## Constant Factors

Algorithm A

```
1 operation per element
```

Algorithm B

```
20 operations per element
```

Both

```
O(n)
```

Algorithm A is significantly faster.

---

## Branch Prediction

Modern CPUs predict branch outcomes.

Predictable branches

```cpp
if(price > 100)
```

perform better than unpredictable branches.

Frequent mispredictions cause CPU pipeline stalls.

---

## False Sharing

Occurs when multiple threads modify different variables located on the same cache line.

Example

```cpp
struct Counter
{
    int counterA;
    int counterB;
};
```

Both variables may occupy the same cache line.

This causes unnecessary cache invalidation between threads.

---

### Solution

Padding

```cpp
struct Counter
{
    int counterA;
    char padding[60];
    int counterB;
};
```

Preferred solution

```cpp
struct alignas(64) Counter
{
    int value;
};
```

Each frequently modified variable occupies a separate cache line.

---

# 4. Compiler Optimizations

Compiler optimization converts source code into more efficient machine code.

---

## -O2

Typical optimizations

- Dead code elimination
- Constant folding
- Function inlining
- Loop optimization
- Register optimization

Example

```cpp
int square(int x)
{
    return x * x;
}

int calculate()
{
    return square(10);
}
```

Compiler may generate

```cpp
return 100;
```

---

## -O3

Includes all `-O2` optimizations plus

- Loop unrolling
- SIMD vectorization
- Aggressive inlining
- Additional loop optimizations

Example

```cpp
for(int i=0;i<8;i++)
{
    sum += values[i];
}
```

Compiler may conceptually generate

```cpp
sum += values[0];
sum += values[1];
...
sum += values[7];
```

---

## Function Inlining

Without inlining

```
Function Call

↓

Execute

↓

Return
```

With inlining

Compiler replaces the function call with its implementation.

Removes function call overhead.

---

## Premature Micro-Optimization

Avoid optimizing before measuring.

Correct workflow

```
Measure

↓

Identify Bottleneck

↓

Optimize

↓

Measure Again
```

---

# 5. Profiling Basics

Profiling identifies where the program spends execution time.

---

## Linux

```
perf
```

Example

```bash
perf record ./application

perf report
```

---

## Windows

Visual Studio Profiler

Provides

- CPU Usage
- Memory Usage
- Call Tree
- Thread Analysis

---

## Flame Graph

Represents CPU execution.

Height

```
Call Stack Depth
```

Width

```
CPU Time
```

The widest block usually represents the function consuming the most execution time.

---

# 6. Multithreading

Allows multiple tasks to execute simultaneously.

---

## std::thread

Creates a thread.

```cpp
std::thread t(processTrade);

t.join();
```

---

## join()

Waits until the thread completes.

---

## detach()

Allows thread execution independently.

Use carefully.

---

## Race Condition

Two threads modify shared data simultaneously.

Example

```cpp
counter++;
```

Result becomes unpredictable.

---

## std::mutex

Protects shared resources.

```cpp
std::mutex mtx;

std::lock_guard<std::mutex> lock(mtx);

counter++;
```

Only one thread accesses the protected section at a time.

---

## std::lock_guard

RAII wrapper around mutex.

Automatically unlocks when leaving scope.

Preferred over manual

```cpp
mtx.lock();

...

mtx.unlock();
```

---

## std::atomic

Provides lock-free atomic operations.

```cpp
std::atomic<int> counter = 0;

counter++;
```

Best suited for

- Counters
- Flags
- Simple shared variables

---

## Lock-Free Programming

Uses atomic CPU instructions instead of mutexes.

Advantages

- Lower latency
- Better scalability
- No lock contention

Disadvantages

- Difficult to implement correctly
- Debugging is challenging

---

## Choosing Synchronization

| Scenario | Recommended |
|----------|-------------|
| Simple counter | `std::atomic` |
| Complex shared object | `std::mutex` |

---

# 7. Floating Point Pitfalls

Floating-point numbers cannot exactly represent many decimal values.

Example

```cpp
double a = 0.1;
double b = 0.2;

std::cout << a + b;
```

Output

```
0.30000000000000004
```

---

## Why It Happens

Binary floating-point cannot exactly represent many decimal fractions.

Small approximation errors accumulate.

---

## Why Money Should Not Use double

Avoid

```cpp
double balance = 100.10;
```

Preferred approaches

### Fixed Point

Store smallest currency unit.

```cpp
long amount = 1050;
```

Represents

```
10.50
```

---

### Decimal Types

Examples

- Decimal
- Money
- BigDecimal

Provide exact decimal arithmetic.

---

## Associativity Problem

Floating-point addition is not associative.

Example

```cpp
double a = 1e16;
double b = -1e16;
double c = 1;
```

```cpp
(a + b) + c
```

Result

```
1
```

```cpp
a + (b + c)
```

Result

```
0
```

Different order produces different results.

---

## Better Summation

Instead of

```cpp
double total = 0;

for(...)
{
    total += value;
}
```

Production numerical libraries often use algorithms such as **Kahan Summation** to reduce accumulated floating-point error.

---

# Time Complexity

| Operation | Complexity |
|-----------|------------|
| Vector Iteration | O(n) |
| List Iteration | O(n) |
| Thread Creation | Expensive compared to function calls |
| Mutex Lock | O(1) |
| Atomic Operation | O(1) |
| Floating Point Addition | O(1) |
| Kahan Summation | O(n) |

---

# Performance Notes

- Cache locality often matters more than Big-O.
- Prefer contiguous memory layouts.
- Minimize cache misses.
- Profile before optimizing.
- Prefer `std::vector` for sequential processing.
- Consider Struct of Arrays for numerical workloads.
- Use `-O2` for most production builds.
- Benchmark before enabling `-O3`.
- Prefer `std::atomic` for simple shared variables.
- Keep lock scope as small as possible.
- Avoid storing money using `double`.

---

# Common Mistakes

- Choosing containers using only Big-O complexity.
- Using `std::list` when `std::vector` is more appropriate.
- Optimizing without profiling.
- Ignoring cache locality.
- Holding mutexes longer than necessary.
- Forgetting to join threads.
- Using mutexes for simple counters instead of atomics.
- Comparing floating-point values using `==`.
- Using `double` for financial calculations.
- Assuming floating-point addition is associative.

---

# Best Practices

- Prefer contiguous memory.
- Design cache-friendly data structures.
- Measure performance before optimizing.
- Profile the application regularly.
- Use RAII wrappers such as `std::lock_guard`.
- Prefer atomics for simple synchronization.
- Minimize lock contention.
- Separate frequently modified variables to avoid false sharing.
- Use decimal or fixed-point representations for currency.
- Focus on algorithmic improvements before micro-optimizations.

---

# Interview Revision

- CPU cache is much faster than RAM.
- CPU loads memory in cache lines (typically 64 bytes).
- `std::vector` usually outperforms `std::list` because of cache locality.
- Struct of Arrays improves cache efficiency for numerical processing.
- Big-O alone does not determine performance.
- Branch prediction affects CPU performance.
- False sharing occurs when multiple threads update variables on the same cache line.
- `-O2` performs safe production optimizations.
- `-O3` performs more aggressive optimizations such as loop unrolling and vectorization.
- Profilers identify execution bottlenecks.
- Flame graph width represents CPU time.
- `std::thread` creates threads.
- `std::mutex` protects shared resources.
- `std::lock_guard` automatically releases mutexes.
- `std::atomic` provides lock-free atomic operations for simple variables.
- Floating-point arithmetic introduces precision errors.
- Financial applications typically use fixed-point or decimal types instead of `double`.
- Floating-point addition is not associative.
- Kahan Summation reduces accumulated floating-point error.
