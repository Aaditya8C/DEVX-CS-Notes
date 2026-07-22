# Circuit Breaker Pattern

---

## The Problem: Cascading Failures

If a downstream service is slow or down, calling services accumulate waiting threads — eventually the entire system freezes.

```
  [Order Svc] ──────────────────────► [Payment Svc]
                                            │
                                     DB overloaded
                                     Takes 30s to respond

  Effect: Order Svc threads all blocked → Order Svc goes down
          → API Gateway times out → User sees error
  This is called: Cascading Failure
```

---

## What is a Circuit Breaker?

Wraps remote calls and **monitors failures**. When failures exceed a threshold, it **trips open** and rejects further calls immediately — giving the failing service time to recover.

---

## Three States

```
  CLOSED ──(failures > threshold)──► OPEN ──(after timeout)──► HALF-OPEN
    │                                                                │
    │◄──────(probe request succeeds)─────────────────────────────────┘
                                          (if probe fails → OPEN again)
```

### CLOSED — Normal
All requests pass through. Failure count monitored. Below threshold — stays closed.

### OPEN — Tripped
Rejects all calls **immediately** without touching the downstream service.
Returns a fallback response in ~1ms instead of waiting 30s.

### HALF-OPEN — Testing Recovery
After a configured timeout, allows 1 probe request through.
- Probe succeeds → CLOSED (resume normal traffic)
- Probe fails → OPEN again (wait another timeout)

---

## Example Flow

```
  t=0s:   Payment Svc working fine.  Circuit: CLOSED

  t=10s:  Payment Svc DB overloads. 5 timeouts in a row.
          Circuit trips OPEN.

  t=10–40s: All payment calls rejected immediately.
            Order Svc stays responsive for other operations.
            Payment Svc receives no load — can recover.

  t=40s:  Circuit enters HALF-OPEN. Sends 1 probe.
          Probe succeeds → Circuit CLOSED. Normal traffic resumes.
```

---

## Circuit Breaker vs Retry

| Pattern | Use When |
|---|---|
| Retry | Transient failure — brief network blip, short overload |
| Circuit Breaker | Systemic failure — service down for significant time |

> Retry alone during a systemic failure floods the already-down service with more load.
> Use both: retry for transient, circuit breaker for systemic.

---

## Fallback Strategies

| Strategy | Example |
|---|---|
| Return cached response | Return last known price from Redis |
| Return default value | "Payment unavailable, try later" |
| Graceful degradation | Show product, disable "Buy Now" button |
| Queue for later | Enqueue the request, process when service recovers |

---

## Bulkhead (Related Pattern)

Isolate each downstream call into its own thread pool so one slow service cannot exhaust all threads.

```
  Without: Payment Svc slow → all 100 threads stuck → Svc unresponsive

  With Bulkhead:
  Payment calls: max 20 threads
  Inventory:     max 30 threads
  → Payment slow → only 20 threads stuck, 80 still serve other traffic
```

---

## Key Takeaways

- Circuit Breaker prevents cascading failures — fail fast instead of waiting
- 3 states: CLOSED (normal) → OPEN (reject all) → HALF-OPEN (test)
- Always provide a **fallback** when the circuit is open
- Retry handles transient errors; Circuit Breaker handles systemic failures — use both
- Add **Bulkhead** alongside to isolate thread pool exhaustion
- Libraries: Resilience4j (Java), Polly (.NET), Istio (service mesh, any language)
