# CAP Theorem

## What is it?

In any distributed system, you can only **guarantee 2 out of 3**:

```
          Consistency (C)
               /\
              /  \
             / CA \
            /──────\
  Availability   Partition Tolerance
      (A)              (P)
      AP                CP
```

---

## The Three Properties

**Consistency (C)**
Every read returns the most recent write or an error. All nodes see the same data.

**Availability (A)**
Every request gets a response (may be stale data). System never returns an error.

**Partition Tolerance (P)**
System continues to operate even when nodes can't communicate with each other.

> P is **mandatory** in distributed systems — network failures always happen.
> The real choice is always **CP vs AP**.

---

## CP — Consistency + Partition Tolerance

Sacrifices: **Availability**

During a network partition, the system **rejects requests** rather than return inconsistent data.

```
  Partition: 3 nodes vs 2 nodes (ZooKeeper, majority = 3)
  3-node side: continues serving
  2-node side: REJECTS all writes (no quorum)
```

| System         | Why CP                                       |
|----------------|----------------------------------------------|
| ZooKeeper      | Coordination config must be exactly correct  |
| HBase          | Prefers errors over inconsistency            |
| MongoDB        | Rejects writes without quorum                |

---

## AP — Availability + Partition Tolerance

Sacrifices: **Strong Consistency** (allows eventual consistency)

During a partition, all nodes keep serving — but may return **stale data**.

```
  Network Split:
  US Node: x = 5 (latest write)
  EU Node: x = 3 (stale — hasn't replicated yet)

  Both nodes still respond → Available
  After healing → data syncs → Eventually Consistent
```

**Example — DNS:** Server A returns updated IP, Server B still returns old IP. Both respond. Eventually all sync.

| System     | Why AP                                      |
|------------|---------------------------------------------|
| Cassandra  | Always available, tunable consistency       |
| DynamoDB   | Highly available by default                 |
| DNS        | Availability over perfect consistency       |
| CouchDB    | Offline-first, syncs later                  |

---

## CA — Consistency + Availability

Sacrifices: **Partition Tolerance**

Only works on a **single node** or perfect network — not practical for distributed systems.

Single-node PostgreSQL is CA: consistent + available, but no partitions are possible.

---

## Summary Table

| Combination | C   | A   | P   | Examples                        | Trade-off                          |
|-------------|-----|-----|-----|---------------------------------|------------------------------------|
| CP          | Yes | No  | Yes | ZooKeeper, HBase, MongoDB       | Rejects requests during partition  |
| AP          | No  | Yes | Yes | Cassandra, DynamoDB, DNS        | Returns stale data during partition|
| CA          | Yes | Yes | No  | Single-node RDBMS               | Cannot handle partitions           |

---

## Real-Life Decisions

| System          | Choice | Reason                                             |
|-----------------|--------|----------------------------------------------------|
| Banking         | CP     | Inconsistency = double spending = catastrophic     |
| Social Media    | AP     | Delayed post is fine; downtime is not              |
| Amazon Cart     | AP     | Stale cart is ok; "unavailable" error is not       |
| Kafka Leader    | CP     | Two leaders = split-brain = data corruption        |

---

## PACELC (Extension to CAP)

Even without partitions there is a tradeoff:
- **During Partition** → choose A or C
- **Else (normal)** → choose Latency or Consistency

Example: Cassandra is AP during partition, and favors low Latency over Consistency in normal operation.
