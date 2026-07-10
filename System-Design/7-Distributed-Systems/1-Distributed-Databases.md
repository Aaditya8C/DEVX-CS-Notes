# Distributed Databases

## What is a Distributed Database?

A **distributed database** is a database that is spread across **multiple machines (nodes)** connected by a network, but appears to users as a **single unified system**.

Data is physically stored in different locations, yet users interact with it as if it's one database.

---

## Single Node vs Distributed

```
SINGLE NODE (Traditional):

  [ Client ] ──► [ Single DB Server ]
                       │
                   (All data here)

DISTRIBUTED:

  [ Client ] ──► [ DB Node 1 ]  ←── Region: US-East
                 [ DB Node 2 ]  ←── Region: EU-West
                 [ DB Node 3 ]  ←── Region: Asia-Pacific
```

---

## Why Do We Need Distributed Databases?

### 1. Scalability

A single machine has physical limits:
- Limited RAM, CPU, Disk
- Cannot handle millions of requests per second

**Solution:** Distribute across many machines → scale horizontally

```
Single Node:
  10,000 req/sec → BOTTLENECK

Distributed (10 Nodes):
  10,000 req/sec → 1,000 req/sec per node
```

---

### 2. High Availability & Fault Tolerance

If a single database goes down → **entire system fails**.

With distribution, if one node fails, others continue serving requests.

```
  [ Node 1 ] ──CRASH──►  (down)
  [ Node 2 ] ──────────►  still serving
  [ Node 3 ] ──────────►  still serving
```

**Uptime goal:** 99.999% = ~5 minutes downtime per year (five nines)

---

### 3. Low Latency (Geographic Distribution)

Serving users from the nearest data center reduces response time.

```
  User in India  ──► Asia Node  (10ms)   — fast
  User in India  ──► US Node    (250ms)  — slow (if single DC)
```

---

### 4. Disaster Recovery

Data replicated across regions survives physical disasters (fire, flood, power outage).

---

### 5. Handling Big Data

Petabytes of data cannot fit on a single disk.
Distribution allows data to be split (partitioned) across many nodes.

---

## Key Challenges

| Challenge            | Description                                       |
|----------------------|---------------------------------------------------|
| Consistency          | All nodes seeing the same data at the same time   |
| Network Partitions   | Network failures splitting the cluster            |
| Latency              | Coordination overhead between nodes               |
| Partial Failures     | Some nodes crash while others continue            |
| Concurrency          | Multiple writes hitting different nodes           |

---

## Real-World Examples

| System       | Type                  | Used By           |
|--------------|-----------------------|-------------------|
| Google Spanner | Distributed SQL     | Google            |
| Amazon DynamoDB | Distributed NoSQL  | Amazon            |
| Apache Cassandra | Wide-column store  | Netflix, Discord  |
| CockroachDB  | Distributed SQL       | Financial systems |
| MongoDB Atlas | Distributed Document | Many startups     |

---

## Key Takeaways

- A distributed database spans multiple machines but appears as one
- Required for: scalability, availability, low latency, disaster recovery, big data
- Main challenges: consistency, network partitions, partial failures
- It's the foundation of almost every modern large-scale system
