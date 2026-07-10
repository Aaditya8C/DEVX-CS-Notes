# Partitioning (Sharding)

## What is it?

Splitting data into smaller subsets (shards) stored on **different nodes**.

**Why?**
- Data too large for one machine
- Distribute write load (unlike replication, all writes don't go to one node)
- Queries only scan relevant shard, not everything

> Partitioning is always combined with replication for fault tolerance.

---

## Strategy 1: Partition by Key Range

Assign consecutive key ranges to partitions (like an encyclopedia).

```
  [ A-D ] → Node 1    [ E-L ] → Node 2
  [ M-R ] → Node 3    [ S-Z ] → Node 4
```

**Good for:** Range queries (`GET orders from Jan 1–31`)
**Problem: Hot spots** — if all writes have sequential keys (e.g., timestamps), one partition gets all traffic.

---

## Strategy 2: Partition by Hash of Key

Compute `hash(key) % N` to assign a partition.

```
  hash("alice") % 4 = 1  → Node 1
  hash("bob")   % 4 = 3  → Node 3
```

**Good for:** Even data distribution, no hot spots
**Problem:** Range queries broken — consecutive keys land on random nodes → must query all partitions (scatter-gather)

**Consistent Hashing:** When a node is added, only ~1/N data moves (vs rehashing everything).
Used by: Cassandra, DynamoDB

---

## Strategy 3: Partition by Secondary Index

**Primary index** = what you partition by (e.g., `user_id`)
**Secondary index** = additional field you want to search by (e.g., `color`, `city`)

### 3a. Local Secondary Index (Document-Partitioned)

Each partition maintains its own index for data it holds.

```
  Partition 1 (cars 1-100):   color:red → [car1, car5]
  Partition 2 (cars 101-200): color:red → [car110, car150]
```

**Problem:** Query "find all red cars" → must hit every partition (scatter-gather) — expensive.
**Used by:** Elasticsearch, MongoDB, Cassandra

### 3b. Global Secondary Index (Term-Partitioned)

One shared index across all partitions, but the index itself is partitioned by value.

```
  Index Partition A (a-r):  color:red  → [car1, car5, car110, car150]
  Index Partition B (s-z):  color:white → [car7, car88]
```

**Good for:** Fast targeted reads — just query the right index partition.
**Problem:** Writes are slower — must update both data partition and index partition (distributed write).
**Used by:** DynamoDB GSI, Elasticsearch (coordinating node)

---

## Comparison

| Strategy                | Range Queries  | Write Distribution | Used By                  |
|-------------------------|----------------|--------------------|--------------------------|
| Key Range               | Fast           | Hot spots possible | HBase, Bigtable          |
| Hash of Key             | Scatter-gather | Even               | Cassandra, DynamoDB      |
| Local Secondary Index   | Scatter-gather | Fast local writes  | Elasticsearch, MongoDB   |
| Global Secondary Index  | Targeted       | Slower (distributed write) | DynamoDB GSI      |

---

## Rebalancing

When nodes are added/removed, partitions are moved to the new node.

**Best practice:** Create more partitions than nodes upfront. On adding a node, move whole partitions — no rehashing needed.
