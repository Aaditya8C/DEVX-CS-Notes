# Faults and Failures in Distributed Systems

## Overview

In a distributed system, things **will go wrong**. This is not a question of *if* but *when*.

> "Everything that can go wrong, will go wrong." — Murphy's Law

The goal is to build systems that are **fault-tolerant**: they continue operating correctly even when some components fail.

```
  FAULT   ─► causes ─► ERROR ─► leads to ─► FAILURE

  Fault:   Defect in a component (disk sector corrupt)
  Error:   Component in wrong state (database can't read block)
  Failure: System stops providing its service (website down)
```

---

## Three Categories of Faults

```
  ┌─────────────────────────────────────────────────┐
  │               DISTRIBUTED SYSTEM                 │
  │                                                  │
  │  [ Hardware Faults ]  [ Software Faults ]        │
  │  (Servers, Disks,     (Bugs, Config,             │
  │   Networks)            Deploy errors)            │
  │                                                  │
  │  [ Human Faults ]                                │
  │  (Misconfig, Ops errors, Unpredictability)       │
  └─────────────────────────────────────────────────┘
```

---

# 1. Hardware Faults

Physical failures of infrastructure components.

---

## Server Failures

A server crashes completely — no more processing.

```
  BEFORE:
  [ Load Balancer ] ──► [ Server 1 ] (active)
                    ──► [ Server 2 ] (active)
                    ──► [ Server 3 ] (active)

  SERVER 1 CRASHES:
  [ Load Balancer ] ──► [ Server 1 ] (down — hardware failure)
                    ──► [ Server 2 ] (active)
                    ──► [ Server 3 ] (active)

  System continues with 2 servers (if load balancer detects failure)
```

**Causes:**
- Power supply failure
- Motherboard / CPU failure
- Overheating
- Memory failure (RAM corrupted)

**Mitigation:**
- Redundant servers (horizontal scaling)
- Heartbeat monitoring + auto-failover
- Auto-scaling groups (AWS ASG, GKE Node Pools)

---

## Database Server Failures

The database node crashes → data inaccessible.

```
  App → DB (Primary) ─── CRASHED ───► (down)

  Without replication:  Entire system down
  With replication:     Failover to Replica

  App → DB (Replica 1) ← becomes new primary
```

**Mitigation:**
- Replication (see Replication notes)
- Managed databases with auto-failover (AWS RDS Multi-AZ)
- Regular backups + Point-In-Time Recovery (PITR)

---

## Disk Failures

Disk corruption or complete drive failure — data lost.

```
  DISK FAILURE RATES:
  A typical hard drive has ~1% chance of failure per year.
  In a 10,000 server data center → 100 disk failures/year
  → Roughly 1-2 failures per day!

  Types:
  ├── Mechanical failure (HDD head crash)
  ├── Bit rot (data corrupts silently over time)
  ├── SSD wear-out (limited write cycles)
  └── Controller failure (disk unreadable)
```

**Impact:**
- Data loss (if no backup)
- Service interruption

**Mitigation:**
- **RAID** (Redundant Array of Independent Disks)
  - RAID-1: Mirror data across 2 disks
  - RAID-5/6: Distributed parity
- **Distributed file systems** (HDFS, GFS) — data replicated across nodes
- **Checksums** to detect silent corruption (ZFS, HDFS)
- Regular backup snapshots

```
  HDFS Replication Example:
  File Block A → Stored on Node1, Node3, Node7  (3 replicas)

  Node1 disk fails → Block A still accessible via Node3 and Node7
```

---

## Network Failures

Network links between nodes fail or become unreliable.

```
  NETWORK FAILURE TYPES:

  1. Complete Network Partition:
     [DC-US] ─────X──── [DC-EU]
     Nodes alive, but can't communicate

  2. Partial Loss:
     Some packets dropped, some delivered
     → Non-deterministic behavior!

  3. High Latency:
     Node looks "slow" — might be alive or might be dead
     → Timeout-based detection is hard

  4. Asymmetric Failure:
     Node A → Node B: working
     Node B → Node A: broken
     → A thinks B is dead; B thinks A is alive
```

**Mitigation:**
- Redundant network paths (dual NICs, multiple switches)
- Timeout + retry with exponential backoff
- Circuit breakers (fail fast instead of hanging)
- Heartbeats for liveness detection

```
  Exponential Backoff Retry:
  Attempt 1: retry after 1s
  Attempt 2: retry after 2s
  Attempt 3: retry after 4s
  Attempt 4: retry after 8s
  Cap at: 60s max
  → Prevents thundering herd on recovery
```

---

## Hardware Faults Summary

| Hardware Fault | Detection Method     | Mitigation                                    |
|----------------|----------------------|-----------------------------------------------|
| Server crash   | Heartbeat / health check | Load balancer failover, redundant servers |
| DB crash       | DB health probe      | Replication, auto-failover                    |
| Disk failure   | SMART monitoring     | RAID, distributed storage, backups            |
| Network split  | Timeout + ping       | Redundant links, partition tolerance          |

---

# 2. Software Faults

Software faults are **harder** to detect than hardware faults because they often lie dormant until triggered.

---

## Bad Code / Bugs

Logic errors that cause incorrect behavior or crashes.

```
  EXAMPLES:

  1. Off-by-one error:
     for i in range(0, len(arr)):   ← should be len(arr)
         process(arr[i+1])          ← IndexError when i = last index

  2. Integer overflow:
     max_users = 2^31 - 1 = 2,147,483,647
     system crosses 2.1B users → counter wraps to negative
     (Example: YouTube view counter bug on "Gangnam Style")

  3. Incorrect algorithm:
     Sorting algorithm has a bug → data served in wrong order
```

**Mitigation:**
- Code reviews and pair programming
- Unit tests, integration tests, end-to-end tests
- Static analysis / linters
- Fuzzing (testing with random/edge-case inputs)

---

## Unhandled Exceptions

Code doesn't handle error conditions → crash or undefined behavior.

```
  BAD CODE (no error handling):

  def get_user(user_id):
      result = db.query("SELECT * FROM users WHERE id=" + user_id)
      return result[0]  # crashes if result is empty!

  GOOD CODE (with error handling):

  def get_user(user_id):
      try:
          result = db.query("SELECT * FROM users WHERE id=?", [user_id])
          if not result:
              raise UserNotFoundException(f"User {user_id} not found")
          return result[0]
      except DatabaseException as e:
          logger.error(f"DB error: {e}")
          raise ServiceUnavailableException("Database temporarily unavailable")
```

**Mitigation:**
- Defensive programming
- Proper exception hierarchy
- Graceful degradation (return default/cached value)
- Detailed error logging

---

## Edge Cases

Rare input conditions that weren't considered during development.

```
  EXAMPLES:

  1. Empty input:
     User submits empty name → system stores "" → breaks display

  2. Very large input:
     Upload 10GB file to endpoint designed for 1MB → OOM crash

  3. Special characters:
     Name: "O'Brian" → SQL query breaks (SQL injection)
     Name: "<script>" → XSS attack

  4. Timezone edge cases:
     "Get all orders on Nov 5, 2023" — which timezone?
     DST changes can cause duplicate or skipped hours.

  5. Leap year:
     App assumes Feb always has 28 days → crashes on Feb 29
```

**Mitigation:**
- Boundary value testing
- Input validation and sanitization
- Property-based testing (test with random inputs)
- Load/stress testing

---

## Configuration Errors

Wrong settings cause incorrect or dangerous behavior.

```
  REAL-WORLD EXAMPLES:

  1. Wrong database connection string:
     DB_URL = "prod-db.example.com"  ← should be dev-db for testing
     → Testing against production database → data corruption

  2. Memory limits too low:
     JVM_HEAP = 256MB  ← app needs 2GB
     → OutOfMemoryError under normal load

  3. Timeout too short:
     HTTP_TIMEOUT = 100ms
     → Any query >100ms fails, even legitimate ones

  4. Wrong AWS region:
     S3_BUCKET_REGION = "us-east-1"  ← bucket is in eu-west-1
     → All file uploads fail
```

**Mitigation:**
- Environment-specific config files (dev, staging, prod)
- Config validation on startup
- Secret management (AWS Secrets Manager, Vault)
- Infrastructure as Code (Terraform, Ansible) for consistency
- Config change reviews

---

## Deployment Errors

New code deployment causes failures.

```
  DEPLOYMENT FAILURE SCENARIOS:

  1. Bad deploy reaches production:
     Deploy v2.0 → has a bug → crashes 50% of requests

  2. Partial deployment:
     v1.0 and v2.0 running simultaneously
     v2.0 changed API response format
     → v1.0 clients can't parse v2.0 responses

  3. Database migration goes wrong:
     ALTER TABLE drops wrong column
     → Data loss

  4. Dependency version conflict:
     New package version incompatible with existing code
```

**Mitigation:**
```
  Deployment Strategies:

  Blue-Green Deployment:
  [ Blue (v1.0) ] ← live traffic
  [ Green (v2.0) ] ← staging, being tested

  Switch: Route traffic to Green when ready
  Rollback: Route back to Blue if issues found

  Canary Release:
  [ v1.0 ] ← 95% of traffic
  [ v2.0 ] ← 5% of traffic (canary)

  Monitor canary → if healthy, gradually shift to 100%
  → Limits blast radius of a bad deploy
```

---

## Merge Request / Merge Conflicts

Code merges introduce subtle bugs.

```
  DEVELOPER A changes:
  function calculateTax(price) {
      return price * 0.10;  // 10% tax
  }

  DEVELOPER B changes (same function, different line):
  function calculateTax(price, discount) {
      return (price - discount) * 0.10;
  }

  MERGED CODE (wrong):
  function calculateTax(price) {
      return (price - discount) * 0.10;  // 'discount' undefined!
  }
```

**Mitigation:**
- Short-lived feature branches
- Automated CI tests on every PR
- Code review requirements (minimum 2 approvers)
- Feature flags (deploy code but disable feature until ready)

---

## Performance Issues (Software)

System becomes too slow due to inefficient code.

```
  N+1 QUERY PROBLEM:

  // Fetch all users
  users = db.query("SELECT * FROM users")   // 1 query

  for user in users:
      orders = db.query("SELECT * FROM orders WHERE user_id = ?", user.id)
      // 1 query PER USER → if 1000 users → 1001 queries!

  FIX: Use JOIN or batch fetch
  data = db.query("""
      SELECT u.*, o.* FROM users u
      JOIN orders o ON u.id = o.user_id
  """)  // 1 query

  MEMORY LEAK:
  Objects created but never garbage collected
  → Memory grows over time → Eventually OOM crash

  SOLUTION: Profiling, APM tools (Datadog, New Relic), load testing
```

---

## Software Faults Summary

| Software Fault      | Detection               | Mitigation                             |
|---------------------|-------------------------|----------------------------------------|
| Bad code/bugs       | Tests, code review      | TDD, static analysis, fuzzing          |
| Unhandled exceptions| Monitoring/alerts       | Defensive coding, error handling       |
| Edge cases          | QA testing              | Boundary testing, input validation     |
| Config errors       | Startup validation      | Config reviews, IaC, secret management |
| Deployment errors   | Monitoring post-deploy  | Blue-green, canary deployments         |
| Merge conflicts     | CI pipeline             | Short branches, automated tests        |
| Performance issues  | APM, profiling          | Query optimization, load testing       |

---

# 3. Human Faults

The most common and unpredictable source of failures.

> Studies show that **human error is responsible for the majority of outages** in large-scale systems.

---

## Unpredictability of Human Actions

Humans do things the system designers never anticipated.

```
  EXAMPLES:

  1. Accidental Data Deletion:
     Engineer runs:
     DELETE FROM users WHERE id = 123;  ← forgot WHERE clause
     → Actually:
     DELETE FROM users;  ← deletes ALL users!

  2. Wrong Environment:
     ssh prod-server  (thought it was dev-server)
     → Runs destructive test on production

  3. Miscommunication:
     Developer: "I'll take down server at 2am"
     Ops: "OK" (thought it was 2am their time, different timezone)
     → Server goes down during peak hours

  4. "Clever" manual fix:
     Operator manually edits config to fix a bug
     → Doesn't document it
     → Next deploy overwrites it → bug returns
```

---

## Mitigation Strategies for Human Faults

### Sandboxed Environments

```
  DEV → STAGING → PRODUCTION

  DEV: Developer experiments freely
       Any mistake only affects dev

  STAGING: Full production mirror
            Test before going live

  PRODUCTION: Real users
               Changes need approval
```

Never run untested operations directly on production.

---

### Rollback and Recovery

Design systems so any change can be **undone**.

```
  Database Migrations:

  migration_v1.sql:
    ALTER TABLE users ADD COLUMN age INT;  ← forward migration

  rollback_v1.sql:
    ALTER TABLE users DROP COLUMN age;     ← rollback migration

  Deploy code: Can rollback within minutes if issues found.
```

---

### Monitoring and Alerting

```
  MONITORING PYRAMID:

                    [ALERT]
                      ↑
              [METRICS DASHBOARD]
                      ↑
           [LOGS & TRACES (APM)]
                      ↑
           [SYSTEM HEALTH CHECKS]

  What to monitor:
  - Error rates (should be ~0%)
  - Latency (p50, p95, p99)
  - Request rate (sudden drop = something wrong)
  - Resource usage (CPU, Memory, Disk)
  - Business metrics (orders per minute, logins)
```

---

### Runbooks & Documentation

```
  WITHOUT RUNBOOK:
  Alert: "Database high CPU!"
  On-call engineer (3am): "Uh... what do I do?"

  WITH RUNBOOK:
  Alert: "Database high CPU!"
  On-call engineer: Opens runbook →
    Step 1: Check slow query log
    Step 2: Kill long-running queries
    Step 3: Escalate if > 90% CPU for 10 mins
  → Resolved in minutes
```

---

### Chaos Engineering

**Deliberately inject failures** in production to find weaknesses before they find you.

```
  Netflix Chaos Monkey:
  → Randomly terminates servers in production
  → Forces engineers to build resilient systems
  → If a random crash doesn't cause an outage → system is resilient

  Types of chaos:
  ├── Kill random instances (Chaos Monkey)
  ├── Slow down network (add latency)
  ├── Fill up disks
  ├── Drop network packets
  └── Kill entire availability zone
```

---

### Access Controls & Audit Logs

```
  PRINCIPLE OF LEAST PRIVILEGE:
  Developer: can read logs
  Developer: cannot delete production DB (blocked)
  DBA: can modify schema on staging
  DBA: needs approval to modify production schema

  AUDIT LOG:
  2024-01-15 03:42:11 | admin@company.com | DELETE | users table | REJECTED
  2024-01-15 03:42:15 | admin@company.com | SELECT | users table | SUCCESS

  "Who did what, when?" → crucial for post-mortem analysis
```

---

### Post-Mortems (Blameless)

After an incident, analyze what went wrong **without blaming individuals**.

```
  POST-MORTEM FORMAT:

  Incident: Production DB went down for 45 minutes
  Date: 2024-01-15, 3:00am-3:45am
  Impact: 5,000 users couldn't log in

  Timeline:
  3:00am - Deploy v2.3 started
  3:05am - DB CPU hit 100%
  3:10am - Alerts fired, on-call engineer paged
  3:20am - Root cause identified (N+1 query in new code)
  3:45am - Rollback to v2.2 complete

  Root Cause: N+1 query introduced in PR #4521

  What went wrong:
  - Code review didn't catch the N+1 query
  - No load test was run before deploy

  Action Items:
  - Add query count limit in CI pipeline
  - Add load testing to staging before deploy
  - Add DB query monitoring alert

  → Focus on SYSTEMS, not blaming the individual
```

---

## Fault Categories Summary

```
  ┌─────────────────────────────────────────────────────────────────┐
  │                    FAULTS IN DISTRIBUTED SYSTEMS                 │
  ├───────────────┬─────────────────────────┬───────────────────────┤
  │  HARDWARE     │  SOFTWARE               │  HUMAN                │
  ├───────────────┼─────────────────────────┼───────────────────────┤
  │ • Server      │ • Bad code / bugs       │ • Accidental deletion │
  │   crash       │ • Unhandled exceptions  │ • Wrong environment   │
  │               │                         │                       │
  │ • DB failure  │ • Edge cases            │ • Miscommunication    │
  │               │                         │                       │
  │ • Disk        │ • Config errors         │ • "Clever" manual     │
  │   failure     │                         │   fixes               │
  │               │ • Deploy errors         │                       │
  │ • Network     │                         │ • Timezone confusion  │
  │   partition   │ • Merge conflicts       │                       │
  │               │                         │                       │
  │               │ • Performance issues    │                       │
  ├───────────────┼─────────────────────────┼───────────────────────┤
  │ MITIGATION:   │ MITIGATION:             │ MITIGATION:           │
  │ Redundancy    │ Tests, code review,     │ Sandboxes, rollback,  │
  │ Replication   │ monitoring, canary      │ monitoring, runbooks, │
  │ Backups       │ deploys, feature flags  │ chaos engineering,    │
  │ Heartbeats    │                         │ blameless post-mortem │
  └───────────────┴─────────────────────────┴───────────────────────┘
```

---

## The Golden Rule of Fault Tolerance

> Assume **everything will fail**. Design your system to **handle failure gracefully**, not to prevent all failure.

```
  REACTIVE (bad): "We'll fix it when it breaks"
  PROACTIVE (good): "We assume it WILL break. How do we respond?"

  Level 1: Detect the fault quickly
  Level 2: Contain the impact (circuit breakers, bulkheads)
  Level 3: Recover automatically (auto-failover, retries)
  Level 4: Learn and improve (post-mortems, chaos testing)
```

---

## Key Takeaways

- **Hardware:** Servers, disks, and networks WILL fail → use redundancy and replication
- **Software:** Bugs, config errors, bad deploys → use testing, reviews, canary deploys
- **Human:** Most outages are caused by humans → use sandboxes, runbooks, post-mortems
- Build for **fault tolerance**, not fault prevention
- Monitor everything, alert on anomalies, automate recovery
- Chaos engineering proactively finds weaknesses before users do
