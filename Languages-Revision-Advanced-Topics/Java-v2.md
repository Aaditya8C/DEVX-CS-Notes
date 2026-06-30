# Core Java for Production Engineering V2

---

## Topics covered

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
