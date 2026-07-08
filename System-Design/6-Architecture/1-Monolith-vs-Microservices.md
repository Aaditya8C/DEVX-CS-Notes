# Monolith vs Microservices

---

## Monolithic Architecture

**Definition:** The entire application — UI, business logic, and data access — is built and deployed as a **single unit**.

```
┌─────────────────────────────────────────┐
│             Monolith (Single Deployable) │
│                                         │
│  ┌──────────┐  ┌──────────┐  ┌───────┐  │
│  │   User   │  │  Order   │  │Payment│  │
│  │ Service  │  │ Service  │  │Service│  │
│  └──────────┘  └──────────┘  └───────┘  │
│                                         │
│  ┌──────────┐  ┌──────────┐             │
│  │ Inventory│  │Notification│           │
│  │ Service  │  │  Service  │            │
│  └──────────┘  └──────────┘             │
│                                         │
└─────────────────────────────────────────┘
                    │
             Single Database
```

All modules share the same process, memory, and database. One build artifact is deployed.

### Advantages
- **Simple to develop** — one codebase, one IDE, one build.
- **Easy to test** — end-to-end testing in one place.
- **Simple to deploy** — one artifact, one deploy command.
- **Low latency** — inter-module calls are in-process (no network hops).
- **Easy to debug** — single log stream, single stack trace.

### Disadvantages
- **Scaling is all-or-nothing** — can't scale just the Order module; must scale the entire app.
- **Long build/deploy times** — changing one line requires redeploying everything.
- **Technology lock-in** — entire app must use the same language/framework.
- **High blast radius** — a bug in Payments can crash the entire app.
- **Team coupling** — large teams step on each other's code.
- **Harder to maintain** over time as codebase grows (big ball of mud).

### When to Use
- Early-stage startups validating an idea.
- Small teams (< 10 engineers).
- Simple domains with limited business logic.
- When you need to move fast and keep infrastructure simple.

### Real-World Examples
| Company | Context |
|---|---|
| **Amazon** (early 2000s) | Started as a monolith before migrating |
| **Netflix** (pre-2009) | Single DVD rental monolith before migration |
| **Shopify** | Still largely a monolith (Rails), highly optimized |
| **Stack Overflow** | Monolith serving billions of requests efficiently |
| **Basecamp** | Deliberately monolith by design philosophy |

---

## Microservices Architecture

**Definition:** The application is broken into **small, independent services**, each responsible for a single business capability, deployed and scaled independently.

```
                        [API Gateway]
                             │
        ┌──────────┬──────────┼──────────┬──────────┐
        │          │          │          │          │
  [User Svc]  [Order Svc] [Payment  [Inventory [Notification
    :8001       :8002      Svc :8003]  Svc :8004]  Svc :8005]
     │             │          │          │          │
  [User DB]   [Order DB] [Payment DB] [Inv DB]  [Message Queue]
 (Postgres)   (Mongo)   (Postgres)  (Redis)    (Kafka)
```

Each service:
- Has its own database (no shared DB).
- Communicates via APIs (REST/gRPC) or events (Kafka).
- Is deployed, scaled, and updated independently.
- Can be written in a different language/framework.

### Advantages
- **Independent scaling** — scale only the service under load (e.g., scale Payment Svc during checkout).
- **Independent deployment** — deploy Order Svc without touching User Svc.
- **Technology flexibility** — use Python for ML service, Go for high-throughput service.
- **Fault isolation** — a crash in Notification Svc does not bring down Orders.
- **Team autonomy** — each team owns one service end-to-end.
- **Easier to understand** — each service is small and focused.

### Disadvantages
- **Operational complexity** — dozens of services to deploy, monitor, and manage.
- **Network latency** — inter-service calls add overhead vs in-process calls.
- **Distributed system problems** — eventual consistency, partial failures, distributed tracing.
- **Data management** — no shared DB; cross-service queries require APIs or event sourcing.
- **Testing is harder** — need to test service interactions, contracts, and network failures.
- **Higher infrastructure cost** — each service needs its own CI/CD, container, monitoring.

### When to Use
- Large teams (multiple squads working in parallel).
- High-scale systems where different parts have different load profiles.
- Different parts of the system need different tech stacks.
- Organizational need for independent release cycles.

### Real-World Examples
| Company | Context |
|---|---|
| **Netflix** | 700+ microservices; each feature team owns their service |
| **Amazon** | Transitioned from monolith; now thousands of services |
| **Uber** | Domain services (Maps, Pricing, Driver Matching, Payments) |
| **Airbnb** | Migrated monolith → microservices as scale demanded |
| **Twitter** | Timelines, Search, Tweets as separate services |
| **LinkedIn** | Service-oriented architecture across profile, feed, messaging |

---

## Comparison Table

| Dimension | Monolith | Microservices |
|---|---|---|
| Deployment | Single artifact | Per-service deployments |
| Scaling | Scale entire app | Scale individual services |
| Development speed | Fast initially | Fast at scale (parallel teams) |
| Complexity | Low operational | High operational |
| Fault isolation | Low | High |
| Tech stack | Single | Polyglot |
| Database | Shared | Per-service (DB per service) |
| Best for | Small teams, early stage | Large teams, high scale |
| Latency | Low (in-process) | Higher (network calls) |
| Testing | Simpler | Harder (integration testing) |

---

## Migrating from Monolith to Microservices

**Key principle:** Do not rewrite everything at once. Extract services **incrementally**.

**Classic pattern: Strangler Fig**
- New features are built as microservices.
- Old monolith code is gradually replaced by services over time.
- Traffic is routed via an API Gateway — the monolith and new services coexist.

### Migration Steps

**Example: E-commerce monolith (User, Order, Payment, Inventory, Notification)**

---

#### Step 1: Identify Boundaries (Domain Analysis)
- Map the monolith into logical domains (bounded contexts).
- Find modules with high change frequency or high load — extract these first.
- **Extract the module least coupled to the rest first.**

```
Monolith
├── User Module        ← High change frequency → extract first
├── Order Module       ← Core domain → extract second
├── Payment Module     ← High isolation need → extract third
├── Inventory Module
└── Notification Module ← Least coupled → easiest to extract
```

---

#### Step 2: Extract the First Service (e.g., Notification)
- Move notification logic into a standalone service.
- Monolith calls Notification Svc via REST or publishes events to a queue (Kafka).
- Notification Svc has its own database.

```
[Monolith] ──Kafka event──→ [Notification Svc]
                                    │
                              [Notification DB]
```

**Infra changes at this step:**
- Containerize Notification Svc (Docker).
- Deploy it separately (Kubernetes pod or EC2 instance).
- Set up its own CI/CD pipeline.
- Add monitoring/logging (Datadog, CloudWatch).

---

#### Step 3: Introduce an API Gateway
- Route external traffic through a gateway instead of hitting the monolith directly.
- The gateway acts as the single entry point and routes to whichever backend handles each route.

```
Client → [API Gateway]
               │
    ┌──────────┴───────────┐
[Monolith]         [Notification Svc]
(still handles     (extracted service)
 most routes)
```

**Infra changes:**
- Deploy API Gateway (AWS API Gateway, Kong, Nginx).
- Add routing rules per service.

---

#### Step 4: Extract More Services Incrementally
- Repeat for User, Order, Payment, Inventory.
- Each extraction: create new DB, migrate data, route traffic gradually (feature flags / canary deploys).

```
Client → [API Gateway]
               │
    ┌──────────┼──────────┬──────────┐
[User Svc] [Order Svc] [Payment Svc] [Monolith remnant]
```

**Infra changes per service:**
- Database per service (provision new RDS/Mongo instance).
- Event bus for async communication (Kafka/SQS).
- Service discovery (Kubernetes DNS, Consul).
- Distributed tracing (Jaeger, Zipkin, AWS X-Ray).
- Centralized logging (ELK Stack, Datadog).

---

#### Step 5: Decommission the Monolith
- Once all domains are extracted, the monolith is retired.
- API Gateway routes 100% of traffic to microservices.

```
Client → [API Gateway]
               │
    ┌──────────┼──────────┬──────────┬───────────┐
[User Svc] [Order Svc] [Payment Svc] [Inv Svc] [Notif Svc]
```

### Infrastructure Changes Summary

| Phase | Infrastructure Added |
|---|---|
| Step 1: First extraction | Docker, separate DB, CI/CD pipeline, monitoring |
| Step 2: API Gateway | API Gateway (Kong / AWS AG / Nginx) |
| Step 3: More services | Kafka/SQS, service discovery, per-service DBs |
| Step 4: Observability | Distributed tracing, centralized logs, dashboards |
| Step 5: Orchestration | Kubernetes cluster, Helm charts, auto-scaling policies |

### Common Pitfalls During Migration
- **Distributed monolith** — services split but sharing the same DB. Avoid at all costs.
- **Big bang rewrite** — trying to migrate everything at once. Always incremental.
- **Ignoring data migration** — splitting the DB is the hardest part; plan it carefully.
- **No observability** — in a distributed system, you need tracing before you need anything else.
