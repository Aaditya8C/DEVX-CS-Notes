# Partitioning (Sharding) in Distributed Systems

## What is Partitioning?

**Partitioning** (also called **Sharding**) means **splitting your data into smaller subsets** (partitions/shards), and storing each subset on a **different node**.

```
WITHOUT PARTITIONING (All data on one node):

  [ Node 1 ] ── All 100GB of data
                 Getting overloaded

WITH PARTITIONING:

  [ Node 1 ] ── Users A-M  (33GB)
  [ Node 2 ] ── Users N-Z  (33GB)
  [ Node 3 ] ── Users by ID range (33GB)
```

Each piece of data belongs to **exactly one partition**.

---

## Why Partitioning?

### 1. Handle Data That Doesn't Fit on One Machine

Single disk: max 20TB → Dataset: 200TB → Need 10 machines minimum

### 2. Query Throughput Scaling

```
  1 Node:   10,000 queries/sec → OVERLOADED
  10 Nodes: 10,000 queries/sec → each handles 1,000 → manageable
```

### 3. Write Scalability

Unlike replication (where all writes go to one leader), partitioning distributes writes too.

### 4. Reduce Latency

Queries only scan relevant partition, not the entire dataset.

---

## Partitioning is Usually Combined with Replication

Each partition has its own replicas for fault tolerance.

```
  Partition A: [ Leader ] → [ Replica 1 ] → [ Replica 2 ]
  Partition B: [ Leader ] → [ Replica 1 ] → [ Replica 2 ]
  Partition C: [ Leader ] → [ Replica 1 ] → [ Replica 2 ]
```

---

# Partitioning Strategies

## Strategy 1: Partition by Key Range

### How It Works

Data is sorted by key. Consecutive key ranges are assigned to partitions.

```
  Partition 1: Keys A ─── M
  Partition 2: Keys N ─── Z

  Or by date:
  Partition 1: Jan 2024 – Apr 2024
  Partition 2: May 2024 – Aug 2024
  Partition 3: Sep 2024 – Dec 2024
```

### Example: Encyclopedia / Time-Series Data

```
  Encyclopedia:
  [ A-D ] → Node 1  (Apple, Banana, Cat, Dog)
  [ E-L ] → Node 2  (Elephant, Fish, Google, Lion)
  [ M-R ] → Node 3  (Mango, Node, Python, Rose)
  [ S-Z ] → Node 4  (SQL, Tiger, Unix, Zoo)

  Query: "Find all entries starting with 'D'"
  → directly goes to Node 1 (no need to scan other nodes)
```

### Advantage: Range Queries Work Well

```
  "Get all orders from Jan 1 to Jan 31"
  → Only hits the January partition
  → Much faster than scanning all data
```

### Problem: Hot Spots (Uneven Load / Skewed Data)

```
  PROBLEM 1: Skewed Distribution

  Partition A: Letters A-D → 5,000 records
  Partition B: Letters E-L → 45,000 records  ← HOT NODE!
  Partition C: Letters M-Z → 50,000 records  ← HOT NODE!

  One node gets overloaded because data is not evenly spread.

  PROBLEM 2: Sequential Write Hot Spot

  If key = timestamp, all writes go to "latest" partition!
  Jan partition: 0 writes (past)
  Dec partition: ALL writes (everything writes to current month)
```

---

## Strategy 2: Partition by Hash of Key

### How It Works

Instead of using the raw key, compute a **hash of the key** and assign to a partition based on the hash value.

```
  hash("user_123")  = 47382  → Partition 2
  hash("user_456")  = 91023  → Partition 4
  hash("user_789")  = 12847  → Partition 1
```

### Example: User Database

```
  Total partitions: 4
  Rule: partition = hash(user_id) % 4

  hash("alice")   % 4 = 1  → Node 1
  hash("bob")     % 4 = 3  → Node 3
  hash("charlie") % 4 = 0  → Node 0
  hash("dave")    % 4 = 2  → Node 2

  Even if users are all named A-D,
  they get spread evenly across nodes
```

```
  [ Node 0 ] ── charlie, frank, ...
  [ Node 1 ] ── alice, henry, ...
  [ Node 2 ] ── dave, ivan, ...
  [ Node 3 ] ── bob, jane, ...

  Evenly distributed
```

### Advantage: Even Data Distribution

Hash functions spread data pseudo-randomly → no hot spots.

### Problem: Range Queries Don't Work

```
  "Get all users with ID between 100 and 200"

  hash(100) → Node 3
  hash(101) → Node 1
  hash(102) → Node 0
  hash(103) → Node 2
  ...

  Every key might be on a different node!
  Must query ALL partitions (scatter-gather) → slow
```

### Consistent Hashing (Advanced)

A technique where adding/removing nodes doesn't require re-hashing all keys — only a fraction of data moves.

```
  Normal hashing:    Add 1 node → rehash EVERYTHING
  Consistent hashing: Add 1 node → only ~1/N data moves
```

Used by: Cassandra, DynamoDB, Amazon's Dynamo

---

## Strategy 3: Partition by Secondary Index

### Background: Primary vs Secondary Index

```
  Primary Index: The main key you partition by (e.g., user_id)
  Secondary Index: An additional field you search by (e.g., color, location, age)
```

Secondary indexes are tricky with partitioning because the data lives on a different partition than the index.

---

### 3a. Local Secondary Index (Document-Partitioned)

Each partition maintains its OWN secondary index for the data it holds.

```
  Partition 1: Contains cars with ID 1-100
    Secondary Index (color):
      red:  [car1, car5, car20]
      blue: [car8, car15]

  Partition 2: Contains cars with ID 101-200
    Secondary Index (color):
      red:  [car110, car150]
      blue: [car125, car199]
```

### Example: Search Across Partitions

```
  Query: "Find all RED cars"

  → Must query ALL partitions! (scatter-gather)
  → Combine results from Partition 1 and Partition 2
  → Then filter for red cars

  Query   ──► Partition 1 (red: car1, car5, car20)
          ──► Partition 2 (red: car110, car150)
          ──► Partition 3 (red: car210, car280)
          Merge all results ← expensive
```

**Problem:** Scatter-gather queries are slow (must hit every partition).

**Used by:**
- **Elasticsearch** (local shard index)
- **MongoDB** (local index per shard)
- **Cassandra** (local secondary indexes)

---

### 3b. Global Secondary Index (Term-Partitioned)

Create a **separate global index** that covers data from ALL partitions, but the global index itself is partitioned.

```
  Global Index (partitioned by term/value):

  Index Partition A: (for values a-r)
    color:red  → [car1, car5, car20, car110, car150, car210]
    color:blue → [car8, car15, car125, car199]

  Index Partition B: (for values s-z)
    color:silver → [car3, car90, car140]
    color:white  → [car7, car88, car199]

  Data Partitions (separate):
  Data Partition 1: car1, car2, ... car100
  Data Partition 2: car101, ... car200
```

### Example Query with Global Index

```
  Query: "Find all RED cars"

  Step 1: Go to Index Partition A (handles 'red')
          → Index tells you: red cars are [car1, car5, car20, car110, car150]

  Step 2: Fetch each car:
          car1   → Data Partition 1
          car5   → Data Partition 1
          car110 → Data Partition 2

  Result: Faster targeted lookup (vs scatter-gather)
```

**But writes are slower:** Writing a new car requires:
1. Update data partition
2. Update global index partition (possibly on a different node)
→ **Distributed write** = more coordination needed

**Used by:**
- **DynamoDB** (Global Secondary Indexes - GSI)
- **Elasticsearch** (global cross-shard aggregations via coordinating node)

---

## Partitioning Strategies Comparison

| Strategy                   | Range Queries  | Write Distribution  | Used By                       |
|----------------------------|----------------|---------------------|-------------------------------|
| Key Range                  | Fast           | Hot spots           | HBase, Bigtable               |
| Hash of Key                | Scatter-gather | Even                | Cassandra, DynamoDB           |
| Local Secondary Index      | Scatter-gather | Local writes        | Elasticsearch, MongoDB        |
| Global Secondary Index     | Targeted       | Distributed         | DynamoDB GSI                  |

---

## Rebalancing Partitions

When nodes are added/removed, data must be redistributed:

```
  BEFORE (3 nodes):
  Node 1: Partition A, B
  Node 2: Partition C, D
  Node 3: Partition E, F

  ADD Node 4:
  Node 1: Partition A
  Node 2: Partition B, C
  Node 3: Partition D, E
  Node 4: Partition F    ← new node gets some partitions
```

**Fixed number of partitions:** Create more partitions than nodes upfront. When nodes are added, move whole partitions.

---

## Key Takeaways

- Partitioning splits data across multiple nodes for scalability
- Key-Range: good for range queries, bad for hot spots
- Hash: good for even distribution, bad for range queries
- Local secondary index: fast local writes, slow scatter-gather reads
- Global secondary index: fast targeted reads, slow distributed writes
- Always combine with replication for fault tolerance
