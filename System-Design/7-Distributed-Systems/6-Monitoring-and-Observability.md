# Monitoring and Observability

## Monitoring vs Observability

**Monitoring** — tells you *that* something is wrong (alerts, dashboards)
**Observability** — tells you *why* it's wrong (understand internal state from external outputs)

### Three Pillars of Observability

| Pillar  | What it is                            | Example                          |
|---------|---------------------------------------|----------------------------------|
| Metrics | Numbers over time                     | "CPU is 90%", "RPS = 5000"       |
| Logs    | Text events with context              | "ERROR: DB connection timeout"   |
| Traces  | Request flow across services          | "Checkout → Payments took 300ms" |

---

# API Monitoring

## 1. Throughput

**Requests Per Second (RPS)** — how many requests your API handles per second.

- Sudden **drop** in RPS → service may be down or rejecting traffic
- Sudden **spike** → traffic surge; may need to scale

---

## 2. Error Codes (Error Rate)

```
  Error Rate = (5xx errors / total requests) × 100

  2xx → Success
  4xx → Client error (bad request, unauthorized, not found)
  5xx → Server error (our fault — crash, timeout, overload)
```

Track error rate **per endpoint** to know exactly where the problem is.

Alert example: `Error rate > 1% for 5 minutes → page on-call`

---

## 3. Health Checks

A dedicated endpoint (`GET /health`) that returns the service's live status.

### Passive Health Check
Observes real traffic. If a server returns N consecutive errors → marked unhealthy → removed from load balancer.

- No extra overhead
- Blind when traffic is zero

### Active Health Check
Monitor sends test probes to each service on a schedule (e.g., every 10 seconds).

- Detects failures even with no traffic
- Small overhead; occasional false positives

| Aspect      | Passive              | Active                    |
|-------------|----------------------|---------------------------|
| Mechanism   | Watches real traffic | Sends test probes         |
| Works with zero traffic | No  | Yes                      |
| Overhead    | None                 | Small                     |
| Best for    | High-traffic services| All services              |

---

## 4. Latency — Percentiles

**Never use averages** — one slow request (e.g., 5000ms) skews the average and hides the real picture.

**Use percentiles:**

```
  P50  = 50% of requests completed within this time   (median / typical user)
  P70  = 70% of requests completed within this time
  P90  = 90% of requests completed within this time   (most users)
  P99  = 99% of requests completed within this time   (worst-case / tail latency)
```

**Example — Checkout API:**

| Percentile | Latency | Meaning                              |
|------------|---------|--------------------------------------|
| P50        | 120ms   | Typical user experience              |
| P70        | 180ms   | 70% of users are this fast or better |
| P90        | 350ms   | 10% of users wait more than 350ms    |
| P99        | 2100ms  | 1% wait over 2s → SLA breach         |

**Why tail latency matters:**
At scale (1B requests/day), P99 = 1% = 10M users/day experiencing slowness.
If you call 5 services, probability that at least one is slow = `1 - (0.99)^5 ≈ 5%`.

### Alert Setup Example

```
  SLO Targets:
    P50 < 100ms
    P90 < 300ms
    P99 < 1000ms
    Error rate < 0.5%

  Alerts:
    WARN:  P90 > 250ms for 5 min
    ALERT: P99 > 1000ms for 2 min → page on-call
```

---

## API Monitoring Summary

| Metric       | What It Measures       | Alert Threshold Example       |
|--------------|------------------------|-------------------------------|
| Throughput   | Requests per second    | Drop > 30% in 5 min           |
| Error Rate   | % of 5xx responses     | > 1% for 5 min                |
| Health Check | Is service alive?      | 3 consecutive failures        |
| P50 Latency  | Typical user latency   | > 200ms for 10 min            |
| P90 Latency  | Most users served by   | > 500ms for 5 min             |
| P99 Latency  | Worst-case latency     | > 1000ms for 2 min (critical) |

---

# Machine Monitoring

## CPU Usage

% of time the processor is busy.

| Level    | Range     | Action                         |
|----------|-----------|--------------------------------|
| Normal   | 0–50%     | Healthy                        |
| Moderate | 50–70%    | Monitor closely                |
| High     | 70–85%    | Investigate, consider scaling  |
| Critical | 85%+      | Performance degrading, alert   |

**Common causes:** Heavy DB queries, CPU-bound computation, traffic spike, N+1 queries, infinite loops.

---

## Memory (RAM) Usage

Amount of RAM in use. Key risk: **memory leaks** — objects allocated but never freed → steady growth → eventual OOM crash.

**Memory leak pattern:**
```
  Memory:
  100% |                          CRASH
   60% |                    ─────/
   20% |──────────────────/
       Day 1    Day 2    Day 3
```

**Alert:** Memory > 80% for 10 min → investigate immediately.

---

## Disk I/O

Rate of data read/written to disk. Measured in MB/s and IOPS (operations per second).

**High read I/O causes:** Cache miss, full table scans (missing index), large data exports.
**High write I/O causes:** Heavy inserts/updates, verbose logging, WAL under load.

**Alert:** Disk utilization > 90% → slowdowns cascade to API latency.

---

## Network

| Metric           | What it means                              | Alert Threshold     |
|------------------|--------------------------------------------|---------------------|
| Inbound/Outbound | MB/s entering and leaving the server       | > 80% of NIC capacity |
| Packet loss      | % of packets dropped                       | > 0.1%              |
| Inter-service latency | ms between internal services          | Sudden spike > baseline |

**Packet loss impact:**
- 0.1% → noticeable TCP slowdown
- 1%+ → TCP retransmits, serious degradation
- 5%+ → connections failing, services appear down

---

## Machine Monitoring Summary

| Metric         | Warning      | Critical     | Mitigation                     |
|----------------|--------------|--------------|--------------------------------|
| CPU            | > 70%        | > 90%        | Scale out, optimize code       |
| Memory         | > 80%        | > 90%        | Fix leak, add RAM, scale       |
| Disk I/O util. | > 80%        | > 95%        | Add indexes, SSD, partition    |
| Network util.  | > 80%        | > 90%        | CDN, compress, scale           |
| Packet loss    | > 0.1%       | > 1%         | Investigate network path       |
| Disk free space| < 20% free   | < 10% free   | Add disk, archive/delete logs  |

---

## How API and Machine Metrics Correlate

| API Symptom            | Machine Signal         | Likely Root Cause            |
|------------------------|------------------------|------------------------------|
| High P99 latency       | CPU > 90%              | CPU-bound code, heavy computation |
| High 503 error rate    | Memory OOM             | Memory leak or traffic spike |
| Requests timing out    | Disk I/O > 90%         | Missing DB index, full scan  |
| Intermittent failures  | Packet loss > 0.5%     | Network issue                |
| Gradual slowdown       | Memory growing daily   | Memory leak                  |
| Sudden 5xx spike       | All infra metrics normal | Deployment bug (software)  |

---

## Common Monitoring Tools

| Category   | Tools                                          |
|------------|------------------------------------------------|
| Metrics    | Prometheus, Datadog, CloudWatch, InfluxDB      |
| Dashboards | Grafana, Kibana                                |
| Logs       | ELK Stack, Loki                                |
| Tracing    | Jaeger, Zipkin, Datadog APM                    |
| Alerting   | PagerDuty, OpsGenie, Grafana Alerting          |
| Full stack | Datadog, New Relic, Dynatrace                  |
