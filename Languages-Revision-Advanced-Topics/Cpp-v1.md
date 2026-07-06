# C++ Fundamentals Revision Notes

---

# 1. Syntax Refresh

## Data Types

```cpp
int orderId = 101;
double price = 102.45;
char side = 'B';
bool active = true;
std::string symbol = "AAPL";
```

---

## References (`&`)

Another name (alias) for an existing variable.

```cpp
double price = 100.5;
double& ref = price;

ref = 120;
```

✔ No copy created

### Use Case

```cpp
void updateTrade(Trade& trade);
```

---

## Pointers (`*`)

Stores memory address.

```cpp
int x = 10;

int* ptr = &x;

std::cout << *ptr;
```

Use when:

- Dynamic memory
- Optional object
- Interfacing with C APIs

Avoid owning raw pointers.

---

## References vs Pointers

| Reference | Pointer |
|------------|----------|
| Cannot be null | Can be null |
| Cannot change target | Can point elsewhere |
| Safer | More flexible |
| Preferred | Use only when necessary |

---

## Const Correctness

Read-only parameter without copying.

```cpp
void process(const Trade& trade);
```

Avoid

```cpp
void process(Trade trade);
```

Useful combinations

```cpp
const Trade& trade;
const Trade* ptr;
Trade* const ptr;
const Trade* const ptr;
```

---

# Best Practice

Always prefer

```cpp
const T&
```

for large read-only objects.

---

# 2. Memory Model

---

## Stack

Automatic memory.

```cpp
void calculate()
{
    Trade trade;
}
```

Characteristics

- Automatic cleanup
- Very fast
- No memory leaks
- Limited size

---

## Heap

Dynamic memory.

```cpp
Trade* trade = new Trade();
```

Must release manually.

```cpp
delete trade;
```

Problems

- Memory leak
- Double delete
- Dangling pointer
- Undefined behavior

---

## Stack vs Heap

| Stack | Heap |
|---------|------|
| Automatic | Manual |
| Fast | Slower |
| Small | Large |
| No delete | delete required |

---

# 3. RAII

(Resource Acquisition Is Initialization)

Resource lifetime tied to object lifetime.

Instead of

```cpp
Trade* trade = new Trade();

delete trade;
```

Use

```cpp
auto trade = std::make_unique<Trade>();
```

Benefits

- No leaks
- Exception safe
- Automatic cleanup

Common RAII Classes

- unique_ptr
- ifstream
- lock_guard
- fstream
- database connection wrappers

---

# 4. Smart Pointers

---

## unique_ptr

Single owner.

```cpp
auto trade = std::make_unique<Trade>();
```

✔ Fastest smart pointer

Cannot copy

Can move

Use when ownership is exclusive.

---

## shared_ptr

Shared ownership.

```cpp
auto trade = std::make_shared<Trade>();
```

Internally maintains reference count.

Use only when multiple owners exist.

Downside

- Atomic reference counting
- Slightly slower

---

## weak_ptr

Non-owning observer.

Prevents circular references.

```cpp
std::weak_ptr<Trade> trade;
```

---

## Which One?

| Scenario | Pointer |
|-----------|---------|
| Single owner | unique_ptr |
| Multiple owners | shared_ptr |
| Observe only | weak_ptr |

---

## Avoid Smart Pointers

In hot-path numerical code.

Prefer

```cpp
std::vector<double>
```

instead of

Millions of individually allocated heap objects.

---

# 5. STL Containers

---

## vector

Dynamic contiguous array.

```cpp
std::vector<double> prices;
```

Complexity

Access

O(1)

Push Back

Amortized O(1)

Middle Insert

O(n)

Best for

- Prices
- Trades
- Risk values

---

## array

Fixed size.

```cpp
std::array<double,10> prices;
```

Fastest sequence container.

---

## deque

Insert/remove both ends efficiently.

```cpp
std::deque<int> window;
```

Best for

Sliding window calculations.

---

## unordered_map

Hash table.

```cpp
std::unordered_map<std::string, Trade>
```

Average Complexity

Lookup

O(1)

Best for

Symbol lookup

Order lookup

Caches

---

## map

Balanced BST.

Always sorted.

Complexity

O(log n)

Best for

Sorted traversal

Leaderboards

Order books

---

## Container Comparison

| Container | Lookup | Insert | Ordered |
|------------|---------|---------|----------|
| vector | O(1) index | End O(1) | No |
| array | O(1) | Fixed | No |
| deque | O(1) ends | O(1) ends | No |
| unordered_map | O(1) avg | O(1) avg | No |
| map | O(log n) | O(log n) | Yes |

---

# 6. Move Semantics

Purpose

Avoid unnecessary copies.

---

Old

```
Object A
     ↓ Copy
Object B
```

Modern

```
Object A
     ↓ Move
Object B
```

Ownership transferred.

---

## std::move

```cpp
std::vector<int> prices = {1,2,3};

std::vector<int> another = std::move(prices);
```

After move

```
prices → Empty

another → Owns memory
```

---

## Production Example

```cpp
portfolio.addTrade(std::move(trade));
```

Ownership transferred.

No expensive copy.

---

## Return Value Optimization (RVO)

```cpp
std::vector<Trade> loadTrades()
{
    std::vector<Trade> trades;

    return trades;
}
```

Compiler usually eliminates copy.

Don't write

```cpp
return std::move(trades);
```

---

# Complexity

| Operation | Complexity |
|------------|------------|
| Copy | O(n) |
| Move | O(1) |

---

# Performance Tips

✅ Pass large objects using

```cpp
const T&
```

---

✅ Prefer stack allocation.

---

✅ Use RAII.

---

✅ Prefer unique_ptr over shared_ptr.

---

✅ Use vector as default container.

---

✅ Reserve vector capacity.

```cpp
std::vector<Trade> trades;

trades.reserve(100000);
```

---

✅ Prefer emplace_back()

```cpp
trades.emplace_back(symbol, price, qty);
```

instead of

```cpp
trades.push_back(Trade(symbol, price, qty));
```

---

# Enterprise Usage

| Component | Common Usage |
|------------|--------------|
| Pricing Engine | vector + move semantics |
| Market Data | vector |
| Risk Engine | const references |
| Order Book | unordered_map / map |
| Portfolio | unique_ptr |
| File Handling | RAII |
| Database Connections | RAII |
| Locks | lock_guard |

---

# Common Mistakes

❌ Passing objects by value

```cpp
void process(Trade trade);
```

✔

```cpp
void process(const Trade& trade);
```

---

❌ Using new/delete directly.

---

❌ Using shared_ptr everywhere.

---

❌ Returning

```cpp
return std::move(localObject);
```

when RVO applies.

---

❌ Using map when unordered_map is sufficient.

---

# Interview One-Liners

- References are aliases; pointers store addresses.
- `const T&` avoids copies while preventing modification.
- Stack memory is automatic; heap memory is dynamic.
- RAII ties resource lifetime to object lifetime.
- `unique_ptr` → single ownership.
- `shared_ptr` → shared ownership.
- `weak_ptr` → non-owning reference.
- `vector` is the default STL container.
- `unordered_map` provides average O(1) lookup.
- Move semantics transfer ownership instead of copying.
- `std::move` enables move semantics.
- RVO usually removes copies during return.
- Modern C++ avoids manual `new` and `delete`.
