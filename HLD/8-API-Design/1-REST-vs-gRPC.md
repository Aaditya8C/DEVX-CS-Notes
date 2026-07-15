# API Design — REST, gRPC, GraphQL

---

## What is an API?

An **API (Application Programming Interface)** defines how two components of a system communicate with each other.

```
  Client ──── API ────► Server
         (contract)
```

The API is a **contract** — it defines:
- What operations are available
- What data format to use
- What the response looks like

---

# 1. REST (Representational State Transfer)

## What is REST?

REST is an **architectural style** for designing APIs over HTTP.

It treats every resource (user, order, product) as a **URL endpoint** and uses HTTP methods to perform operations.

```
  Resource:   /users/123
  Methods:    GET, POST, PUT, PATCH, DELETE
```

---

## HTTP Methods → CRUD Mapping

| HTTP Method | Operation | Example |
|---|---|---|
| `GET` | Read | `GET /users/123` → fetch user |
| `POST` | Create | `POST /users` → create user |
| `PUT` | Update (full) | `PUT /users/123` → replace user |
| `PATCH` | Update (partial) | `PATCH /users/123` → update name only |
| `DELETE` | Delete | `DELETE /users/123` → remove user |

---

## REST Request/Response

```
REQUEST:
  GET /products/42 HTTP/1.1
  Host: api.amazon.com
  Authorization: Bearer <token>
  Accept: application/json

RESPONSE:
  HTTP/1.1 200 OK
  Content-Type: application/json

  {
    "id": 42,
    "name": "iPhone 15",
    "price": 79999,
    "inStock": true
  }
```

---

## HTTP Status Codes (Must Know)

| Code | Meaning | When to Use |
|---|---|---|
| `200 OK` | Success | GET, PUT, PATCH success |
| `201 Created` | Resource created | POST success |
| `204 No Content` | Success, no body | DELETE success |
| `400 Bad Request` | Client sent invalid data | Missing fields, wrong type |
| `401 Unauthorized` | Not authenticated | No/invalid token |
| `403 Forbidden` | Authenticated but no permission | User accessing admin route |
| `404 Not Found` | Resource doesn't exist | `GET /users/9999` |
| `409 Conflict` | State conflict | Duplicate email on signup |
| `422 Unprocessable` | Validation error | Invalid email format |
| `429 Too Many Requests` | Rate limited | API abuse |
| `500 Internal Server Error` | Server bug | Unhandled exception |
| `503 Service Unavailable` | Server overloaded/down | DB connection failed |

---

## REST Principles

### 1. Stateless
Each request contains all information needed. Server stores no session state.

```
  GOOD (Stateless):
  GET /orders?userId=123&status=pending  ← all context in request

  BAD (Stateful):
  GET /my-orders  ← server must remember who "my" refers to
```

### 2. Resource-Based URLs
URLs represent **nouns** (things), not verbs (actions).

```
  GOOD:
  GET /users/123/orders
  POST /orders

  BAD:
  GET /getOrdersForUser?id=123
  POST /createNewOrder
```

### 3. Uniform Interface
Same HTTP verbs used consistently across all resources.

### 4. Cacheable
Responses should be marked cacheable or non-cacheable.

```
  Cache-Control: max-age=3600   ← cache for 1 hour
  Cache-Control: no-store       ← never cache (sensitive data)
```

---

## Versioning REST APIs

APIs change over time. Version them to avoid breaking existing clients.

| Strategy | Example | Notes |
|---|---|---|
| URI versioning | `/v1/users`, `/v2/users` | Most common, easy to see |
| Header versioning | `Accept: application/vnd.api.v2+json` | Cleaner URLs |
| Query param | `/users?version=2` | Easy to test in browser |

> **Best practice:** Always version your APIs. Breaking a v1 contract crashes all clients.

---

## REST — Advantages & Disadvantages

### Advantages
- Simple and human-readable
- Works everywhere (browsers, mobile, CLI)
- Easy to debug (use `curl`, Postman, browser)
- Widely understood and adopted
- Stateless — scales horizontally easily

### Disadvantages
- Over-fetching — response contains more data than needed
- Under-fetching — need multiple requests to get related data
- No strict schema — client/server can drift apart
- HTTP/1.1 is text-based — more overhead than binary protocols

---

---

# 2. gRPC (Google Remote Procedure Call)

## What is gRPC?

gRPC is a **high-performance RPC framework** built by Google.

- Uses **Protocol Buffers (Protobuf)** as the serialization format (binary, not JSON)
- Built on **HTTP/2** — supports multiplexing, streaming, header compression
- You define a **contract in a `.proto` file** — both sides must follow it strictly

```
  Client ──── gRPC call ────► Server
              (binary, fast)
```

---

## How gRPC Works

### Step 1: Define the contract (.proto file)

```protobuf
syntax = "proto3";

service UserService {
  rpc GetUser (GetUserRequest) returns (UserResponse);
  rpc CreateUser (CreateUserRequest) returns (UserResponse);
}

message GetUserRequest {
  int32 user_id = 1;
}

message UserResponse {
  int32 id = 1;
  string name = 2;
  string email = 3;
}
```

### Step 2: Generate code

The protobuf compiler generates client and server code automatically in any language (Go, Python, Java, Node.js, etc.)

### Step 3: Call like a local function

```go
// Client-side Go code — looks like a normal function call
user, err := client.GetUser(ctx, &pb.GetUserRequest{UserId: 123})
```

---

## gRPC Streaming Types

Unlike REST (request-response only), gRPC supports 4 communication patterns:

```
1. Unary (request → response)          — like REST
   Client ──request──► Server ──response──► Client

2. Server Streaming                     — server sends multiple responses
   Client ──request──► Server ──response1──► Client
                               ──response2──► Client
                               ──response3──► Client
   Example: Subscribe to live stock price feed

3. Client Streaming                     — client sends multiple requests
   Client ──data1──►
           ──data2──► Server ──response──► Client
           ──data3──►
   Example: File upload in chunks

4. Bidirectional Streaming              — both sides stream simultaneously
   Client ◄──────────────────────────► Server
   Example: Chat app, live video call
```

---

## REST vs gRPC — Comparison

| Feature | REST | gRPC |
|---|---|---|
| Protocol | HTTP/1.1 | HTTP/2 |
| Data Format | JSON (text) | Protobuf (binary) |
| Schema | No strict schema | Strict `.proto` contract |
| Performance | Slower (text parsing) | 5–10x faster (binary) |
| Streaming | No (polling workarounds) | Native bidirectional |
| Browser Support | Native | Needs gRPC-Web proxy |
| Debugging | Easy (readable JSON) | Harder (binary) |
| Code Generation | Manual | Auto-generated clients |
| Versioning | URL versioning | Field numbers in proto |
| Best For | Public APIs, web clients | Internal microservices |

---

## When to Use REST vs gRPC

```
Use REST when:
  Yes Building a public API (third-party developers)
  Yes Client is a browser (native HTTP support)
  Yes Team is small or unfamiliar with Protobuf
  Yes Simple CRUD operations

Use gRPC when:
  Yes Internal microservice communication
  Yes Performance is critical (low latency, high throughput)
  Yes Need streaming (real-time data, IoT, video)
  Yes Polyglot services (auto-generate clients in 10+ languages)
  Yes Strong contract enforcement across teams
```

**Real-World Examples:**
| Company | Choice | Reason |
|---|---|---|
| Netflix | gRPC internally | Low latency between 700+ services |
| Uber | gRPC internally | Real-time ride tracking |
| Google | gRPC everywhere | They built it |
| Stripe | REST | Public API for developers |
| GitHub | REST + GraphQL | Public API |
| Twitter/X | REST | Public API |

---

# 3. GraphQL

## What is GraphQL?

GraphQL is a **query language for APIs** developed by Facebook (2015).

Instead of fixed endpoints, clients **ask for exactly the data they need**.

```
REST (over-fetching):
  GET /users/123
  Response: { id, name, email, address, phone, createdAt, ... }
  Client only needed: name and email

GraphQL (precise):
  query {
    user(id: 123) {
      name
      email
    }
  }
  Response: { name: "Aaditya", email: "a@b.com" }
```

---

## REST vs GraphQL vs gRPC — Summary Table

| Feature | REST | GraphQL | gRPC |
|---|---|---|---|
| Best for | Public APIs | Client-driven UIs | Internal microservices |
| Data fetching | Fixed response | Precise queries | Efficient binary |
| Over-fetching | Common problem | Solved | N/A |
| Learning curve | Low | Medium | High |
| Browser support | Yes Native | Yes Native | No Needs proxy |
| Streaming | No | Yes Subscriptions | Yes Native |
| Schema | None | GraphQL Schema | Protobuf |

---

## Key Takeaways

- **REST** — Use for public-facing APIs. Simple, human-readable, universally supported.
- **gRPC** — Use for internal service-to-service communication. Faster, streaming, strict contracts.
- **GraphQL** — Use when the UI/client needs flexible queries (avoids over/under-fetching).
- In large systems, you'll often see **all three**: REST or GraphQL for external clients, gRPC for internal microservices.
- **Always version your REST APIs.** Always define a `.proto` contract before writing gRPC code.
