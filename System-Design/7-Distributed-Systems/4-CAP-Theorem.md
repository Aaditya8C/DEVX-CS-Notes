# CAP Theorem

## What is the CAP Theorem?

The **CAP Theorem** (Brewer's Theorem) states:

> In any distributed system, you can only **guarantee 2 out of 3** properties simultaneously:
> - **C** — Consistency
> - **A** — Availability
> - **P** — Partition Tolerance

```
              Consistency (C)
                   /\
                  /  \
                 /    \
                / CA   \
               /        \
              /    CAP   \
             /  (impossible)\
            /────────────────\
  Availability (A)    Partition Tolerance (P)
         AP                  CP
```

---

## The Three Properties Explained

---

### C — Consistency

> Every read receives the **most recent write** or an error.

All nodes in the system **see the same data at the same time**.

```
  CONSISTENT SYSTEM:
  User A writes: x = 5

  Node 1: x = 5  (up to date)
  Node 2: x = 5  (up to date)
  Node 3: x = 5  (up to date)

  ANY node you read from → returns 5

  NOT CONSISTENT (Inconsistent):
  User A writes: x = 5

  Node 1: x = 5  (up to date)
  Node 2: x = 3  (stale, hasn't replicated yet)
  Node 3: x = 3  (stale)
```

**Example:** Bank balance must be consistent. If you transfer 5000, every ATM worldwide must immediately show the updated balance. Showing stale data would be wrong.

---

### A — Availability

> Every request receives a **response** (not an error) — though it may not be the most recent data.

The system is **always up and responding**, even if data might be stale.

```
  AVAILABLE SYSTEM:
  User reads x from Node 2

  Even if Node 2 is behind or Node 1 crashed:
  → Node 2 still responds (may return old value)
  → No timeout, no error

  NOT AVAILABLE (Unavailable):
  → System returns error 503
  → Or request times out
  → "Sorry, currently unavailable"
```

**Example:** Google Search always returns results, even if they are slightly stale. It never says "No results, we're down." — it just returns what it has.

---

### P — Partition Tolerance

> The system **continues to operate** even when network partitions (communication failures between nodes) occur.

A **network partition** = nodes cannot communicate with each other.

```
  NETWORK PARTITION:

  [ Node 1 ] ─── NETWORK BREAK ─── [ Node 2 ]
      │                                  │
  US Data Center                    EU Data Center

  Nodes are alive, but cannot talk to each other!

  PARTITION TOLERANT SYSTEM:
  → Both nodes continue to serve requests independently
  → System doesn't crash

  NOT PARTITION TOLERANT:
  → System halts until the network is restored
  → Not practical for distributed systems
```

**Why P is always required in real systems:**

Network failures **ALWAYS** happen in distributed systems. You cannot guarantee a perfect network. Therefore, **P is not optional** in practice — you must tolerate partitions.

This is why the real choice is **CP vs AP**.

---

## The Real Tradeoff: CP vs AP

Since P is mandatory, you choose between **C and A** during a partition:

```
  PARTITION HAPPENS ──► CHOOSE:

  Option 1 (CP): Reject the request → ensure consistency
                 "I will return an error rather than stale data"

  Option 2 (AP): Return stale data → ensure availability
                 "I will return what I have, even if outdated"
```

---

## The Three Combinations

---

### CP — Consistency + Partition Tolerance

**Sacrifices:** Availability

During a network partition, the system **refuses to serve requests** rather than return inconsistent data.

```
  Normal:               During Partition:

  [Leader] ──► [R1]    [Leader] ─BROKEN─ [R1]
              [R2]                        [R2]

  Read/Write OK         Leader: "I can't verify
                         consistency → returning ERROR"
```

**Behavior:**
- Writes might be rejected during partition
- Reads may return an error if they can't confirm freshness
- System comes back when partition heals

**Example — ZooKeeper (Distributed Coordination):**
```
  ZooKeeper cluster: 5 nodes (need majority = 3)

  Partition: 2 nodes isolated from 3 nodes

  3-node side: can form quorum → continues
  2-node side: "I don't have majority" → REJECTS all writes
               Availability sacrificed for consistency!
```

**Real-World Use Cases:**

| System      | Why CP                                              |
|-------------|-----------------------------------------------------|
| ZooKeeper   | Config/coordination must be exactly correct         |
| HBase       | HDFS-backed; prefers errors over inconsistency      |
| MongoDB     | (Primary mode) — rejects writes without quorum      |
| Redis Sentinel | Leader-based; read-only during failover          |

---

### AP — Availability + Partition Tolerance

**Sacrifices:** Consistency (strong consistency)

During a partition, all nodes continue serving requests, potentially returning **stale/different data**.

```
  Network Split:

  [US Node] ─BROKEN─ [EU Node]

  US user: read → US Node returns "x = 5" (latest)
  EU user: read → EU Node returns "x = 3" (stale)

  Different users see different values ← Inconsistent
  But system keeps running ← Available

  After partition heals → nodes sync → eventually consistent
```

**Behavior:**
- Every node responds, even during partition
- Data may be stale or conflicting
- Eventual consistency — data converges after partition heals

**Example — DNS (Domain Name System):**
```
  You update your domain's IP: example.com → 203.0.113.1

  DNS Server A (updated): returns 203.0.113.1 (new)
  DNS Server B (not yet): returns 192.168.1.1  (old)

  Both servers respond ← Always available
  Eventually all DNS servers update ← Eventual consistency
  (This is by design! DNS prioritizes availability)
```

**Real-World Use Cases:**

| System       | Why AP                                             |
|--------------|----------------------------------------------------|
| Cassandra    | Always available; tunable consistency              |
| CouchDB      | Offline-first, syncs later                         |
| DynamoDB     | Highly available by default; optional strong reads |
| Amazon S3    | Eventual consistency, always returns something     |
| DNS          | High availability over perfect consistency         |

---

### CA — Consistency + Availability

**Sacrifices:** Partition Tolerance

System is consistent and available **but cannot handle network partitions**.

> This is not practical for distributed systems.
> Network partitions always happen. CA is only achievable in **single-node** or **perfectly reliable network** systems.

```
  CA System (impractical):

  Assumption: No network failures EVER

  → If network fails → system crashes
  → Basically a single node database

  Traditional RDBMS on a single node:
  PostgreSQL on one server → CA (no partition possible)
  Once you add replicas → P comes into play
```

**In practice:** When vendors say "CA database" they usually mean a single-node relational database that doesn't handle distribution.

---

## CAP Combinations Summary

```
           Consistency
                │
         ┌──────┴───────┐
         │              │
         ▼              ▼
        CP              AP
  (ZooKeeper,      (Cassandra,
   HBase,           DynamoDB,
   MongoDB*)         DNS,
                    CouchDB)

         CA  ← only single-node, impractical for distributed
```

| Combination | C   | A   | P   | Examples                          | Trade-off                            |
|-------------|-----|-----|-----|-----------------------------------|--------------------------------------|
| CP          | Yes | No  | Yes | ZooKeeper, HBase, MongoDB          | Rejects requests during partition    |
| AP          | No  | Yes | Yes | Cassandra, DynamoDB, DNS, CouchDB | Returns stale data during partition  |
| CA          | Yes | Yes | No  | Single-node RDBMS (PostgreSQL)    | Cannot handle partitions             |

---

## Real-Life Application Examples

### Banking System (CP)

```
  Bank Balance: 10,000
  Two ATMs try to withdraw 10,000 simultaneously
  (Network Partition between Bank Nodes)

  CP System:
  ATM 1: "Can't verify — REJECT" (safe)
  ATM 2: "Can't verify — REJECT" (safe)

  After partition heals → both withdrawals processed normally

  Why CP? Inconsistency = double spending = catastrophic
```

### Social Media (AP)

```
  User posts: "Happy Birthday Mom!"

  AP System:
  US servers:   "Post visible"
  EU servers:   "Post not visible yet" (replicating)
  Asia servers: "Post not visible yet" (replicating)

  → After a few seconds, everyone sees the post

  Why AP? A delayed post is okay.
           Being down is not okay (users go elsewhere).
```

### Shopping Cart (AP — Amazon's choice)

```
  Amazon's Shopping Cart: AP system

  User adds item to cart on US node
  User checks cart on EU node (different partition)
  → May not see the item immediately (stale read)
  → But cart never shows "ERROR: Unavailable"

  Design choice: "Better to have slightly stale cart
                  than to show an error page."
```

### Leader Election / Config (CP — ZooKeeper)

```
  ZooKeeper manages: which node is the Kafka leader?

  Network partition: 3 nodes vs 2 nodes

  3-node side: "We have quorum → continue"
  2-node side: "We don't have majority → read-only/error"

  Why CP? Two Kafka nodes thinking they're leader
           = split-brain = data corruption = catastrophic
```

---

## PACELC Extension (Beyond CAP)

CAP only talks about behavior during partitions.
**PACELC** extends it: even without partitions, there's a tradeoff.

```
  PAC:  During Partition → Choose A or C
  ELC:  Else (normal) → Choose Latency or Consistency

  Example: Cassandra
  - During partition: AP (available, eventually consistent)
  - Normal operation: Choose low Latency over Consistency (tunable)
```

---

## Key Takeaways

- **C** = All nodes see the same data (no stale reads)
- **A** = System always responds (no errors/timeouts)
- **P** = System works even during network failures
- P is **mandatory** in distributed systems → real choice is **CP vs AP**
- **CP:** ZooKeeper, HBase, MongoDB → prefers errors over inconsistency
- **AP:** Cassandra, DynamoDB, DNS → prefers stale data over errors
- **CA:** Single-node databases only (not truly distributed)
- Choose based on your **business requirements** — banking = CP, social = AP
