# Circuit Breaker Pattern

---

## The Problem: Cascading Failures

In a microservices system, services call each other. If one service goes down or becomes slow, all services calling it can get stuck waiting — **bringing the whole system down**.

```
  User Request
       │
  [API Gateway]
       │
  [Order Service] ──────────────────► [Payment Service]
                                              │
                                         ⚠️ Payment Service is DOWN
                                         (DB overloaded, taking 30s to respond)

  Effect:
  - Order Service's threads are all waiting (30s timeouts)
  - Order Service becomes unresponsive too
  - API Gateway times out
  - User sees error

  This is called CASCADING FAILURE.
```

---

## What is a Circuit Breaker?

A **Circuit Breaker** is a design pattern that **prevents cascading failures** by stopping calls to a failing service and allowing it time to recover.

It wraps remote service calls and **monitors for failures**. When failures exceed a threshold, it **"trips"** (opens) and rejects further calls immediately.

> Named after the electrical circuit breaker that cuts power to prevent damage.

---

## Circuit Breaker States

The Circuit Breaker has **3 states**:

```
              failures > threshold
  CLOSED ─────────────────────────────► OPEN
    │                                      │
    │ (normal operation)      (fail fast)  │
    │                                      │ after timeout
    │                                      ▼
    │◄────────────────────────────── HALF-OPEN
         if probe request succeeds
         (back to CLOSED)

         if probe request fails
         ──────────────────────────────► OPEN again
```

---

### State 1: CLOSED (Normal)

- All requests pass through to the downstream service
- Circuit Breaker **counts failures** (errors, timeouts)
- If failure rate stays below threshold → stays CLOSED

```
  Order Service ──── request ────► Payment Service ──► response Yes
                                   (working fine)
  
  Failure count: 0/10 → CLOSED Yes
```

---

### State 2: OPEN (Tripped)

- Failures exceeded the threshold (e.g., 5 failures in 10 seconds)
- Circuit Breaker **immediately rejects** all calls — no waiting for Payment Service
- Returns a fallback response or error instantly

```
  Order Service ──── request ────► [Circuit Breaker]
                                         │
                                   No OPEN — rejects immediately
                                   Returns: "Payment service unavailable, try later"
                                         │
                                   Does NOT call Payment Service
                                   (gives it time to recover)
```

**Why this is better than waiting:**
- Order Service threads are **freed immediately** — no 30-second hangs
- Order Service remains responsive to users
- Payment Service gets a chance to recover without more load

---

### State 3: HALF-OPEN (Testing)

- After a configured **timeout** (e.g., 30 seconds), the circuit enters HALF-OPEN
- Allows **one test request** through to Payment Service
- If it **succeeds** → circuit CLOSES (back to normal)
- If it **fails** → circuit OPENS again (waits another timeout)

```
  (After 30s timeout)
  Order Service ──── 1 probe request ────► Payment Service
                                                 │
                                          Yes Success → CLOSED
                                          No Fail   → OPEN (wait again)
```

---

## Full Flow Example

```
  t=0s:   Payment Service working fine. Circuit: CLOSED Yes

  t=10s:  Payment Service DB overloads. Starts timing out.
          Failure count: 1, 2, 3, 4, 5 (threshold = 5)
          Circuit OPENS ⚡

  t=10-40s: Order Service calls payment.
            Circuit rejects ALL calls immediately.
            Returns: "Service unavailable" in 1ms (not 30s timeout).
            Payment Service receives NO traffic → can recover.

  t=40s:  Circuit enters HALF-OPEN.
          Sends 1 probe request to Payment Service.

  t=40s:  Payment Service has recovered. Probe request succeeds. Yes
          Circuit CLOSES. Normal traffic resumes.
```

---

## Circuit Breaker Parameters

| Parameter | Typical Value | Meaning |
|---|---|---|
| **Failure threshold** | 50% in 10s | Trip open when 50% of calls fail |
| **Minimum calls** | 10 | Don't trip on 1 failure out of 1 call |
| **Open duration** | 30s | Wait 30s before entering HALF-OPEN |
| **Probe requests** | 1–3 | Number of test requests in HALF-OPEN |
| **Success threshold** | 2 | Close after 2 consecutive successes |

---

## Circuit Breaker vs Retry

These are complementary patterns, not alternatives.

| Pattern | Use When |
|---|---|
| **Retry** | Transient failure — the error is temporary (network blip, brief overload) |
| **Circuit Breaker** | Systemic failure — the service is down for a significant time |

```
  Retry alone (dangerous):
  Payment Service is DOWN for 2 minutes.
  Order Service retries every 1 second → 120 retry calls → floods Payment Service
  Payment Service can't recover under the extra load.

  Circuit Breaker + Retry (correct):
  After 5 failures → Circuit OPENS → Order Service stops calling
  Payment Service can recover without extra load.
  After 30s → HALF-OPEN → probe → SUCCESS → resume
```

> Best practice: Use **Retry for transient errors** and **Circuit Breaker for systemic failures**. Use both together.

---

## Fallback Strategies

When the circuit is OPEN, you need a fallback:

| Strategy | Example |
|---|---|
| **Return cached response** | Return last known product price from cache |
| **Return default value** | "Payment unavailable, try later" |
| **Degrade gracefully** | Show product but disable "Buy Now" button |
| **Queue for later** | Put the payment request in a queue, process when service recovers |

---

## Architecture with Circuit Breakers

```
              [API Gateway]
                    │
     ┌──────────────┼──────────────┐
     │              │              │
[Order Svc]   [User Svc]   [Product Svc]
     │
  [CB: Payment]  ← Circuit Breaker wrapping Payment call
     │
  [Payment Svc]


  Each inter-service call has its own Circuit Breaker:
  Order → Payment CB
  Order → Inventory CB
  User  → Notification CB
```

---

## Popular Circuit Breaker Libraries

| Library | Language | Used By |
|---|---|---|
| **Resilience4j** | Java | Spring Boot microservices |
| **Hystrix** (deprecated) | Java | Netflix (original, now deprecated) |
| **Polly** | .NET | .NET microservices |
| **opossum** | Node.js | Node.js services |
| **go-circuit** | Go | Go microservices |
| **Istio / Envoy** | Service Mesh | Any language (network level) |

> Netflix built **Hystrix** and open-sourced it. It's now deprecated in favor of **Resilience4j**.

---

## Bulkhead Pattern (Companion to Circuit Breaker)

Related pattern: **Bulkhead** isolates different calls into separate thread pools so one slow service can't consume all threads.

```
  WITHOUT Bulkhead:
  Order Service has 100 threads total.
  Payment Service is slow → all 100 threads stuck waiting.
  Order Service can't handle any other requests.

  WITH Bulkhead:
  Payment calls:   max 20 threads
  Inventory calls: max 30 threads
  Notification:    max 10 threads
  Other:           40 threads

  Payment Service is slow → only 20 threads stuck.
  Remaining 80 threads still serve other requests.
```

---

## Key Takeaways

- Circuit Breaker **prevents cascading failures** — if downstream service is down, fail fast
- **3 states**: CLOSED (normal) → OPEN (reject all) → HALF-OPEN (test)
- When OPEN → return a **fallback** (cache, default, queue) instead of error
- **Retry** handles transient errors; **Circuit Breaker** handles systemic failures — use both
- Use **Bulkhead** alongside Circuit Breaker to isolate thread pool exhaustion
- In production: use **Resilience4j** (Java) or **Istio** (service mesh, any language)
- Always add **monitoring** — alert when circuit trips in production
