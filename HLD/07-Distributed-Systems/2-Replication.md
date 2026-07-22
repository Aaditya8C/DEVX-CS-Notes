# Replication

## What is it?

Keeping **copies of the same data on multiple nodes** (replicas).

**Why?**
- High availability — if one node fails, another takes over
- Read scalability — multiple nodes can serve reads
- Fault tolerance — survives hardware/network failures
- Low latency — place replicas near users geographically

---

## 1. Single-Leader Replication

**All writes** go to one leader. Leader replicates to followers. Reads can come from any node.

```
  [ Client ] ──WRITE──► [ Leader ]
                             │
                    ┌────────┴────────┐
                    ▼                 ▼
               [ Follower 1 ]   [ Follower 2 ]
               (READ)           (READ)
```

### Sync vs Async

| Mode          | How it works                            | Pros              | Cons                       |
|---------------|-----------------------------------------|-------------------|----------------------------|
| Synchronous   | Leader waits for follower ACK           | No data loss      | Slower, blocked if follower is down |
| Asynchronous  | Leader confirms immediately, replicates later | Fast        | Replication lag, data loss if leader crashes |
| Semi-sync     | One follower sync, rest async           | Balance of both   | —                          |

### Failover
1. Detect leader failure (heartbeat timeout)
2. Elect most up-to-date follower as new leader
3. Redirect all writes to new leader

**Risks:** Split-brain (two nodes think they are leader), data loss from async lag.

---

## 2. Multi-Leader Replication

Multiple nodes accept writes. Leaders sync with each other.

```
  [DC: US]                    [DC: EU]
  ┌──────────────┐            ┌──────────────┐
  │  Leader-US   │ ◄────────► │  Leader-EU   │
  │  Follower 1  │            │  Follower 3  │
  │  Follower 2  │            │  Follower 4  │
  └──────────────┘            └──────────────┘
```

**Use case:** Multi-datacenter — each DC writes locally for low latency.

**Main problem: Write Conflicts** — two leaders accept conflicting writes for the same record.

**Conflict Resolution:**
- Last Write Wins (LWW) — use timestamp
- Custom merge function — app decides
- Record conflict — let user resolve
- CRDT — data structures that auto-merge

---

## 3. Leaderless Replication

No leader. Client writes to **multiple nodes simultaneously**. Uses **quorum** to decide success.

```
  [ Client ] ──WRITE──► Node 1 (ACK)
             ──WRITE──► Node 2 (ACK)
             ──WRITE──► Node 3 (slow)

  Got 2 ACKs → write success (W=2)
```

### Quorum Rule: W + R > N

```
  N = total replicas
  W = writes required for success
  R = reads required for consensus

  N=3, W=2, R=2 → W+R=4 > 3 → CONSISTENT
```

At least one node in the read set always has the latest write.

**Stale data repair:**
- **Read Repair** — detects stale node during read, sends latest value back
- **Anti-entropy** — background process continuously syncs replicas

---

## Comparison

| Feature           | Single-Leader    | Multi-Leader       | Leaderless        |
|-------------------|------------------|--------------------|-------------------|
| Writes go to      | One leader       | Any local leader   | Any W nodes       |
| Write conflicts   | None             | Require resolution | Version vectors   |
| Consistency       | Strong (sync)    | Eventual           | Configurable (W+R)|
| Complexity        | Low              | Medium             | High              |
| Best for          | SQL databases    | Multi-region       | Massive NoSQL     |
