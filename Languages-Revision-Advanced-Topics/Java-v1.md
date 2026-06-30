# Core Java for Production Engineering V1

---

## What this guide covers

* OOP fundamentals

  * Composition vs Inheritance
  * Interface vs Abstract Class
  * `equals()` / `hashCode()` contract
  * Immutability
* Collections internals

  * ArrayList vs LinkedList
  * HashMap vs TreeMap vs LinkedHashMap
  * ConcurrentHashMap
* Generics and wildcards
* Exception handling
* Java memory model basics
* Streams API and lambdas
* Optional
* Multithreading and concurrency



---

# 1. OOP Fundamentals

## 1.1 Composition vs Inheritance

### What it is

* **Inheritance** = `is-a`
* **Composition** = `has-a`

Example:

* `EquityTrade is a Trade` → inheritance can make sense
* `TradeService has a TradeValidator` → composition is usually better

### Why we need it

Inheritance shares behavior, but it also creates tight coupling. If the parent changes, child classes can break unexpectedly.

Composition keeps classes independent and easier to test, maintain, and replace.

### Production intuition

In enterprise code, business services usually **use** other objects instead of extending them.

Example:

```java
class TradeService {
    private final TradeValidator validator;
    private final TradeRepository repository;

    TradeService(TradeValidator validator, TradeRepository repository) {
        this.validator = validator;
        this.repository = repository;
    }

    public void processTrade(Trade trade) {
        validator.validate(trade);
        repository.save(trade);
    }
}
```

### Business use case

Used in trade processing, pricing, risk, settlement, and notification services.

### Best practice

Prefer composition unless the relationship is truly stable and naturally `is-a`.

---

## 1.2 Interface vs Abstract Class

### What it is

* **Interface** = contract
* **Abstract class** = shared base implementation + contract

### Why we need it

Interfaces help decouple code and swap implementations. Abstract classes help when multiple classes share common state or helper methods.

### Production intuition

* Use **interfaces** for service contracts
* Use **abstract classes** only when shared logic is genuinely useful

Example:

```java
interface PricingService {
    BigDecimal price(Trade trade);
}
```

```java
abstract class BasePricingService {
    protected BigDecimal round(BigDecimal value) {
        return value.setScale(2, RoundingMode.HALF_UP);
    }
}
```

### Best practice

Default to interface. Use abstract class only when you need shared code or shared fields.

---

## 1.3 `equals()` / `hashCode()` Contract

### What it is

If two objects are logically equal, their hash codes must also be equal.

### Why we need it

Hash-based collections like `HashMap` and `HashSet` depend on this contract.

If you break it, objects may not be found correctly in maps or sets.

### Production intuition

Use stable business fields for equality, especially for IDs and value objects.

Example:

```java
class TradeId {
    private final String value;

    TradeId(String value) {
        this.value = value;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (!(o instanceof TradeId)) return false;
        TradeId tradeId = (TradeId) o;
        return value.equals(tradeId.value);
    }

    @Override
    public int hashCode() {
        return value.hashCode();
    }
}
```

### Common mistake

Override `equals()` but forget `hashCode()`.

### Best practice

Use only stable, immutable fields in equality logic.

---

## 1.4 Immutability

### What it is

An immutable object cannot change after creation.

### Why we need it

Immutable objects are safer in multithreaded code, easier to reason about, and easier to cache.

### Why `String` is immutable

Strings are used everywhere: keys, logs, headers, symbols, IDs. If they changed after being used as keys, many systems would break.

### Why records are immutable

Java records are designed as data carriers:

* concise
* final fields
* no setters
* ideal for DTOs and value objects

### How to design immutable value objects

```java
public final class Money {
    private final String currency;
    private final BigDecimal amount;

    public Money(String currency, BigDecimal amount) {
        this.currency = currency;
        this.amount = amount;
    }

    public String currency() { return currency; }
    public BigDecimal amount() { return amount; }
}
```

### Best practice

Use immutability for IDs, DTOs, and shared objects whenever possible.

---

# 2. Collections Internals

## 2.1 ArrayList vs LinkedList

### What it is

* **ArrayList** = dynamic array
* **LinkedList** = nodes connected by pointers

### Why we need it

Different workloads need different access patterns.

### Internal intuition

* ArrayList stores elements in contiguous memory
* LinkedList stores each element in a node with links to next/previous

### Complexity

* `ArrayList.get(i)` → `O(1)`
* `LinkedList.get(i)` → `O(n)`
* Add at end:

  * ArrayList → amortized `O(1)`
  * LinkedList → `O(1)`
* Insert/remove in middle:

  * ArrayList → `O(n)` due to shifting
  * LinkedList → `O(n)` due to traversal, then pointer updates

### Production use

ArrayList is the default choice in most enterprise Java applications.

### Best practice

Use ArrayList unless you have a strong reason not to.

---

## 2.2 HashMap vs TreeMap vs LinkedHashMap

### What it is

* **HashMap** → fast general-purpose key-value map
* **TreeMap** → sorted by key
* **LinkedHashMap** → preserves insertion order

### Why we need it

Different business cases require speed, ordering, or sorting.

### HashMap intuition

* Uses `hashCode()` to choose a bucket
* Collisions are handled inside buckets
* Resizes when load factor threshold is exceeded

### Load factor

Default load factor is `0.75`.
When size exceeds threshold, the map grows and entries are rehashed.

### Complexity

* HashMap get/put → average `O(1)`
* TreeMap get/put → `O(log n)`
* LinkedHashMap get/put → close to HashMap, with ordering overhead

### Production use

* HashMap: caches, lookups, reference data
* TreeMap: sorted reports, time-series, curves
* LinkedHashMap: ordered output, LRU-style behavior

### Best practice

Choose the map based on actual requirement, not habit.

---

## 2.3 ConcurrentHashMap

### What it is

A thread-safe map for concurrent access.

### Why we need it

In multi-threaded systems, shared `HashMap` usage can corrupt data.

### Production intuition

Used in:

* market data caches
* session caches
* idempotency tracking
* shared reference data

### Best practice

Use `ConcurrentHashMap` for shared mutable state.
Still design carefully for atomic workflows.

---

# 3. Generics and Wildcards

## What it is

Generics let you write reusable, type-safe code.

Example:

```java
List<Trade> trades = new ArrayList<>();
```

### Why we need it

Without generics, you lose type safety and need casting. That pushes bugs to runtime.

### Internal intuition

Java generics are mostly enforced at compile time. At runtime, type information is largely erased.

---

## Wildcards

### `? extends T`

Use when you only need to read from a collection of `T` or its subtypes.

### `? super T`

Use when you want to write `T` values into a collection.

### PECS

* **Producer Extends**
* **Consumer Super**

### Example

```java
public void copyTrades(List<? extends Trade> source, List<? super Trade> target) {
    for (Trade trade : source) {
        target.add(trade);
    }
}
```

### Why this matters in API design

It makes APIs more flexible and reusable.

### Best practice

Use wildcards in method parameters when the method only reads or only writes.

---

# 4. Exception Handling

## Checked vs Unchecked

### Checked exceptions

Compiler forces you to handle them. Usually represent recoverable issues like `IOException`.

### Unchecked exceptions

Usually represent programming errors or invalid business state, like `NullPointerException` or `IllegalArgumentException`.

### Why we need it

In production, some failures are recoverable, and some are bugs or invalid requests.

### Custom exceptions

Use meaningful domain exceptions like:

* `InvalidTradeException`
* `InsufficientBalanceException`
* `OrderRejectedException`

### Try-with-resources

Use it for resources that must be closed:

* database connections
* files
* streams
* sockets

Example:

```java
try (Connection conn = dataSource.getConnection()) {
    // use connection
}
```

### Best practice

Catch specific exceptions. Avoid `catch (Exception e)` except at top level.

---

# 5. Java Memory Model Basics

## 5.1 Stack vs Heap

### Stack

Stores method calls, local variables, and references.

### Heap

Stores actual objects.

### Intuition

Each thread has its own stack.
The heap is shared across threads.

### Production relevance

Understanding stack vs heap helps when debugging memory issues, object lifetime, and concurrency bugs.

---

## 5.2 Garbage Collection

### What it is

Java automatically removes objects that are no longer reachable.

### Generational GC

Conceptually:

* Young generation for short-lived objects
* Old generation for long-lived objects

Most objects die young, so GC optimizes for that.

### Production relevance

Useful when thinking about object allocation rate, latency spikes, and memory pressure.

---

## 5.3 String Pool

### What it is

Java keeps string literals in a shared pool.

### Why it matters

It reduces memory usage because repeated string values can reuse the same object.

### Production relevance

Very useful in systems with repeated values like currency codes, symbols, status values, and headers.

---

# 6. Streams API + Lambdas

## What it is

Streams let you process collections in a pipeline style.

Think of it like readable data processing:

* filter
* transform
* combine
* collect

### Common operations

#### `filter()`

Keeps only matching items.

#### `map()`

Transforms each item.

#### `reduce()`

Combines multiple items into one value.

#### `collect()`

Gathers results into a collection or grouped structure.

### Example

```java
List<Trade> highValueTrades = trades.stream()
    .filter(t -> t.getAmount() > 1_000_000)
    .collect(Collectors.toList());
```

### Why we need it

It makes collection processing cleaner and easier to read than nested loops.

### Production use

Useful in portfolio aggregation, reporting, analytics, and trade filtering pipelines.

### Best practice

Use streams for clarity. Do not force streams where a simple loop is clearer.

---

## Optional

### What it is

A wrapper that represents “value may or may not be present”.

### Why we need it

It helps avoid `null` checks and reduces `NullPointerException`.

### Best practice

Use `Optional` for return values. Avoid using it for fields and method parameters in most cases.

---

# 7. Multithreading and Concurrency

## Thread

A thread is a unit of execution.

### Runnable

Defines the work to be done.

### ExecutorService

Manages threads for you. This is the standard production approach.

Example:

```java
ExecutorService executor = Executors.newFixedThreadPool(10);
executor.submit(() -> processTrade());
```

### Why we need it

Do not create too many threads manually. Thread pools are more efficient and easier to control.

---

## synchronized

### What it is

Ensures only one thread enters a critical section at a time.

### Why we need it

Used to protect shared mutable state like balances, positions, counters, and caches.

### Production relevance

In financial systems, incorrect concurrent updates can lead to wrong positions, wrong P&L, and serious business issues.

---

## volatile

### What it is

Ensures visibility of updates across threads.

### Important

It gives visibility, not atomicity.

### Best use

Good for simple flags, not for counters or complex updates.

---

## CompletableFuture

### What it is

Used for async and parallel workflows.

### Why we need it

Lets you run independent tasks in parallel and combine results later.

### Example use case

A trade response may require:

* pricing
* risk
* customer data
* reference data

These can often run in parallel instead of waiting one by one.

### Best practice

Use it for asynchronous composition, but be careful with exception handling and blocking calls.

---

# 8. Thread Safety in Financial Systems

This is extremely important in trading and banking systems.

## Why it matters

Shared state is everywhere:

* positions
* balances
* exposure
* market data caches
* idempotency tracking

If multiple threads update the same object without protection, you can get wrong business results.

## Common tools

* immutable objects
* `ConcurrentHashMap`
* `synchronized`
* locks
* atomic classes
* thread pools

## Practical rule

Minimize shared mutable state.
When you must share state, protect it carefully.

---

# 9. Common Mistakes

* Using inheritance where composition is better
* Forgetting `hashCode()` when overriding `equals()`
* Using mutable objects as map keys
* Choosing the wrong collection by guesswork
* Using `HashMap` in multi-threaded code
* Overusing `Optional`
* Creating threads manually everywhere
* Assuming `volatile` makes operations atomic
* Swallowing exceptions or catching overly broad exceptions
* Writing stream code that is harder to read than a simple loop

---

# 10. Practical Rules of Thumb

* Prefer composition over inheritance
* Prefer interface over abstract class unless shared implementation is needed
* Make value objects immutable
* Use `HashMap` by default, `TreeMap` only when sorting is needed, and `LinkedHashMap` when order matters
* Use `ConcurrentHashMap` for shared maps
* Use generics and wildcards to make APIs flexible
* Use checked exceptions for recoverable external failures
* Use try-with-resources whenever a resource must be closed
* Use streams for readable collection processing
* Use ExecutorService and CompletableFuture for concurrency, not raw threads
* Protect shared state in financial systems carefully

---

# 11. Quick Revision Summary

* Composition means **has-a**; inheritance means **is-a**
* Interfaces define contracts; abstract classes share behavior
* `equals()` and `hashCode()` must always match
* Immutable objects are safer and easier to maintain
* ArrayList is the default collection choice
* HashMap is fastest for general key-value access
* TreeMap is sorted; LinkedHashMap preserves order
* ConcurrentHashMap is the safe choice for shared maps
* Generics improve type safety
* `? extends` is for reading, `? super` is for writing
* Checked exceptions are for recoverable failures
* Stack stores call frames; heap stores objects
* GC reclaims unreachable objects automatically
* Streams make collection logic cleaner
* Optional helps avoid null-related bugs
* ExecutorService is preferred over manual thread creation
* synchronized protects critical sections
* volatile gives visibility, not atomicity
* Financial systems need strong thread safety because shared state affects money and risk

---

# 12. Final Note

These topics are the foundation of enterprise Java.

If you understand them well, you will be able to:

* read production code faster
* write safer code
* avoid concurrency bugs
* choose the right collection
* design cleaner APIs
* work more confidently in large financial systems

EOF








