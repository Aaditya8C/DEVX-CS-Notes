# API Design — REST vs gRPC

---

## REST (Representational State Transfer)

Architectural style for APIs over HTTP. Treats resources as URLs, uses HTTP verbs.

```
  GET    /users/123       → fetch user
  POST   /users           → create user
  PUT    /users/123       → replace user
  PATCH  /users/123       → partial update
  DELETE /users/123       → remove user
```

### Key Rules
- **Stateless** — each request carries all context (no server-side session)
- **Resource URLs** — nouns, not verbs (`/orders`, not `/createOrder`)
- **Versioned** — always version APIs (`/v1/users`) to avoid breaking clients

### HTTP Status Codes

| Code | Meaning |
|---|---|
| 200 | OK |
| 201 | Created |
| 400 | Bad Request (client error) |
| 401 | Unauthorized (no/bad token) |
| 403 | Forbidden (no permission) |
| 404 | Not Found |
| 429 | Too Many Requests (rate limited) |
| 500 | Internal Server Error |
| 503 | Service Unavailable |

---

## gRPC (Google Remote Procedure Call)

High-performance framework using **Protocol Buffers (binary)** over **HTTP/2**.

Define a contract in a `.proto` file → auto-generate client/server code in any language.

```protobuf
service UserService {
  rpc GetUser (GetUserRequest) returns (UserResponse);
}
message GetUserRequest { int32 user_id = 1; }
message UserResponse   { int32 id = 1; string name = 2; }
```

### Streaming Types

```
  Unary:               Client ──req──► Server ──res──► Client
  Server Streaming:    Client ──req──► Server ──res1/res2/res3──► Client
  Client Streaming:    Client ──chunk1/chunk2──► Server ──res──► Client
  Bidirectional:       Client ◄────────────────────────────────► Server
```

---

## REST vs gRPC

| Feature | REST | gRPC |
|---|---|---|
| Protocol | HTTP/1.1 | HTTP/2 |
| Format | JSON (text) | Protobuf (binary) |
| Performance | Moderate | 5–10x faster |
| Schema | None (informal) | Strict `.proto` |
| Streaming | No | Yes (4 modes) |
| Browser | Native | Needs proxy |
| Debugging | Easy (readable) | Harder (binary) |
| Best for | Public APIs | Internal microservices |

## GraphQL (Brief)

Client asks for **exactly** the fields it needs — solves REST's over/under-fetching.

| | REST | GraphQL | gRPC |
|---|---|---|---|
| Best for | Public APIs | Client-driven UIs | Internal services |
| Over-fetching | Common | Solved | N/A |
| Browser support | Native | Native | Needs proxy |

---

## When to Use

**REST** — public API, browser clients, simple CRUD, small teams.

**gRPC** — internal microservices, performance-critical, streaming, polyglot teams.

**GraphQL** — mobile/web clients needing flexible, precise queries.

> Most large systems use all three: REST/GraphQL externally, gRPC internally.
