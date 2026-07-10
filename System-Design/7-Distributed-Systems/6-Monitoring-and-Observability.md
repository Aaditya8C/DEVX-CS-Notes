# Monitoring and Observability

## What is Monitoring?

**Monitoring** is the practice of **continuously collecting, tracking, and alerting on metrics** from your system so you know when something is going wrong — ideally before users notice.

```
  System produces data
        │
        ▼
  [ Metrics / Logs / Traces ]
        │
        ▼
  [ Monitoring Platform ]  (Prometheus, Datadog, Grafana)
        │
        ▼
  [ Dashboards + Alerts ]
        │
        ▼
  [ On-call Engineer gets paged ]
```

---

## What is Observability?

**Observability** is the ability to understand the **internal state of a system** purely from its external outputs (metrics, logs, traces).

A system is "observable" if you can answer:
- Why is this API slow?
- Which service caused this error?
- What happened at 3am last Tuesday?

```
  MONITORING       OBSERVABILITY
  "Is it broken?"  "Why is it broken?"

  Monitoring = knowing THAT something is wrong
  Observability = understanding WHY it's wrong
```

---

## Three Pillars of Observability

```
  ┌─────────────────────────────────────────────────┐
  │                OBSERVABILITY                     │
  │                                                  │
  │  [ METRICS ]    [ LOGS ]    [ TRACES ]           │
  │                                                  │
  │  Numbers over  Text events  Request flow         │
  │  time          with context across services      │
  │                                                  │
  │  "CPU is 90%"  "Error:      "Request took 300ms" │
  │               DB timeout"   "Slow at payments"   │
  └─────────────────────────────────────────────────┘
```

---

## Why is Monitoring Important?

### 1. Detect Problems Before Users Do

```
  WITHOUT MONITORING:
  User: "Your app is down!"
  Engineer: scrambles to figure out what happened

  WITH MONITORING:
  Alert fires: "Error rate > 5% for 2 minutes"
  Engineer: investigates and fixes before most users notice
```

### 2. Meet SLAs (Service Level Agreements)

SLA: "We guarantee 99.9% uptime" = max 8.7 hours downtime/year

Monitoring tracks whether you are meeting your SLA targets.

### 3. Capacity Planning

```
  CPU trend shows 10% growth per month
  At this rate → 100% in 10 months
  → Plan hardware upgrade in 8 months
```

### 4. Root Cause Analysis

After an incident: "Why did the DB CPU spike at 3am?"
→ Metrics show a batch job ran at 2:58am → caused the spike.

### 5. Performance Optimization

Find which API endpoints are slowest → prioritize optimization work.

---

# API Monitoring

## What to Monitor for APIs

```
  ┌─────────────────────────────────────────┐
  │              API MONITORING              │
  │                                          │
  │  Throughput  │  Error Codes  │  Latency  │
  │              │               │           │
  │  Health Check│               │  Alerts   │
  └─────────────────────────────────────────┘
```

---

## 1. Throughput

**Throughput** = number of requests your API handles per unit time.

Measured as: **Requests Per Second (RPS)** or **Requests Per Minute (RPM)**

```
  EXAMPLE:

  Time      |  RPS
  ──────────|──────
  12:00pm   |  5,000
  12:05pm   |  5,200
  12:10pm   |  4,900
  12:15pm   |  500     ← sudden drop! Something is wrong
  12:20pm   |  5,100
```

**Why it matters:**
- A sudden drop in RPS might mean the service is down or rejecting traffic
- A sudden spike might indicate a traffic surge (sale, viral post) — system might need to scale up

**Dashboard:**
```
  Throughput Graph (last 24h):

  5,000 |    ∧    ∧      ∧
  4,000 |   / \  / \    / \
  3,000 |  /   \/   \  /   \
  2,000 | /          \/
  1,000 |
      0 |──────────────────────
         12am  6am  12pm  6pm
```

---

## 2. Error Codes (Error Rate)

**Error Rate** = percentage of requests that return error responses.

```
  HTTP Status Codes:

  2xx → Success
    200 OK
    201 Created
    204 No Content

  4xx → Client Errors (bad request from user)
    400 Bad Request
    401 Unauthorized
    403 Forbidden
    404 Not Found
    429 Too Many Requests (rate limit hit)

  5xx → Server Errors (our fault)
    500 Internal Server Error
    502 Bad Gateway
    503 Service Unavailable
    504 Gateway Timeout
```

**What to track:**

```
  Error Rate = (5xx errors / total requests) * 100

  Example:
  Total requests:  10,000/min
  5xx errors:         50/min
  Error Rate:        0.5%

  Alert threshold: "Alert if error rate > 1% for 5 minutes"
```

**Error Rate by Endpoint:**
```
  Endpoint             | Error Rate
  ─────────────────────|───────────
  GET  /api/users      | 0.1%
  POST /api/orders     | 3.2%   ← investigate this!
  GET  /api/products   | 0.0%
  PUT  /api/payments   | 0.8%
```

This tells you exactly which endpoint is problematic.

---

## 3. Health Checks

A **health check** is a mechanism to determine whether a service is alive and able to serve traffic.

### Passive Health Check

The system **observes traffic** to detect failures. No dedicated probe is sent.

```
  HOW IT WORKS:

  [ Load Balancer ] ──► [ Server 1 ] ← watching responses
                    ──► [ Server 2 ] ← watching responses
                    ──► [ Server 3 ] ← watching responses

  If Server 1 returns 5 consecutive errors → marked as UNHEALTHY
  Load balancer stops sending traffic to Server 1

  Recovery: If Server 1 returns successful responses again
            → marked HEALTHY → traffic resumes
```

**Pros:** No extra network traffic
**Cons:** Relies on real user traffic to detect failures. If no traffic → can't detect failure.

---

### Active Health Check

The monitoring system **proactively sends test requests** (probes) to each service at regular intervals.

```
  HOW IT WORKS:

  [ Health Checker ] ──ping every 10s──► [ Server 1 ] → "200 OK" (healthy)
                    ──ping every 10s──► [ Server 2 ] → "200 OK" (healthy)
                    ──ping every 10s──► [ Server 3 ] → timeout  (unhealthy!)

  Server 3 marked as unhealthy → removed from load balancer pool
  Alert fired → on-call engineer paged
```

**Typical health check endpoint:**
```
  GET /health
  Response 200 OK:
  {
    "status": "healthy",
    "db": "connected",
    "cache": "connected",
    "version": "2.1.4"
  }

  Response 503:
  {
    "status": "unhealthy",
    "db": "disconnected",   ← tells you exactly what failed
    "cache": "connected"
  }
```

**Pros:** Detects failures even with zero traffic
**Cons:** Extra network overhead; false positives possible

---

### Passive vs Active Comparison

| Aspect          | Passive                        | Active                          |
|-----------------|--------------------------------|---------------------------------|
| How it works    | Observes real traffic          | Sends test probes               |
| Detection speed | Only when traffic exists       | Continuous, even with no traffic |
| Extra overhead  | None                           | Small (probe requests)          |
| False positives | Rare                           | Possible (network blip)         |
| Best for        | High-traffic services          | All services, especially idle   |

---

## 4. Latency

**Latency** = time taken to complete a request (from client sends request to client receives response).

```
  Client sends request ──► Server processes ──► Client gets response
       │                                              │
       └──────────────── LATENCY ────────────────────┘
```

### Why Average Latency is Misleading

```
  Request latencies for 10 requests (in ms):
  10, 12, 11, 9, 13, 10, 11, 12, 500, 10

  Average = (10+12+11+9+13+10+11+12+500+10) / 10 = 59.8ms

  Average says "59.8ms" but 9 out of 10 users got ~11ms response.
  One user got 500ms — but average hides this!
```

**Solution: Use Percentiles**

---

### Percentile Latency (P50, P70, P90, P95, P99)

A **percentile** tells you: "X% of requests completed in Y ms or less."

```
  Example dataset: 1000 requests, sorted by latency:
  
  P50 (50th percentile) = 45ms
  → 50% of requests completed in 45ms or less
  → This is the "typical" user experience (the median)

  P70 (70th percentile) = 80ms
  → 70% of requests completed in 80ms or less

  P90 (90th percentile) = 150ms
  → 90% of requests completed in 150ms or less
  → 10% of users wait MORE than 150ms

  P95 (95th percentile) = 280ms
  → 5% of users wait more than 280ms

  P99 (99th percentile) = 800ms
  → 1% of users wait more than 800ms (worst-case)
  → Called "tail latency"
```

**Visualising:**
```
  Latency Distribution:

  Requests
     |
  400|  ###
  300| #####
  200|#######
  100|#########  ##
    0|──────────────────────► latency (ms)
      10  50  100 200 500 1000

  P50 = 50ms   (most users — peak of the curve)
  P90 = 200ms  (right side of main cluster)
  P99 = 800ms  (the far-right outliers — "long tail")
```

---

### Real-World Example: E-Commerce Checkout API

```
  Metric    | Value   | Meaning
  ──────────|─────────|──────────────────────────────────────────
  P50       | 120ms   | Typical user checks out in 120ms
  P70       | 180ms   | 70% of users are served in 180ms or less
  P90       | 350ms   | 10% of users wait more than 350ms
  P95       | 600ms   | 5% of users wait more than 600ms
  P99       | 2100ms  | 1% of users wait over 2 seconds
  P99.9     | 8000ms  | 0.1% of users wait 8+ seconds (timeout risk!)

  SLA Target: P99 < 1000ms
  Current P99: 2100ms → SLA BREACH → alert fires!
```

**Why tail latency matters:**

```
  At Google/Amazon scale:
  1 billion requests/day
  P99 = 1% affected = 10 million users/day seeing slow responses!

  If your service calls 5 downstream services:
  Probability at least one call is slow (P99):
  = 1 - (0.99)^5 = ~5%
  → 5 in 100 users experience a slow request
```

---

### Latency SLOs and Alerts

**SLO (Service Level Objective):** An internal target you set to maintain quality.

```
  Example SLOs:
  - P50 latency < 100ms
  - P90 latency < 300ms
  - P99 latency < 1000ms
  - Error rate < 0.5%

  Alert rules:
  - WARN:  P90 > 250ms for 5 minutes
  - ALERT: P99 > 800ms for 2 minutes  → page on-call
  - ALERT: Error rate > 1% for 3 minutes → page on-call
```

**Alert setup example (Prometheus/Grafana):**
```
  Alert: HighP99Latency
  Condition: http_request_duration_p99 > 1.0 (seconds)
  For: 2 minutes
  Severity: critical
  Message: "P99 latency on /api/checkout exceeded 1s"
  → Pages on-call engineer via PagerDuty
```

---

## API Monitoring Summary

| Metric       | What It Measures               | Alert Threshold Example         |
|--------------|--------------------------------|---------------------------------|
| Throughput   | Requests per second            | Drop >30% in 5 min              |
| Error Rate   | % of 5xx responses             | >1% for 5 min                   |
| Health Check | Is service alive?              | 3 consecutive failures          |
| P50 Latency  | Typical (median) user latency  | >200ms for 10 min               |
| P90 Latency  | 90% of users served within     | >500ms for 5 min                |
| P99 Latency  | Worst-case (tail latency)      | >1000ms for 2 min (page alert)  |

---

# Machine / Infrastructure Monitoring

Beyond API-level metrics, you must also monitor the underlying machines running your services.

```
  API MONITORING:   "Is the application behaving correctly?"
  MACHINE MONITORING: "Is the infrastructure healthy enough?"

  Often correlated:
  High CPU → slow API responses → high P99 latency
```

---

## 1. CPU Usage

**CPU Usage** = percentage of time the processor is busy executing instructions.

```
  CPU USAGE LEVELS:

  0% - 50%   Normal      Service is healthy
  50% - 70%  Moderate    Monitor closely
  70% - 85%  High        Investigate — may need scaling
  85% - 95%  Very High   Performance degrading
  95%+       Critical    Requests dropping, slowdown severe
```

**What causes high CPU?**
```
  Common Causes:
  ├── Expensive database queries (no index, full table scan)
  ├── CPU-intensive computation (image processing, crypto)
  ├── Infinite loop or runaway process
  ├── Traffic spike (too many requests)
  └── N+1 query problems in application code
```

**Dashboard view:**
```
  CPU Usage (last 1 hour):

  100% |                    ████
   80% |              ██████████
   60% |         ██████████████
   40% |    █████████████████████
   20% |████████████████████████
    0% |────────────────────────
        12:00  12:15  12:30  12:45

  Alert: CPU > 85% for > 5 minutes → page on-call
```

**Mitigation:**
- Horizontal scaling (add more servers)
- Optimize hot code paths
- Move CPU-intensive work to background jobs
- Cache expensive computations

---

## 2. Memory (RAM) Usage

**Memory Usage** = amount of RAM currently in use by the system.

```
  MEMORY STATES:

  [ USED ] ← application data, caches
  [ BUFFERS/CACHE ] ← OS-managed caching (can be freed)
  [ FREE ] ← immediately available

  Key metric: Used memory as % of total RAM
```

**What causes high memory?**
```
  ├── Memory leak: objects allocated but never freed
  │   → Memory grows continuously over hours/days
  │   → Eventually crashes (OOM — Out Of Memory)
  │
  ├── Large in-memory cache
  │   → Intentional, but needs to be bounded
  │
  ├── Too many concurrent connections
  │   Each connection uses RAM → too many = OOM
  │
  └── Large payload processing
      Reading entire file into memory
```

**Memory Leak Pattern:**
```
  Memory over time with a leak:

  100% |                              ████ CRASH
   80% |                         █████
   60% |                    █████
   40% |               █████
   20% |          █████
    0% |█████─────────────────────────────
        Day 1   Day 2   Day 3   Day 4  Day 5

  The steady upward climb = memory leak
  Alert: Memory > 80% → investigate immediately
```

**Mitigation:**
- Profile memory usage to find leaks
- Set memory limits per container/pod
- Add horizontal scaling when memory is consistently high
- Use streaming instead of loading entire files into RAM

---

## 3. Disk I/O

**Disk I/O** = the rate at which data is being read from and written to disk.

```
  TWO KEY METRICS:

  Read throughput:  MB/s being read from disk
  Write throughput: MB/s being written to disk

  Also:
  IOPS = Input/Output Operations Per Second
  → How many separate read/write operations per second

  Disk Utilization %:
  → How busy the disk is (like CPU for storage)
```

**What causes high Disk I/O?**
```
  High READS:
  ├── Cache miss → reading from disk instead of memory
  ├── Full table scans in database (no index)
  └── Large file downloads / data exports

  High WRITES:
  ├── High write traffic (many inserts/updates)
  ├── Logging too verbosely (every request logged to disk)
  └── Database write-ahead log (WAL) under high load
```

**Disk I/O Dashboard:**
```
  Disk Write Throughput (MB/s):

  200 |              ██
  150 |         █████████
  100 |    ██████████████
   50 |████████████████████
    0 |──────────────────────
       12:00  12:15  12:30  12:45

  Disk Latency (ms):
  High disk latency → queries slow → API latency goes up
```

**Alert example:**
```
  Alert: "Disk utilization > 90% for 5 minutes"
  → Could mean disk is near capacity or heavily loaded
  → Investigate: which process is consuming disk I/O?
```

**Mitigation:**
- Add indexes to avoid full table scans
- Use SSDs for databases (much higher IOPS than HDDs)
- Move logs to a dedicated log volume
- Increase read cache (more RAM = more OS page cache)
- Partition data across multiple disks (RAID, sharding)

---

## 4. Network (Bandwidth & Packet Loss)

**Network Monitoring** covers both the volume of data and the quality of the connection.

```
  KEY NETWORK METRICS:

  Inbound traffic (ingress):  MB/s coming into the server
  Outbound traffic (egress):  MB/s going out of the server
  Packet loss:                % of network packets dropped
  Network latency:            ms for packets to travel between nodes
  TCP retransmits:            count of packets that had to be resent
```

**What to watch:**

```
  Bandwidth saturation:
  Server's NIC: 1 Gbps capacity
  Current usage: 950 Mbps → 95% saturated!
  → Packets start dropping → requests time out

  Packet Loss:
  0%:    No loss — ideal
  0.1%:  Noticeable degradation in latency
  1%+:   Serious — TCP retransmits cause major slowdown
  5%+:   TCP connections failing, services appearing down
```

**Network Dashboard:**
```
  Inbound Traffic (MB/s):

  800 |             ████████████
  600 |        ████████████████████
  400 |   ██████████████████████████
  200 |████████████████████████████████
    0 |──────────────────────────────
       12:00  12:10  12:20  12:30

  Packet Loss (%):
   0.5%|                   ██
   0.2%|              ████████
   0.0%|██████████████──────────
```

**Inter-service network latency:**
```
  Service A → Service B:  2ms  (same data center, normal)
  Service A → Service C:  180ms (cross-region, expected)

  If Service A → Service B suddenly jumps to 50ms:
  → Network congestion or misconfigured routing → alert!
```

**Mitigation:**
- Monitor and alert on bandwidth utilization (alert at 80%)
- Use CDN to offload static content and reduce egress
- Optimize payload size (compression, pagination)
- Monitor inter-service latency to catch network issues early

---

## Machine Monitoring Summary

| Metric           | Normal Range    | Warning Threshold  | Critical Threshold | Mitigation              |
|------------------|-----------------|--------------------|--------------------|-------------------------|
| CPU Usage        | 0 - 50%         | > 70% for 5 min    | > 90% for 2 min    | Scale out, optimize     |
| Memory Usage     | 0 - 70%         | > 80% for 10 min   | > 90%              | Fix leak, scale out     |
| Disk I/O Util.   | 0 - 60%         | > 80% for 5 min    | > 95%              | Index, SSD, partition   |
| Network Util.    | 0 - 60%         | > 80% of bandwidth | > 90%              | CDN, compress, scale    |
| Packet Loss      | 0%              | > 0.1%             | > 1%               | Investigate network     |
| Disk Free Space  | > 30% free      | < 20% free         | < 10% free         | Add disk, clean logs    |

---

## How API and Machine Metrics Correlate

Understanding the relationship helps diagnose root causes faster.

```
  SCENARIO: P99 latency spikes at 3pm

  Step 1: API Metrics show latency spike at 3pm
  Step 2: Check Machine Metrics at 3pm:
          CPU: 92%  ← high
          Memory: 65% (normal)
          Disk I/O: 88% ← high
          Network: 40% (normal)

  Conclusion: CPU + Disk I/O are the bottleneck
  Likely cause: A database query doing a full table scan
  Action: Find missing index → add it → resolve spike
```

```
  CORRELATION TABLE:

  API Symptom               Machine Signal         Likely Cause
  ─────────────────────────────────────────────────────────────
  High P99 latency          CPU at 90%+            CPU bound (heavy computation)
  High error rate (503)     Memory OOM             Memory leak or traffic spike
  Requests timing out       Disk I/O > 90%         DB full table scans, no indexes
  Intermittent failures     Packet loss > 0.5%     Network issues
  Gradual performance decay Memory growing daily   Memory leak
  Sudden 5xx spike          All metrics normal     Deployment bug (software fault)
```

---

## Monitoring Stack (Common Tools)

```
  DATA COLLECTION:
  ├── Prometheus        → scrapes metrics from services
  ├── StatsD / InfluxDB → time-series metrics
  └── CloudWatch (AWS)  → managed metrics for AWS services

  VISUALIZATION:
  ├── Grafana           → dashboards and alerting
  └── Kibana            → log visualization (with Elasticsearch)

  LOGGING:
  ├── ELK Stack (Elasticsearch, Logstash, Kibana)
  └── Loki + Grafana    → lightweight log aggregation

  DISTRIBUTED TRACING:
  ├── Jaeger            → open-source distributed tracing
  ├── Zipkin            → distributed request tracing
  └── Datadog APM       → commercial APM with tracing

  ALERTING:
  ├── PagerDuty         → on-call management and escalation
  ├── OpsGenie          → alert routing
  └── Grafana Alerting  → built-in alert rules

  FULL OBSERVABILITY PLATFORMS:
  ├── Datadog           → metrics + logs + traces + alerting
  ├── New Relic         → APM and infrastructure monitoring
  └── Dynatrace         → AI-powered observability
```

---

## Key Takeaways

- **Monitoring** tells you *that* something is wrong; **observability** tells you *why*
- The three pillars of observability are **metrics, logs, and traces**
- API monitoring should track throughput, error rates, health checks, and latency percentiles
- **Use percentiles** (P50, P90, P99) — averages hide tail latency problems
- **Passive health checks** rely on traffic; **active health checks** probe continuously
- Machine monitoring covers CPU, memory, disk I/O, and network — correlate with API metrics to find root cause
- Set **SLOs** with clear alert thresholds; only page on-call for things that need human action
- Always monitor both the application and the infrastructure — a hardware issue can look like a software bug
