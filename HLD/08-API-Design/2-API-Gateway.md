# API Gateway

---

## What is it?

A **single entry point** for all client traffic in a microservices system.

```
  WITHOUT:                        WITH:
  Client ──► User Svc :8001       Client ──► [API Gateway] ──► User Svc
  Client ──► Order Svc :8002                      │         ──► Order Svc
  Client ──► Payment Svc :8003                    │         ──► Payment Svc
  (every client knows every addr)      (one address, one contract)
```

---

## Responsibilities

| Responsibility | What it does |
|---|---|
| **Routing** | `/api/orders/*` → Order Service |
| **Auth** | Validates JWT once — services don't implement auth |
| **Rate Limiting** | Throttle per client/API key |
| **SSL Termination** | HTTPS outside, HTTP inside trusted network |
| **Load Balancing** | Distributes across service instances |
| **Caching** | Cache responses for repeated identical requests |
| **Protocol Translation** | REST JSON → gRPC binary (and back) |
| **Logging** | Central point to log all requests and latency |

---

## API Gateway vs Load Balancer

| Feature | Load Balancer | API Gateway |
|---|---|---|
| Purpose | Distribute traffic across instances | Route and control API traffic |
| Auth | No | Yes |
| Rate Limiting | No | Yes |
| URL-based routing | Limited | Yes (complex rules) |
| Protocol translation | No | Yes |

> Use both together: LB in front of the gateway (to scale the gateway itself), gateway in front of services.

```
  Client ──► [Load Balancer] ──► [API Gateway 1]
                              ──► [API Gateway 2] ──► Services
```

---

## Backend for Frontend (BFF)

One gateway per client type, each optimised for its client's needs.

```
  Mobile App ──► [Mobile BFF]   ──► Services  (smaller payloads)
  Web App    ──► [Web BFF]      ──► Services  (richer data)
  3rd Party  ──► [Partner BFF]  ──► Services  (rate-limited, scoped)
```

Used by: Netflix, Spotify, SoundCloud.

---

## Failure Modes

- Gateway is a **single point of failure** — always run multiple instances behind a LB.
- Misconfigured routes → requests hit wrong service.
- Becomes a **bottleneck** under extreme load → scale horizontally.

---

## Popular Choices

| Product | Notes |
|---|---|
| AWS API Gateway | Managed, integrates with Lambda and Cognito |
| Kong | Open-source, extensible plugins |
| Nginx | Lightweight, high-performance |
| Traefik | Auto-discovers Kubernetes services |

---

## Key Takeaways

- API Gateway is the **front door** of a microservices system
- Centralises Auth, Rate Limiting, Routing, Logging — services stay lean
- Not a replacement for a Load Balancer — they solve different problems
- Deploy multiple instances — it is a critical chokepoint
