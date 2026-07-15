# Faults and Failures

## Fault vs Error vs Failure

```
  Fault → Error → Failure

  Fault:   Defect in a component (disk sector bad)
  Error:   Component in wrong state (DB can't read block)
  Failure: System stops serving users (website down)
```

> Goal: Build **fault-tolerant** systems — continue operating despite faults.

---

# 1. Hardware Faults

| Fault          | Causes                                       | Mitigation                                     |
|----------------|----------------------------------------------|------------------------------------------------|
| Server crash   | Power, CPU, RAM, overheating                 | Redundant servers, heartbeat + auto-failover   |
| DB failure     | Hardware crash                               | Replication, managed auto-failover (RDS Multi-AZ) |
| Disk failure   | ~1% failure/year — 100 failures/year per 10K servers | RAID, distributed storage (HDFS), checksums, backups |
| Network failure| Partition, packet loss, asymmetric failure   | Redundant NICs, exponential backoff retries, circuit breakers |

**Network failure types:**
- **Complete partition** — nodes alive but can't communicate
- **Packet loss** — non-deterministic, hard to detect
- **Asymmetric** — A→B works, B→A broken (dangerous!)
- **High latency** — node appears slow/dead; hard to distinguish

---

# 2. Software Faults

| Fault               | Example                                              | Mitigation                              |
|---------------------|------------------------------------------------------|-----------------------------------------|
| Bad code / bugs     | Off-by-one, integer overflow (YouTube Gangnam Style) | Code review, unit/integration tests, fuzzing |
| Unhandled exceptions| `result[0]` on empty query → crash                  | Defensive coding, try/catch, graceful degradation |
| Edge cases          | Empty string, 10GB upload, `O'Brian` SQL injection, leap year | Input validation, boundary testing   |
| Config errors       | Wrong DB URL, too-low memory limit, short timeout    | Config validation on startup, IaC, env separation |
| Deployment errors   | Bad deploy, partial rollout, DB migration drops column | Blue-green deploy, canary releases, rollback plan |
| Merge conflicts     | Merged function references undefined variable        | Short-lived branches, CI tests on every PR, feature flags |
| Performance issues  | N+1 queries, memory leaks                            | Query optimization, profiling, APM tools |

**Deployment Strategies:**
```
  Blue-Green:  swap live/staging in one step → instant rollback
  Canary:      5% of traffic gets v2 → monitor → gradually increase
```

---

# 3. Human Faults

Most outages are caused by human error.

| Fault               | Example                                           | Mitigation                             |
|---------------------|---------------------------------------------------|----------------------------------------|
| Accidental deletion | `DELETE FROM users` without WHERE clause          | Sandboxed envs, access controls        |
| Wrong environment   | SSH into prod thinking it's dev                   | Clear env labels, 2-factor for prod    |
| Miscommunication    | "Down at 2am" — different timezones               | Written runbooks, change management    |
| Undocumented fix    | Manual config edit, next deploy overwrites it     | Infrastructure as Code (IaC), audit logs |

**Mitigations:**
- **Sandboxed environments** — DEV → STAGING → PROD pipeline
- **Rollback** — every deploy and migration must have an undo path
- **Access controls** — least privilege; only DBA can touch prod schema
- **Audit logs** — who did what, when
- **Runbooks** — step-by-step guides for common incident responses
- **Chaos Engineering** — Netflix Chaos Monkey: deliberately kill nodes in production to prove resilience
- **Blameless post-mortems** — after incidents, focus on fixing systems, not blaming people

---

## The Golden Rule

> Assume **everything will fail**. Design to **handle failure gracefully**, not prevent it entirely.

```
  Level 1: Detect fast
  Level 2: Contain impact (circuit breakers, bulkheads)
  Level 3: Recover automatically (failover, retries)
  Level 4: Learn and improve (post-mortems, chaos tests)
```
