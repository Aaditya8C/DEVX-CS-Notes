# Replication in Distributed Systems

## What is Replication?

**Replication** means keeping **copies of the same data on multiple nodes**.

```
           WRITE
  [ Client ] ──────► [ Primary Node ]
                            │
                    ┌───────┴───────┐
                    ▼               ▼
             [ Replica 1 ]   [ Replica 2 ]
```

Each node that stores a copy is called a **replica**.

---

## Why Replication?

### 1. High Availability
If one node fails, another replica serves the data.

```
  [ Primary ]  ──CRASH──►  (down)
  [ Replica 1 ] ──────────►  takes over
  [ Replica 2 ] ──────────►  still readable
```

### 2. Fault Tolerance
Survives hardware failures, network issues, power outages.

### 3. Read Scalability
Multiple replicas can serve read requests simultaneously.

```
  1000 read requests/sec
      │
  ┌───┼───┐
  ▼   ▼   ▼
 R1  R2  R3   ← each handles ~333 req/sec
```

### 4. Low Latency (Geo-distribution)
Place replicas near users in different regions.

```
  [ US Replica ] ← US users
  [ EU Replica ] ← EU users
  [ Asia Replica ] ← Asia users
```

---

## Replication Algorithms

Three main approaches:

1. Single-Leader (Master-Slave) Replication
2. Multi-Leader Replication
3. Leaderless Replication

---

# 1. Single-Leader Replication

## Concept

One node is designated as the **leader (primary/master)**.

- **All writes** go to the leader
- **Leader replicates** changes to followers (replicas)
- **Reads** can go to leader or followers

```
             WRITE
  [ Client ] ──────► [ Leader (Primary) ]
                             │
               ┌─────────────┴─────────────┐
               │                           │
               ▼                           ▼
     [ Follower 1 (Replica) ]   [ Follower 2 (Replica) ]
               │                           │
           (READ)                       (READ)
```

---

## Example: E-Commerce Order System

- **Write:** User places an order → goes to Leader DB
- **Read:** Show order history → can read from any Follower

```
  POST /order  ──► [ Leader DB ]
                        │
                   replicates
                    ┌───┴───┐
                    ▼       ▼
               [Follower1] [Follower2]

  GET /orders  ──► [Follower1]  (load balanced)
```

---

## Synchronous Replication

The leader **waits for acknowledgement** from the follower before confirming write to client.

```
  Client ──WRITE──► Leader ──REPLICATE──► Follower
    ▲                                        │
    │                                        │
    └─────────────── ACK ◄───────────────────┘
         (only responds after follower confirms)
```

**Pros:**
- Follower is guaranteed to be up-to-date
- No data loss if leader crashes (follower has latest data)

**Cons:**
- Slower (must wait for follower to respond)
- If follower is slow/unavailable → write is blocked
- Not practical to have ALL followers synchronous

**Used when:** Strong consistency is critical (e.g., banking, financial systems)

---

## Asynchronous Replication

The leader **does NOT wait** for follower acknowledgement. It confirms to client immediately and replicates in the background.

```
  Client ──WRITE──► Leader ──ACK──► Client (confirmed)
                        │
                   (later, in background)
                        ▼
                    [Follower]
```

**Pros:**
- Fast (no waiting for replicas)
- Leader can still write even if followers are down
- Better throughput

**Cons:**
- **Replication lag** — followers may be behind
- If leader crashes before replication → **data loss**

**Used when:** High availability and performance matter more than consistency (e.g., social media feeds)

---

## Semi-Synchronous (Hybrid)

One follower is synchronous, rest are asynchronous. This is a common practical choice.

```
  Leader ──SYNC──► Follower 1 (synchronous)
         ──ASYNC─► Follower 2 (asynchronous)
         ──ASYNC─► Follower 3 (asynchronous)
```

At least one up-to-date copy always exists → safe failover.

---

## Leader Failover

If the leader crashes:
1. Detect failure (via heartbeats / timeout)
2. Elect new leader (usually the most up-to-date follower)
3. Redirect writes to new leader

```
  [ Leader ]  ──CRASH──► (down)

  Follower 1: "I have latest data" ──► Becomes new Leader
  Follower 2: Follows new leader
```

**Problems with Failover:**
- Split-brain: two nodes both think they are leader (dangerous!)
- Data loss: async replication may mean some writes were lost
- Choosing the right timeout (too short → false positives)

---

## Single-Leader Summary

| Aspect       | Detail                                |
|--------------|---------------------------------------|
| Writes       | Only to leader                        |
| Reads        | Leader or followers                   |
| Sync Mode    | Waits for replica ACK, slower        |
| Async Mode   | Does not wait, faster, risk of lag   |
| Failover     | Elect new leader from followers       |
| Use Case     | MySQL replication, PostgreSQL streaming|

---

# 2. Multi-Leader Replication

## Concept

Multiple nodes can **accept writes** simultaneously (each is a leader in its own right) and they replicate writes to each other.

```
  Data Center US                Data Center EU
  ┌────────────────┐            ┌────────────────┐
  │ [ Leader US ]  │ ◄────────► │ [ Leader EU ]  │
  │     │          │            │     │          │
  │ [Follower 1]   │            │ [Follower 3]   │
  │ [Follower 2]   │            │ [Follower 4]   │
  └────────────────┘            └────────────────┘
```

Each data center has its own leader and followers.
**Leaders sync with each other** across data centers.

---

## Example: Global Collaboration App (Google Docs style)

Two users in different data centers edit a document simultaneously.

```
  US User edits ──► [Leader US] ──────────────────────────► [Leader EU]
                                  (async replication)            │
  EU User edits ──► [Leader EU] ──────────────────────────► [Leader US]
```

Both users get fast local writes.
Changes are asynchronously propagated to other data centers.

---

## Example with Multiple Data Centers

```
                         INTERNET
                            │
         ┌──────────────────┼──────────────────┐
         │                  │                  │
   [DC: US-East]      [DC: EU-West]      [DC: Asia]
   ┌───────────┐      ┌───────────┐      ┌───────────┐
   │ Leader-US │◄────►│ Leader-EU │◄────►│ Leader-AS │
   │           │      │           │      │           │
   │ Follower1 │      │ Follower3 │      │ Follower5 │
   │ Follower2 │      │ Follower4 │      │ Follower6 │
   └───────────┘      └───────────┘      └───────────┘

  US users   ──► Leader-US (low latency)
  EU users   ──► Leader-EU (low latency)
  Asia users ──► Leader-AS (low latency)
```

---

## Write Conflict Problem

The main challenge: **two leaders accept conflicting writes simultaneously**.

```
  User A: Set title = "Hello"  ──► [Leader US]
  User B: Set title = "World"  ──► [Leader EU]

  After replication:
  Leader US: title = "Hello" or "World" ??
  Leader EU: title = "Hello" or "World" ??
```

### Conflict Resolution Strategies

| Strategy                | Description                                        |
|-------------------------|----------------------------------------------------|
| Last Write Wins (LWW)   | Use timestamp; latest write survives               |
| Custom Merge Function   | App logic decides how to merge (like Git merge)   |
| Record Conflict         | Store both versions, let user resolve              |
| CRDT                    | Data structures that auto-merge without conflicts  |

---

## Multi-Leader Summary

| Aspect        | Detail                                              |
|---------------|-----------------------------------------------------|
| Writes        | Accepted by multiple leaders                        |
| Latency       | Low (local writes per region)                       |
| Availability  | Very high (each DC is independent)                  |
| Main Problem  | Write conflicts require resolution                  |
| Use Cases     | Multi-datacenter deployments, offline-first apps    |
| Examples      | MySQL NDB Cluster, CouchDB, Google Docs internal    |

---

# 3. Leaderless Replication

## Concept

**No designated leader.** Any node can accept reads and writes. The client sends requests to **multiple nodes simultaneously** and uses a **quorum** to determine the result.

```
              WRITE (send to all 3)
  [ Client ] ──────────────────────────►
               │          │         │
            [Node 1]   [Node 2]  [Node 3]
               │          │         │
             ACK         ACK       (slow)

  Client gets 2 ACKs → write is considered successful
```

---

## Quorum Mode (W + R > N)

This is the fundamental rule of leaderless replication.

```
  N = total number of replicas
  W = number of nodes that must confirm a WRITE
  R = number of nodes that must respond to a READ

  Rule: W + R > N  →  at least one node has latest value
```

### Example with N=3, W=2, R=2

```
  N = 3 nodes
  W = 2 (write to 2 nodes before confirming to client)
  R = 2 (read from 2 nodes, take latest value)

  W + R = 4 > N = 3  → CONSISTENT
```

```
  WRITE:
  Client ──►  Node 1 ──ACK
         ──►  Node 2 ──ACK
         ──►  Node 3 (failed / slow)

  Client gets 2 ACKs (W=2 satisfied) → success

  READ (later):
  Client ──►  Node 1 → returns v2 (latest)
         ──►  Node 2 → returns v2 (latest)
         ──►  Node 3 → returns v1 (stale)

  Client sees v2 from 2 nodes → correct answer
```

Because W+R > N, at least one node in the read set has the latest write.

---

## Real-World Example: Amazon DynamoDB / Cassandra Style

```
  Write: user_id=123, name="Alice Updated"
                │
         ┌──────┼──────┐
         ▼      ▼      ▼
       [N1]   [N2]   [N3]   (N=3, W=2, R=2)
       WRITE  WRITE  SLOW
       ACK    ACK
                │
         Client: "Write successful"

  Read: user_id=123
                │
         ┌──────┼──────┐
         ▼      ▼      ▼
       [N1]   [N2]   [N3]
      "Alice  "Alice  "Alice"
      Updated Updated  (old)

  R=2 → need 2 responses → N1 + N2 have latest → return "Alice Updated"
```

---

## Read Repair & Anti-Entropy

Leaderless systems have mechanisms to fix stale data:

1. **Read Repair:** When a read detects a stale node, it sends the latest value back to fix it.
2. **Anti-entropy process:** Background process constantly compares and syncs replicas.

---

## Sloppy Quorum & Hinted Handoff

If target nodes are down, writes go to temporary nodes (sloppy quorum).
When original nodes recover, data is handed off back (hinted handoff).

---

## Leaderless Summary

| Aspect        | Detail                                              |
|---------------|-----------------------------------------------------|
| Writes        | To any W out of N nodes                             |
| Reads         | From any R out of N nodes                           |
| Consistency   | Depends on W+R>N configuration                     |
| No leader     | No single point of failure                          |
| Trade-off     | May read stale data if W+R is less than N           |
| Examples      | Amazon DynamoDB, Apache Cassandra, Riak             |

---

## Replication Algorithms Comparison

| Feature              | Single-Leader       | Multi-Leader             | Leaderless          |
|----------------------|---------------------|--------------------------|---------------------|
| Write Target         | One leader only     | Any local leader         | Any node            |
| Read Scalability     | Good (followers)    | Good (local reads)       | Excellent           |
| Write Scalability    | Limited (1 leader)  | Good (per-region)        | Excellent           |
| Consistency          | Strong (sync)       | Eventual (conflicts)     | Configurable (W+R)  |
| Conflict Handling    | No conflicts        | Resolution needed        | Version vectors     |
| Latency              | Low-Medium          | Very Low (geo-local)     | Low                 |
| Complexity           | Low                 | Medium                   | High                |
| Best For             | SQL databases       | Multi-region writes      | Massive scale NoSQL |
