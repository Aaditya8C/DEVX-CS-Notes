# API Gateway

---

## What is an API Gateway?

An **API Gateway** is a **single entry point** for all client requests to a backend system.

Instead of clients talking directly to individual microservices, they talk to the API Gateway, which routes, transforms, and controls the traffic.

```
WITHOUT API Gateway (problem):

  Mobile App ──────────────────► User Service      :8001
  Web App    ──────────────────► Order Service     :8002
  Third Party ─────────────────► Payment Service   :8003
  Mobile App ──────────────────► Product Service   :8004

  Problems:
  - Clients must know every service's address
  - Auth logic duplicated in every service
  - Hard to change internal URLs
  - No central rate limiting or logging
```

```
WITH API Gateway (solution):

  Mobile App  ─────┐
  Web App     ─────┼──► [API Gateway] ──► User Service    :8001
  Third Party ─────┤         │        ──► Order Service   :8002
                             │        ──► Payment Service :8003
                             │        ──► Product Service :8004
                    (Single entry point)
```

---

## Responsibilities of an API Gateway

### 1. Routing
Routes each incoming request to the correct backend service based on the URL path.

```
  /api/users/*     → User Service
  /api/orders/*    → Order Service
  /api/products/*  → Product Service
  /api/payments/*  → Payment Service
```

---

### 2. Authentication & Authorization
Validates the token once at the gateway — **services don't need to implement auth themselves**.

```
  Client ──► Gateway [validates JWT] ──► Service
                  │
            If token invalid:
            401 Unauthorized (never reaches service)
```

---

### 3. Rate Limiting
Throttles requests per client/API key to prevent abuse.

```
  User A: 1000 req/min allowed
  User A sends 1500 req/min
  Gateway drops requests 1001-1500 with 429 Too Many Requests
```

---

### 4. Load Balancing
Distributes requests across multiple instances of the same service.

```
  ──► User Service Instance 1
  ──► User Service Instance 2
  ──► User Service Instance 3
```

---

### 5. SSL Termination
Handles HTTPS encryption at the gateway. Internal services communicate over plain HTTP.

```
  Client ──HTTPS──► [API Gateway] ──HTTP──► Services
                   (SSL terminates here)
         (encrypted)              (trusted network)
```

---

### 6. Request/Response Transformation
Translates between protocols or formats.

```
  Client sends REST JSON
  Gateway translates to gRPC binary for internal service
  Service returns gRPC response
  Gateway translates back to JSON for client
```

---

### 7. Caching
Caches responses for repeated identical requests.

```
  GET /products/1  (first time)  → hits Product Service → cached
  GET /products/1  (next 1000x)  → served from Gateway cache
```

---

### 8. Observability
Central point to log all requests, measure latency, and emit metrics.

```
  Every request logged:
  - Client IP, User ID
  - Endpoint, response code
  - Latency (gateway to service + back)
```

---

## Architecture Diagram (Detailed)

```
              ┌──────────────────────────────────────┐
              │             API GATEWAY              │
              │                                      │
  Clients     │  ┌────────┐  ┌──────┐  ┌─────────┐  │
  ─────────►  │  │  Auth  │→ │Rate  │→ │ Router  │  │
              │  │  (JWT) │  │Limit │  │         │  │
              │  └────────┘  └──────┘  └────┬────┘  │
              │                             │        │
              │  ┌───────────┐  ┌───────────┘        │
              │  │SSL Termination│  ┌──Cache──┐      │
              │  └───────────┘  └──┤Logging  │      │
              └────────────────────┴─────────┘──────┘
                                        │
              ┌─────────────────────────┼─────────────────────┐
              │                         │                     │
     ┌────────▼──────┐        ┌─────────▼─────┐    ┌─────────▼─────┐
     │  User Service │        │ Order Service  │    │Payment Service│
     │   (3 pods)    │        │   (5 pods)     │    │   (2 pods)    │
     └───────────────┘        └───────────────┘    └───────────────┘
```

---

## API Gateway vs Load Balancer

These are often confused. They operate at different layers.

| Feature | Load Balancer | API Gateway |
|---|---|---|
| Purpose | Distribute traffic across instances | Route, control, and manage API traffic |
| Layer | L4 (TCP/IP) or L7 (HTTP) | L7 (HTTP/Application) |
| Auth | No No | Yes Yes |
| Rate Limiting | No No | Yes Yes |
| Routing by URL path | Limited | Yes Yes (complex rules) |
| Protocol translation | No No | Yes REST → gRPC |
| Caching | No No | Yes Yes |
| Who uses it | Single service, multiple instances | Microservices ecosystem |

> A system often has **both**: Load Balancer in front of the API Gateway (to scale the gateway itself), and the Gateway routing to services.

```
  Client ──► [Load Balancer] ──► [API Gateway 1]
                              ──► [API Gateway 2]  ──► Services
                              ──► [API Gateway 3]
```

---

## Real-World API Gateway Products

| Product | Used By | Notes |
|---|---|---|
| **AWS API Gateway** | Most AWS users | Managed, integrates with Lambda, Cognito |
| **Kong** | Uber, Expedia | Open-source, extensible with plugins |
| **Nginx** | Netflix, Dropbox | Lightweight, high-performance |
| **Traefik** | Docker/k8s users | Auto-discovers services in Kubernetes |
| **Apigee** (Google) | Enterprise | Feature-rich API management |
| **Zuul** (Netflix) | Netflix | OSS, Java-based, older |

---

## Backend for Frontend (BFF) Pattern

A variant of API Gateway: **one gateway per client type**.

```
  Mobile App   ──► [Mobile BFF]    ──► Microservices
  Web App      ──► [Web BFF]       ──► Microservices
  3rd Party    ──► [Partner BFF]   ──► Microservices
```

**Why?**
- Mobile needs smaller payloads
- Web needs richer data
- Each BFF optimized for its client

**Used by:** Netflix, SoundCloud, Spotify

---

## Failure Modes of API Gateway

| Failure | Impact | Mitigation |
|---|---|---|
| Gateway goes down | **Entire system unreachable** | Run multiple gateway instances behind a Load Balancer |
| Gateway becomes bottleneck | High latency for all services | Scale gateway horizontally |
| Misconfigured routes | Requests go to wrong service | IaC, automated tests for routing rules |
| Auth bug | Security breach | Rigorous auth testing, WAF (Web Application Firewall) |

> The API Gateway is a **single point of failure** if not deployed with redundancy. Always run multiple instances.

---

## Key Takeaways

- API Gateway is the **front door** of a microservices system
- Centralizes **Auth, Rate Limiting, Routing, Logging** — services stay lean
- **Not a replacement for a Load Balancer** — they solve different problems
- Deploy **multiple gateway instances** — it's a critical chokepoint
- Consider **BFF pattern** when clients have very different needs
- Popular choices: **AWS API Gateway** (managed), **Kong** (open-source), **Nginx** (lightweight)
