# C++ Language Features for Numeric Code - Revision Notes

---

# 1. Templates & Generic Programming

## What are Templates?

Templates allow writing **generic code** that works with multiple data types.

Write once → Compiler generates type-specific code.

```
Template
    ↓
Compiler
    ↓
int version
double version
Money version
Decimal version
```

---

## Function Template

```cpp
template<typename T>
T add(T a, T b)
{
    return a + b;
}
```

Usage

```cpp
add(10, 20);
add(10.5, 20.5);
```

Compiler generates separate functions for each type.

---

## Class Template

```cpp
template<typename T>
class Portfolio
{
private:
    std::vector<T> values;

public:

    void add(T value)
    {
        values.push_back(value);
    }

    T latest() const
    {
        return values.back();
    }
};
```

Usage

```cpp
Portfolio<double> prices;
Portfolio<Money> balances;
Portfolio<Decimal> pnl;
```

---

## Why Templates?

Without templates

```
PriceCalculatorDouble

PriceCalculatorDecimal

PriceCalculatorMoney
```

With templates

```
PriceCalculator<T>
```

One implementation.

Multiple data types.

---

## Template Metaprogramming (Awareness)

Templates can execute logic during **compile time**.

Common modern features

- constexpr
- if constexpr
- type_traits
- concepts (C++20)

Mostly used in

- Boost
- Eigen
- QuantLib
- STL

---

## Advantages

- Code reuse
- Type safety
- Zero runtime overhead
- Better compiler optimization

---

## Disadvantages

- Slower compilation
- Larger binaries (multiple instantiations)
- Complex compiler errors

---

# Best Practices

✅ Use templates when logic is independent of data type.

❌ Don't replace normal polymorphism unnecessarily.

---

# 2. constexpr

## What is constexpr?

Compute values during **compile time** instead of runtime.

```
Source Code
      ↓
Compiler Calculates
      ↓
Executable Contains Result
```

---

## Constant Variable

```cpp
constexpr double PI = 3.1415926535;
```

---

## constexpr Function

```cpp
constexpr int square(int x)
{
    return x * x;
}
```

Usage

```cpp
constexpr int result = square(10);
```

Compiler stores

```
100
```

No runtime computation.

---

## Production Examples

```cpp
constexpr int DAYS_PER_YEAR = 365;

constexpr double DISCOUNT_FACTOR = 0.95;

constexpr double RISK_WEIGHT = 0.12;
```

---

## Benefits

- Faster execution
- Better optimization
- Safer constants
- Zero runtime cost

---

## Best Use Cases

- Mathematical constants
- Financial constants
- Lookup tables
- Configuration values
- Array sizes

---

# 3. Operator Overloading

## What is Operator Overloading?

Allows custom classes to behave like built-in types.

Instead of

```cpp
money.add(otherMoney);
```

Write

```cpp
money + otherMoney;
```

Cleaner and easier to read.

---

## Internal Working

Compiler converts

```cpp
a + b;
```

to

```cpp
a.operator+(b);
```

or

```cpp
operator+(a, b);
```

---

## Example

```cpp
class Money
{
private:
    double amount;

public:

    Money(double amt)
        : amount(amt)
    {}

    Money operator+(const Money& other) const
    {
        return Money(amount + other.amount);
    }

    double value() const
    {
        return amount;
    }
};
```

Usage

```cpp
Money salary(5000);
Money bonus(1000);

Money total = salary + bonus;
```

---

## Commonly Overloaded Operators

```
+
-
*
/
==
!=
<
>
+=
-=
<<
>>
```

---

## Production Usage

Custom numeric types

- Money
- Decimal
- Matrix
- ComplexNumber
- Vector

Examples

```cpp
Money pnl = profit + interest;

Matrix result = matrixA * matrixB;

Decimal total = amount1 + amount2;
```

Makes mathematical formulas readable.

---

# Time Complexity

## Templates

| Metric | Complexity |
|----------|------------|
| Runtime | O(0) overhead |
| Compile Time | Higher |
| Binary Size | May increase |

---

## constexpr

| Metric | Complexity |
|----------|------------|
| Runtime | O(0) if evaluated at compile time |
| Compile Time | Slight increase |

---

## Operator Overloading

Depends on implementation.

Example

Money addition

```
O(1)
```

Matrix multiplication

```
O(n³)
```

Operator syntax does **not** change algorithm complexity.

---

# Enterprise Usage

## Templates

Used in

- Pricing Engine
- Risk Engine
- Matrix Libraries
- Generic Collections
- Quantitative Models

---

## constexpr

Used for

- Trading constants
- Risk constants
- Discount factors
- Mathematical constants
- Compile-time lookup tables

---

## Operator Overloading

Used in

- Money
- Decimal
- Matrix
- Vector
- Complex Number
- Financial Formula Libraries

---

# Common Mistakes

❌ Writing templates when only one type is needed.

❌ Making templates overly complex.

❌ Overloading operators with unexpected behavior.

Example

```cpp
a + b;
```

should **never** modify either operand.

---

❌ Returning reference to local object.

Wrong

```cpp
Money& operator+(...)
```

Correct

```cpp
Money operator+(...) const
```

---

❌ Forgetting `const`

Correct

```cpp
Money operator+(const Money& other) const;
```

---

# Best Practices

✅ Keep templates simple.

✅ Use `constexpr` for constants and compile-time calculations.

✅ Overload operators only when semantics are intuitive.

✅ Return objects by value for arithmetic operators.

✅ Pass large operands using `const&`.

---

# Performance Tips

### Templates

- Zero runtime overhead.
- Compiler performs aggressive inlining.
- Eliminates virtual function calls.

---

### constexpr

- Eliminates runtime calculations.
- Generates highly optimized machine code.

---

### Operator Overloading

- Keep implementations lightweight.
- Avoid unnecessary object copies.
- Combine with move semantics where applicable.

---

# Interview One-Liners

- Templates generate type-specific code at compile time.
- Function templates create generic functions.
- Class templates create generic classes.
- Template metaprogramming performs computation during compilation.
- `constexpr` enables compile-time computation.
- `constexpr` improves performance by removing runtime work.
- Operator overloading makes custom types behave like built-in types.
- Overload operators only when behavior is intuitive.
- Templates have zero runtime overhead but increase compile time.
- `constexpr` is ideal for constants and mathematical formulas.

