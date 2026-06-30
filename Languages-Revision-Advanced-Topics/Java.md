# Core Java for Production Engineering

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
* JVM internals

  * Class loading
  * JIT compilation
  * Garbage collection
  * Memory leaks vs memory pressure
* Deeper concurrency

  * `Lock`, `ReentrantLock`
  * `AtomicInteger`, `AtomicReference`
  * `ThreadLocal`
  * `ReadWriteLock`
* Transactions

  * ACID
  * rollback
  * isolation levels
  * propagation
* Logging and observability

  * SLF4J
  * Logback
  * MDC / correlation IDs
  * metrics and tracing
* Reflection and annotations

  * runtime metadata
  * framework behavior
  * annotation-driven development




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

---

# 1. JVM Internals

## 1.1 Class Loading

### What it is

Class loading is how the JVM finds, reads, and prepares Java classes before using them.

### Why we need it

Java code cannot run until the JVM knows what each class looks like.
If class loading fails, the application may crash at startup or when a class is first used.

### How it works internally

At a high level:

```text
Class file / JAR
      ↓
ClassLoader
      ↓
Load class bytes
      ↓
Verify
      ↓
Prepare
      ↓
Resolve
      ↓
Initialize
```

The JVM usually uses multiple class loaders:

* Bootstrap
* Platform
* Application
* Sometimes custom class loaders in frameworks

### Production relevance

In enterprise systems, class loading matters when:

* the app fails due to missing JARs
* there are dependency conflicts
* multiple versions of a library exist
* a framework loads plugins dynamically

### Common mistakes

* Classpath conflicts
* `ClassNotFoundException`
* `NoClassDefFoundError`
* Putting the wrong version of a dependency in production

### Best practice

Keep dependencies clean and predictable.
Avoid unnecessary custom class loading unless the design truly needs it.

---

## 1.2 JIT Compilation

### What it is

The JVM starts by interpreting bytecode, then compiles frequently used code into optimized machine code.

### Why we need it

It gives Java a balance between portability and performance.

### How it works internally

1. Java source is compiled to bytecode.
2. JVM starts by interpreting bytecode.
3. Hot code paths are detected.
4. JIT compiler optimizes them.
5. Frequently executed code runs faster.

### Production relevance

This is why Java services often get faster after warm-up.

In low-latency systems, warm-up matters because:

* initial requests may be slower
* performance stabilizes after the JVM optimizes hot paths

### Common mistakes

* Assuming first-run latency equals steady-state performance
* Benchmarking without warm-up

### Best practice

When benchmarking Java code, always account for JVM warm-up.

---

## 1.3 Garbage Collection

### What it is

Garbage collection automatically removes objects that are no longer reachable.

### Why we need it

It prevents manual memory management errors like dangling pointers and double frees.

### How it works conceptually

Most objects are short-lived, so JVM groups memory by lifetime:

```text
Heap
├── Young Generation
│   ├── Eden
│   └── Survivor spaces
└── Old Generation
```

Objects created recently go to the young generation.
If they survive long enough, they get promoted to old generation.

### Production relevance

GC affects:

* latency
* throughput
* memory usage
* stop-the-world pauses

In financial systems, long GC pauses can delay trade processing or market data updates.

### Common mistakes

* Creating too many short-lived objects in hot paths
* Ignoring GC logs
* Treating all GC problems as “memory leak” problems

### Best practice

Reduce unnecessary allocations in performance-sensitive code.
Watch GC behavior in production using logs and monitoring.

---

## 1.4 Memory Leaks vs Memory Pressure

### What it is

* **Memory leak**: memory stays referenced accidentally and is never freed
* **Memory pressure**: the program legitimately needs a lot of memory, causing GC to work harder

### Why we need it

Not every memory problem is a leak.
Understanding the difference helps debugging.

### Production relevance

A cache that never evicts entries may look like a leak.
A batch job processing millions of records may simply create memory pressure.

### Best practice

Check:

* object retention
* cache growth
* thread-local misuse
* static collections
* unbounded queues

---

# 2. Deeper Concurrency

## 2.1 Lock and ReentrantLock

### What it is

A lock protects shared mutable state by allowing only one thread at a time into a critical section.

### Why we need it

`synchronized` is simple, but sometimes you need more flexibility:

* timed locking
* interruptible locking
* fairness options

### How it works

```text
Thread A
   ↓
Acquire lock
   ↓
Critical section
   ↓
Release lock

Thread B
   ↓
Wait until lock is free
```

### Example

```java id="jv7t4r"
private final ReentrantLock lock = new ReentrantLock();

public void updatePosition(Position position) {
    lock.lock();
    try {
        // update shared state safely
    } finally {
        lock.unlock();
    }
}
```

### Production relevance

Used when multiple threads update shared state like:

* positions
* balances
* caches
* counters

### Common mistakes

* Forgetting `unlock()` in `finally`
* Holding locks for too long
* Nesting locks and creating deadlocks

### Best practice

Keep locked sections small and focused.
Prefer immutability when possible.

---

## 2.2 AtomicInteger / AtomicReference

### What it is

Atomic classes let you update values safely without using explicit locks for simple operations.

### Why we need it

Useful for counters, flags, and small shared state updates.

### How it works internally

They use low-level CPU-supported atomic operations such as compare-and-swap.

### Example

```java id="z1b6xv"
private final AtomicInteger retryCount = new AtomicInteger(0);

public void recordRetry() {
    retryCount.incrementAndGet();
}
```

### Production relevance

Used for:

* request counters
* retry tracking
* id generation
* lightweight shared state

### Common mistakes

* Using atomic classes for complex multi-step business logic
* Assuming they replace all synchronization needs

### Best practice

Use atomic classes for simple state changes only.

---

## 2.3 ThreadLocal

### What it is

ThreadLocal stores a separate value for each thread.

### Why we need it

Sometimes you want data to stay tied to the current thread, such as:

* request context
* correlation IDs
* security context

### How it works

Each thread has its own copy of the value.

```text
Thread A -> value A
Thread B -> value B
```

### Production relevance

Very common in logging and request tracing.

### Common mistakes

* Forgetting to clear ThreadLocal in thread pools
* Leaking request data across reused threads

### Best practice

Always clean up ThreadLocal values in pooled-thread environments.

---

## 2.4 ReadWriteLock

### What it is

A lock that allows multiple readers or one writer.

### Why we need it

Useful when reads are frequent and writes are rare.

### Production relevance

Good for shared caches, reference data, configuration snapshots, and lookup tables.

### Best practice

Use it only when read-heavy access truly benefits from it.
Do not use it automatically everywhere.

---

# 3. Transactions

## 3.1 ACID

### What it is

Transactions guarantee four things:

* **Atomicity**: all or nothing
* **Consistency**: data stays valid
* **Isolation**: concurrent transactions do not interfere badly
* **Durability**: committed data survives failures

### Why we need it

In financial systems, partial updates are dangerous.

Example:

* money debited from one account
* money not credited to another account

That is unacceptable.

### Production relevance

Used in:

* payments
* order booking
* settlement
* ledger updates
* portfolio changes

### Best practice

Treat transaction boundaries carefully.
Never mix business logic and DB updates casually.

---

## 3.2 Rollback

### What it is

If a transaction fails, all changes made in that transaction are undone.

### Why we need it

Prevents partial or inconsistent updates.

### Production relevance

Very important in trade booking and payment workflows.

### Common mistakes

* Catching exceptions and hiding failures
* Doing external side effects before DB commit
* Assuming rollback happens for all exception types automatically

### Best practice

Keep risky side effects outside the critical DB transaction when possible.

---

## 3.3 Isolation Levels

### What it is

Isolation defines how one transaction sees the changes of another.

### Why we need it

Concurrent transactions can create anomalies if isolation is too weak.

### Common concepts

* Read Uncommitted
* Read Committed
* Repeatable Read
* Serializable

### Production relevance

Important in:

* balances
* trade booking
* inventory-like systems
* reporting jobs

### Best practice

Use the weakest isolation level that still preserves correctness for the use case.

---

## 3.4 Transaction Propagation

### What it is

Propagation decides what happens when one transactional method calls another.

### Why we need it

Enterprise services often call multiple layers, and transaction boundaries must behave predictably.

### Production relevance

Used in service orchestration:

* booking a trade
* writing audit logs
* updating related records

### Best practice

Be deliberate about where transactions begin and end.
Do not spread one transaction across too much work.

---

# 4. Logging and Observability

## 4.1 Logging

### What it is

Logging records what the application is doing.

### Why we need it

In production, logs are often the fastest way to debug issues.

### Production relevance

Used for:

* request tracing
* exception diagnosis
* business events
* audit trails

### Best practice

Log meaningful business events, not everything.

---

## 4.2 SLF4J and Logback

### What it is

* **SLF4J** = logging API
* **Logback** = logging implementation

### Why we need it

This separates logging usage from the actual logging engine.

### Example

```java id="b7p9kq"
private static final Logger log = LoggerFactory.getLogger(TradeService.class);

log.info("Trade received: {}", tradeId);
```

### Best practice

Use parameterized logging, not string concatenation.

---

## 4.3 MDC / Correlation IDs

### What it is

MDC stores per-request diagnostic data such as correlation ID.

### Why we need it

When a request passes through multiple services, logs need a common identifier.

### Production relevance

Essential in microservices, especially in distributed financial systems.

### Example flow

```text
Client Request
    ↓
API Gateway
    ↓
Trade Service
    ↓
Risk Service
    ↓
Settlement Service
```

Same correlation ID should appear in all logs.

### Best practice

Always propagate correlation IDs across service boundaries.

---

## 4.4 Metrics and Tracing

### What it is

* **Metrics** show counts, latency, error rates
* **Tracing** shows request flow across services

### Why we need it

Logs alone are not enough in production.

### Production relevance

Used to monitor:

* API latency
* error spikes
* queue depth
* DB time
* downstream service behavior

### Best practice

Combine logs, metrics, and tracing for full observability.

---

# 5. Reflection and Annotations

## 5.1 Reflection

### What it is

Reflection lets Java inspect and interact with classes, methods, and fields at runtime.

### Why we need it

Frameworks need it to:

* create objects dynamically
* inspect annotations
* wire dependencies
* serialize/deserialize data

### Production relevance

Used heavily by Spring, Hibernate, Jackson, validation frameworks, and testing tools.

### Common mistakes

* Overusing reflection in business code
* Writing slow and fragile code
* Breaking encapsulation carelessly

### Best practice

Use reflection mainly in frameworks and infrastructure code, not in normal business logic.

---

## 5.2 Annotations

### What it is

Annotations attach metadata to code.

Examples:

* `@Service`
* `@Autowired`
* `@Transactional`
* `@JsonProperty`
* `@NotNull`

### Why we need it

Annotations let frameworks apply behavior without hardcoding logic everywhere.

### Production relevance

Very common in Spring-based enterprise applications.

### Example

```java id="g8w2ln"
@Service
public class TradeService {

    @Transactional
    public void bookTrade(Trade trade) {
        // business logic
    }
}
```

### Best practice

Use annotations to express intent clearly, not to hide complex logic.

---

## 5.3 How annotations work internally

### Intuition

Annotations are metadata that can be read by:

* compiler
* runtime frameworks
* annotation processors

### Production relevance

Frameworks inspect them at runtime to decide what to do.

Example:

* `@Transactional` tells Spring to wrap method execution in transaction behavior
* `@RestController` tells Spring to expose HTTP endpoints

---

# 6. Common Mistakes

* Ignoring classpath and dependency issues
* Benchmarking Java without JVM warm-up
* Treating every memory issue as a leak
* Holding locks for too long
* Using `ThreadLocal` carelessly with thread pools
* Overusing reflection in business code
* Relying on logs without correlation IDs
* Mixing too much business logic inside transactions
* Assuming all exceptions cause rollback automatically
* Thinking metrics and tracing are optional in production

---

# 7. Best Practices

* Understand JVM warm-up before judging performance
* Prefer immutability to reduce concurrency bugs
* Keep synchronized or locked sections small
* Use atomic classes for simple state only
* Always clean up `ThreadLocal` values
* Define clear transaction boundaries
* Use meaningful logs with correlation IDs
* Use annotations to declare intent
* Keep reflection inside framework or infrastructure layers
* Monitor with logs, metrics, and tracing together

---

# 8. Quick Revision Summary

* JVM class loading happens before code can run
* JIT improves performance after warm-up
* GC cleans unreachable objects automatically
* Memory leaks and memory pressure are different problems
* Locks protect shared mutable state
* Atomic classes are for simple thread-safe updates
* ThreadLocal gives each thread its own value
* ReadWriteLock helps when reads dominate writes
* Transactions keep database work consistent
* Isolation level controls how concurrent transactions interact
* Logging is essential for production debugging
* MDC helps trace requests across services
* Reflection lets frameworks inspect code at runtime
* Annotations attach metadata that frameworks can use
* These topics are foundational in enterprise Java

---

# 9. Final Note

These topics matter because they are the difference between code that only works locally and code that survives real production traffic.

In financial systems, the important questions are not only:

* Does it compile?
* Does it work?

They are also:

* Is it safe under concurrency?
* Is it observable?
* Is it transactionally correct?
* Is it maintainable under scale?
* Will it behave predictably in production?

That is what these Java topics help you answer.

