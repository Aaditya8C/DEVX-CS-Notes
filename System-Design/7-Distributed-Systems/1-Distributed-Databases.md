# Distributed Databases

## What is it?

A database spread across **multiple machines (nodes)** that appears as a **single system** to users.

---

## Why Distributed Databases?

| Reason               | Problem it Solves                                 |
|----------------------|---------------------------------------------------|
| Scalability          | Single machine hits CPU/RAM/disk limits           |
| High Availability    | Single node failure = entire system down          |
| Low Latency          | Serve users from nearest data center              |
| Disaster Recovery    | Data survives regional outages                    |
| Big Data             | Petabytes can't fit on one disk                   |

---

## Key Challenges

- **Consistency** — all nodes seeing the same data
- **Network Partitions** — nodes losing communication
- **Partial Failures** — some nodes crash, others keep running
- **Concurrency** — multiple writes to different nodes

---

## Real-World Examples

| System           | Type               | Used By           |
|------------------|--------------------|-------------------|
| Google Spanner   | Distributed SQL    | Google            |
| Amazon DynamoDB  | Distributed NoSQL  | Amazon            |
| Apache Cassandra | Wide-column store  | Netflix, Discord  |
| CockroachDB      | Distributed SQL    | Financial systems |
