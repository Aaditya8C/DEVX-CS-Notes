# Lambdas, std::function, STL Algorithms and Numeric Utilities

## Topics Covered

- Lambda Expressions
- Capture Lists
- `std::function`
- STL Algorithms
- `<algorithm>` Header
- `<numeric>` Header
- Performance Considerations
- Time Complexity
- Common Mistakes
- Best Practices
- Interview Revision Points

---

# 1. Lambda Expressions

## What is a Lambda?

A lambda is an anonymous function that can be defined directly where it is needed.

Instead of creating a separate function, you write the logic inline.

General syntax

```cpp
[capture](parameters)
{
    // body
};
```

Example

```cpp
auto add = [](int a, int b)
{
    return a + b;
};
```

---

## Capture List

Capture by Value

```cpp
int tax = 5;

auto calculate = [tax](int price)
{
    return price + tax;
};
```

The lambda receives a copy of `tax`.

---

Capture by Reference

```cpp
int total = 0;

auto update = [&total](int value)
{
    total += value;
};
```

The original variable is modified.

---

Capture Everything

```cpp
[=]     // Capture all variables by value

[&]     // Capture all variables by reference
```

Prefer explicit captures instead of capturing everything.

Example

```cpp
[&portfolio]
```

---

## Common Use Cases

Sorting

```cpp
std::sort(trades.begin(), trades.end(),
    [](const Trade& a, const Trade& b)
    {
        return a.price < b.price;
    });
```

Filtering

```cpp
std::find_if(...)
```

Transforming

```cpp
std::transform(...)
```

Accumulating

```cpp
std::accumulate(...)
```

---

# 2. std::function

## What is std::function?

A wrapper that can store any callable object.

Supported callables

- Normal functions
- Lambda expressions
- Functors
- Member functions

---

Example

```cpp
std::function<int(int, int)> operation;

operation = [](int a, int b)
{
    return a + b;
};

std::cout << operation(10, 20);
```

---

Store a Normal Function

```cpp
int multiply(int a, int b)
{
    return a * b;
}

std::function<int(int, int)> operation = multiply;
```

---

When to Use

- Callbacks
- Event handlers
- Strategy pattern
- Runtime configurable logic

Avoid `std::function` inside performance-critical loops because it introduces runtime overhead through type erasure.

---

# 3. STL Algorithms

Prefer STL algorithms over handwritten loops whenever possible.

Benefits

- Readable
- Reusable
- Well-tested
- Optimized
- Easier to maintain

---

# sort()

Sorts elements.

```cpp
std::sort(prices.begin(), prices.end());
```

Descending order

```cpp
std::sort(prices.begin(), prices.end(),
    [](int a, int b)
    {
        return a > b;
    });
```

Time Complexity

```
O(n log n)
```

---

# transform()

Applies an operation to every element.

```cpp
std::transform(prices.begin(),
               prices.end(),
               taxes.begin(),
               [](double price)
               {
                   return price * 0.18;
               });
```

Time Complexity

```
O(n)
```

---

# accumulate()

Defined in

```cpp
#include <numeric>
```

Calculates cumulative value.

```cpp
std::vector<int> values{10,20,30};

int total = std::accumulate(
    values.begin(),
    values.end(),
    0
);
```

Custom accumulation

```cpp
double portfolioValue =
std::accumulate(
    positions.begin(),
    positions.end(),
    0.0,
    [](double total, const Position& p)
    {
        return total + p.marketValue();
    }
);
```

Time Complexity

```
O(n)
```

---

# partial_sum()

Calculates running totals.

```cpp
std::partial_sum(
    values.begin(),
    values.end(),
    result.begin()
);
```

Input

```
10 20 30 40
```

Output

```
10 30 60 100
```

Time Complexity

```
O(n)
```

---

# find_if()

Finds the first matching element.

```cpp
auto it = std::find_if(
    trades.begin(),
    trades.end(),
    [](const Trade& t)
    {
        return t.price > 100;
    }
);
```

Time Complexity

```
O(n)
```

---

# remove_if()

Removes matching elements.

```cpp
trades.erase(
    std::remove_if(
        trades.begin(),
        trades.end(),
        [](const Trade& t)
        {
            return t.cancelled;
        }),
    trades.end()
);
```

Time Complexity

```
O(n)
```

---

# count_if()

Counts matching elements.

```cpp
int buyTrades =
std::count_if(
    trades.begin(),
    trades.end(),
    [](const Trade& t)
    {
        return t.side == 'B';
    }
);
```

Time Complexity

```
O(n)
```

---

# for_each()

Executes an operation for every element.

```cpp
std::for_each(
    prices.begin(),
    prices.end(),
    [](double& price)
    {
        price *= 2;
    }
);
```

Time Complexity

```
O(n)
```

---

# Common Algorithms Summary

| Algorithm | Purpose | Complexity |
|-----------|---------|------------|
| `sort()` | Sort elements | O(n log n) |
| `transform()` | Modify elements | O(n) |
| `accumulate()` | Sum or aggregate values | O(n) |
| `partial_sum()` | Running total | O(n) |
| `find_if()` | Find matching element | O(n) |
| `remove_if()` | Remove matching elements | O(n) |
| `count_if()` | Count matching elements | O(n) |
| `for_each()` | Apply operation | O(n) |

---

# Performance Notes

- Lambdas are usually inlined by the compiler.
- STL algorithms are highly optimized.
- Algorithms improve readability and reduce implementation bugs.
- `std::function` introduces runtime overhead due to type erasure.
- Prefer templates or `auto` in performance-critical code.

---

# Common Mistakes

Passing large objects by value inside lambdas.

```cpp
[](Trade trade)
```

Prefer

```cpp
[](const Trade& trade)
```

---

Capturing everything unnecessarily.

Avoid

```cpp
[&]
```

Prefer

```cpp
[&portfolio]
```

---

Using `std::function` inside tight loops.

---

Forgetting the Erase-Remove Idiom.

Incorrect

```cpp
std::remove_if(...);
```

Correct

```cpp
container.erase(
    std::remove_if(...),
    container.end()
);
```

---

Writing manual loops when an STL algorithm already exists.

Avoid

```cpp
for (...)
{
    total += value;
}
```

Prefer

```cpp
std::accumulate(...)
```

---

# Best Practices

- Keep lambdas short and focused.
- Capture only the variables you need.
- Pass large objects as `const&`.
- Prefer STL algorithms over handwritten loops.
- Use `std::function` only when runtime flexibility is required.
- Use templates or `auto` in hot paths.
- Learn common STL algorithms before writing custom implementations.

---

# Interview Revision

- Lambda is an anonymous function.
- Capture list controls access to outer variables.
- Capture by value copies variables.
- Capture by reference modifies original variables.
- `std::function` stores any callable object.
- `sort()` performs sorting in O(n log n).
- `transform()` applies an operation to each element.
- `accumulate()` computes aggregate values.
- `partial_sum()` computes running totals.
- `find_if()` returns the first matching element.
- `remove_if()` must be followed by `erase()`.
- Prefer STL algorithms over handwritten loops.
- Prefer explicit captures over `[=]` or `[&]`.
- Avoid `std::function` in performance-critical code.
