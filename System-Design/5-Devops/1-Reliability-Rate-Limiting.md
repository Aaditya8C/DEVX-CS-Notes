# DevOps - Reliability & Rate Limiting

---

## Single Point of Failure (SPOF)

**Definition:** A component whose failure causes the entire system to go down.

**Example:** A single database server — if it crashes, the entire application is unavailable.

### Solutions

#### 1. More Nodes (Replication)
- Run multiple instances of each component.
- If one node fails, others continue serving traffic.

#### 2. Master-Slave Architecture
- **Master** handles all writes.
- **Slaves** replicate data from master and serve reads.
- If master fails, a slave is promoted to master (failover).

```
Writes → Master DB
              │
    ┌─────────┴─────────┐
 Slave DB          Slave DB
    │                   │
 (Reads)            (Reads)
```

#### 3. Load Balancer
- Distributes incoming requests across multiple application servers.
- If one server goes down, the load balancer routes traffic to healthy servers.
- The load balancer itself can also be a SPOF — solved by running redundant load balancers.

```
Client
  │
Load Balancer
  │
┌─────┬─────┐
App1  App2  App3
```

#### 4. Multiple Regions
- Deploy the system across multiple geographic regions (e.g., US-East, US-West, EU).
- If an entire data center or region goes down, traffic fails over to another region.
- Used by Netflix, Amazon, Google in production.

---

## Containers vs Virtual Machines

| Feature | Virtual Machine (VM) | Container |
|---|---|---|
| Isolation | Full OS isolation | Process-level isolation |
| Boot Time | Minutes | Seconds |
| Size | GBs (includes full OS) | MBs (shares host OS kernel) |
| Overhead | High (hypervisor layer) | Low |
| Portability | Less portable | Highly portable |
| Use Case | Running different OSes | Microservices, CI/CD |
| Examples | VMware, VirtualBox, AWS EC2 | Docker, Kubernetes Pods |

**Key Point:** Containers share the host OS kernel — they are faster and lighter, but less isolated than VMs.

---

## Rate Limiting

**Definition:** Controlling how many requests a client can make to a service within a time window.

**Why needed:**
- Prevents abuse and DDoS attacks.
- Protects downstream services from being overwhelmed.
- Ensures fair resource distribution across clients.

---

### Problem 1: Cascading Failure

**Definition:** A failure in one service overloads its dependencies, causing them to fail too — spreading across the system like a cascade.

**Example with 4 servers:**

Suppose Service A calls Service B. Service B is slow or overloaded.

```
Client → Service A → Service B (slow)
                        │
                   Service C → Service D
```

- Service B is slow → Service A threads pile up waiting for B.
- Service A runs out of threads → Service A starts failing.
- Client retries → more load on A and B.
- B overloads C and D → entire system collapses.

**Root cause:** Unbounded request queues + synchronous calls + no circuit breakers.

**Solution: Queues (Asynchronous Decoupling)**
- Instead of Service A calling B directly, A enqueues a message into a queue (e.g., Kafka, RabbitMQ).
- B consumes from the queue at its own pace.
- If B is slow, messages accumulate in the queue — they do not cascade back to A.
- A remains responsive; the queue acts as a buffer.

```
Service A → [Queue] → Service B (processes at own pace)
```

- Additional solutions: **Circuit Breaker** (stop calling a failing service), **Timeouts**, **Retry with backoff**.

---

### Problem 2: Popular Content Creator Post Problem

**Definition:** When a celebrity or popular account posts, a sudden massive spike in reads/writes hits the system — far beyond what normal rate limiting handles.

**Example:** A YouTube channel with 50M subscribers posts a video. Millions of users simultaneously:
- Fetch the video metadata.
- Increment the view counter.
- Post comments.

If each view increments a counter in the database, the DB gets hammered with millions of writes per second.

**Solution: Jitter + Approximate Statistics (YouTube's approach)**

- **Do not count every view precisely in real-time.**
- YouTube uses **approximate counters**: instead of writing to the DB on every view, views are batched and aggregated.
- **Jitter:** Adds a random small delay to requests so they don't all hit the system at the exact same millisecond — spreads the spike over time.

```
View Event → Random small delay (Jitter) → Batched counter update → DB
```

- The displayed view count is an **approximation** (eventual consistency), not an exact real-time number.
- This is acceptable — users do not need to see the exact count to the millisecond.

**Key insight:** For high-cardinality counters under sudden spikes, **approximate statistics with jitter** is far more scalable than exact real-time counting.

---

### Problem 3: Bulk Job Scheduling Problem

**Definition:** Many large background jobs (e.g., report generation, data exports, ML training) are scheduled to run at the same time, overwhelming the system.

**Example:**
- 10,000 users all schedule a "monthly report export" job at midnight on the 1st of every month.
- At midnight, 10,000 jobs fire simultaneously → CPU and DB spike → system crashes or degrades.

**Solution: Batch Processing**

- **Do not execute all jobs immediately when triggered.**
- Group jobs into batches and process them sequentially or with controlled parallelism.
- Spread job execution across a time window (e.g., over 2 hours instead of all at midnight).

```
10,000 Jobs → Job Queue → Worker Pool (e.g., 50 workers) → Process in batches
```

**How it works:**
1. Jobs are submitted to a queue (e.g., Celery, AWS SQS, Kafka).
2. A fixed-size worker pool pulls jobs from the queue.
3. Workers process jobs one by one — system load stays constant.
4. Jobs complete eventually (not instantly), but the system remains stable.

**Real-world use:** AWS Batch, Google Cloud Dataflow, Apache Spark — all use batch processing for bulk workloads.

**Key insight:** Batch processing trades **latency** (jobs take longer to complete) for **stability** (system does not get overwhelmed).
