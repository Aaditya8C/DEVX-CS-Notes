# Saga Pattern — Distributed Transactions

---

## The Problem: Distributed Transactions

In a **microservices** system, a business operation often spans **multiple services**, each with its own database.

Traditional ACID transactions can't span service boundaries — you can't do a 2-phase commit across services you don't own.

```
  E-Commerce: Place an Order

  Step 1: Order Service    → Create order record
  Step 2: Payment Service  → Charge customer ₹5000
  Step 3: Inventory Service→ Reduce stock by 1
  Step 4: Notification Svc → Send confirmation email

  Problem: What if Payment succeeds but Inventory fails?
  → Customer was charged, but no stock was reserved
  → Inconsistent state!
```

---

## What is the Saga Pattern?

A **Saga** breaks a long-running transaction into a **sequence of local transactions**, each local to its own service.

If one step **fails**, the Saga runs **compensating transactions** to undo the previous steps.

```
  SAGA = Sequence of local transactions + compensating transactions for rollback
```

---

## Saga Execution Types

### 1. Choreography-Based Saga

Services **communicate via events** (Kafka, SQS). Each service listens for events and decides what to do next.

No central coordinator — services react to each other's events.

```
  [Order Service]
      │ publishes: OrderCreated
      ▼
  [Payment Service]  ← listens to OrderCreated
      │ publishes: PaymentSucceeded / PaymentFailed
      ▼
  [Inventory Service] ← listens to PaymentSucceeded
      │ publishes: InventoryReserved / InventoryFailed
      ▼
  [Notification Service] ← listens to InventoryReserved
      │ publishes: EmailSent
```

**Happy Path:**
```
  OrderCreated → PaymentSucceeded → InventoryReserved → EmailSent
```

**Failure Path (Inventory failed):**
```
  OrderCreated → PaymentSucceeded → InventoryFailed
                       │
                  [Payment Service listens to InventoryFailed]
                  → Publishes: PaymentRefunded  (compensating transaction)
                       │
                  [Order Service listens to PaymentRefunded]
                  → Marks order as CANCELLED
```

---

### 2. Orchestration-Based Saga

A **central Saga Orchestrator** tells each service what to do next. It coordinates the saga.

```
                      [Saga Orchestrator]
                           │
         ┌─────────────────┼──────────────────┐
         │                 │                  │
    Step 1:           Step 2:            Step 3:
  Create Order  →  Charge Payment  →  Reserve Inventory
         │                 │                  │
    [Order Svc]      [Payment Svc]      [Inventory Svc]

  On failure at Step 3 (Inventory):
  Orchestrator → Refund Payment (compensating) → Cancel Order (compensating)
```

---

## Choreography vs Orchestration

| Feature | Choreography | Orchestration |
|---|---|---|
| Coordinator | None (event-driven) | Central Saga Orchestrator |
| Coupling | Loose (services don't know each other) | Services know the orchestrator |
| Complexity | Hard to track flow | Easier to visualize |
| Single point of failure | No | Yes (orchestrator) |
| Debugging | Harder (trace events) | Easier (single log) |
| Best for | Simple sagas (2–3 steps) | Complex sagas (many steps) |
| Used by | Simple event-driven systems | Uber, Netflix, Order management |

---

## Compensating Transactions

A **compensating transaction** is the "undo" of a previous step.

| Original Transaction | Compensating Transaction |
|---|---|
| Create Order → status: PENDING | Cancel Order → status: CANCELLED |
| Charge ₹5000 from wallet | Refund ₹5000 to wallet |
| Reserve 1 unit of inventory | Release reservation |
| Send email "Order Confirmed" | Send email "Order Cancelled" |

> ⚠️ Compensating transactions are **not the same as database rollbacks**.
> They are new forward-moving transactions that **logically undo** the effect.
> You cannot un-send an email — but you can send a cancellation email.

---

## Full Saga Flow Example (E-Commerce Order)

```
  USER: "Place Order for ₹5000"

  Step 1: ORDER SERVICE
    → Create order { id: 123, status: PENDING }
    → Publish: OrderCreated

  Step 2: PAYMENT SERVICE (hears OrderCreated)
    → Deduct ₹5000 from wallet
    → Publish: PaymentSucceeded

  Step 3: INVENTORY SERVICE (hears PaymentSucceeded)
    → Reserve 1 unit of "iPhone 15"
    → Publish: InventoryReserved  Yes

  Step 4: NOTIFICATION SERVICE (hears InventoryReserved)
    → Send email "Order confirmed!"

  Final: ORDER SERVICE (hears InventoryReserved)
    → Update order { status: CONFIRMED }

  ─────────────────────────────────────────────────────

  FAILURE SCENARIO: Inventory out of stock at Step 3

  Step 3: INVENTORY SERVICE
    → Cannot reserve — out of stock
    → Publish: InventoryFailed

  Compensation Step 1: PAYMENT SERVICE (hears InventoryFailed)
    → Refund ₹5000 to wallet
    → Publish: PaymentRefunded

  Compensation Step 2: NOTIFICATION SERVICE (hears InventoryFailed)
    → Send email "Order cancelled — out of stock"

  Compensation Step 3: ORDER SERVICE (hears PaymentRefunded)
    → Update order { status: CANCELLED }
```

---

## Saga vs 2-Phase Commit (2PC)

| Feature | Saga Pattern | 2-Phase Commit (2PC) |
|---|---|---|
| Works across services | Yes Yes | No Requires shared transaction manager |
| Locks resources | No No locks | Yes Locks resources during commit |
| Consistency | Eventual | Strong |
| Performance | High | Low (blocking locks) |
| Fault tolerance | High | Low (coordinator is SPOF) |
| Use in microservices | Yes Yes | No Not practical |

---

## Idempotency in Sagas

Because sagas use retries, the same message may be delivered **more than once**.

Each service must be **idempotent** — processing the same message twice must have the same effect as once.

```
  PaymentSucceeded message delivered twice:

  Non-idempotent: ₹5000 deducted twice → BUG
  Idempotent:     Check if payment_id already processed → skip if yes → SAFE
```

**How to make operations idempotent:**
- Use a unique **idempotency key** per request (order_id, payment_id)
- Before processing: check if key already exists in a `processed_events` table
- If exists → skip (already processed)

---

## Where Saga is Used

| Company | Use Case |
|---|---|
| **Amazon** | Order placement across Order, Payment, Inventory, Fulfillment |
| **Uber** | Ride booking across Matching, Payment, Driver, Trip services |
| **Netflix** | Billing across Account, Payment, Subscription services |
| **Airbnb** | Booking across Listing, Payment, Host Notification services |

---

## Key Takeaways

- Saga solves **distributed transaction** problems in microservices — no shared DB, no 2PC
- **Two types:** Choreography (event-driven, no coordinator) and Orchestration (central coordinator)
- Each step has a **compensating transaction** to rollback on failure
- Sagas provide **eventual consistency**, not strong consistency
- Make every service operation **idempotent** — messages can be delivered more than once
- Saga complexity grows with steps — for complex workflows, use **Orchestration**
- Tools: **AWS Step Functions** (orchestration), **Kafka** (choreography), **Temporal.io** (workflow orchestration)
