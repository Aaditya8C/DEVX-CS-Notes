# Saga Pattern — Distributed Transactions

---

## The Problem

In microservices, a business operation spans multiple services each with its own DB. Traditional ACID transactions cannot cross service boundaries.

```
  Place Order:
  1. Order Service    → create order
  2. Payment Service  → charge ₹5000
  3. Inventory Service→ reserve stock

  What if Step 3 fails after Step 2 succeeded?
  → Customer charged, no stock reserved → inconsistent state
```

---

## What is a Saga?

A sequence of **local transactions**, one per service. On failure, **compensating transactions** undo previous steps.

```
  Saga = local transactions + compensating transactions for rollback
```

---

## Two Types

### 1. Choreography (Event-Driven)
No central coordinator. Each service reacts to events published by the previous service.

```
  [Order Svc] ──publishes──► OrderCreated
  [Payment Svc] ─── hears OrderCreated ──► charges ₹5000 ──publishes──► PaymentSucceeded
  [Inventory Svc] ── hears PaymentSucceeded ──► reserves stock ──publishes──► InventoryReserved
```

**On failure (Inventory fails):**
```
  InventoryFailed published
  → Payment Svc hears it → refunds ₹5000 → publishes PaymentRefunded
  → Order Svc hears it   → cancels order
```

### 2. Orchestration (Central Coordinator)
A Saga Orchestrator tells each service what to do and handles failures.

```
  [Saga Orchestrator]
       │── Step 1: Create Order  ──► [Order Svc]
       │── Step 2: Charge        ──► [Payment Svc]
       │── Step 3: Reserve Stock ──► [Inventory Svc]

  On failure at Step 3:
       │── Compensate: Refund    ──► [Payment Svc]
       │── Compensate: Cancel    ──► [Order Svc]
```

---

## Choreography vs Orchestration

| Feature | Choreography | Orchestration |
|---|---|---|
| Coordinator | None | Central orchestrator |
| Coupling | Loose | Services coupled to orchestrator |
| Flow visibility | Hard to trace | Easy to visualise |
| Single point of failure | No | Yes (orchestrator) |
| Best for | Simple sagas (2–3 steps) | Complex sagas (many steps) |

---

## Compensating Transactions

Each step has an "undo" operation.

| Original | Compensating |
|---|---|
| Create order (PENDING) | Cancel order (CANCELLED) |
| Charge ₹5000 | Refund ₹5000 |
| Reserve stock | Release reservation |

> Compensating transactions are not DB rollbacks — they are new forward operations that logically undo the effect.

---

## Idempotency

Messages can be delivered more than once (retries). Each service must be **idempotent** — processing the same message twice must have the same result as once.

- Attach a unique **idempotency key** (order_id, payment_id) to each operation
- Before processing: check if key already handled — if yes, skip

---

## Key Takeaways

- Saga solves distributed transactions across microservices — no shared DB, no 2PC
- Choreography = event-driven, no coordinator, loose coupling
- Orchestration = central coordinator, easier to reason about
- Every step needs a compensating transaction for rollback
- Make all operations idempotent — messages may arrive more than once
- Tools: Kafka (choreography), AWS Step Functions / Temporal.io (orchestration)
