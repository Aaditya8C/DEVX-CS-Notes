# Case Study: URL Shortener (TinyURL / Bitly)

---

## Requirements

**Functional**
- Given a long URL → generate a unique short URL
- Given a short URL → redirect to the original long URL

**Non-Functional**
- Very high availability — system must never be down
- Very low latency — redirects must be near-instant

---

## API Design

```
POST /create-uri
  Body: { long_uri: "https://..." }
  Response: 201 Created
  Body: { short_uri: "https://short.ly/aB3xK9z" }

GET /{short-uri}
  Response: 301 Permanent Redirect
  Header: Location: <original long URL>
```

> 301 (Permanent) — browser caches the redirect, reduces future load on server.
> Use 302 (Temporary) if you need analytics on every hit (browser won't cache).

---

## Schema

| Field | Type | Notes |
|---|---|---|
| short_uri | string (PK) | 7-char Base62 key |
| long_uri | string | Original URL |
| created_at | timestamp | Creation time |
| click_count | int | For analytics / caching decisions |

---

## Scale Estimation

```
  Assume: 1000 URLs created per second
  Per year: 1000 × 60 × 60 × 24 × 365 = ~31.5 billion URLs
```

**How long should the short URL be?**

```
  Allowed characters:
  a–z = 26,  A–Z = 26,  0–9 = 10  →  Total = 62 chars (Base62)

  6 chars: 62^6 = ~56 billion    (close, risky)
  7 chars: 62^7 = ~3.5 trillion  (>> 31.5 billion, safe)

  Use 7-character Base62 keys.
```

---

## Short URL Generation — How it Works

Each web server needs to generate unique short keys without collisions.

**Problem with naive approaches:**
- UUID/random — two servers may generate the same key (collision)
- Counter per server — servers need to coordinate to not reuse the same number

**Solution: ZooKeeper for Range Assignment**

ZooKeeper pre-assigns each web server a unique range of integers:

```
  Web Server 1 → range 0 to 1,000,000
  Web Server 2 → range 1,000,001 to 2,000,000
  Web Server 3 → range 2,000,001 to 3,000,000
  ...
```

Each server converts its next integer to a Base62 string:

```
  Integer 125  →  Base62 → "cb"
  Integer 1000 →  Base62 → "g8"

  (same idea as converting decimal to another number base)
```

- No two servers generate the same key (ranges are disjoint)
- No DB round-trip needed per generation — servers work independently
- When a server exhausts its range, it requests the next range from ZooKeeper

---

## POST /create-uri — Flow

```
  1. Client sends POST /create-uri with long_uri
  2. Load Balancer routes to a Web Server
  3. Web Server takes the next integer from its ZooKeeper range
     and converts it to a 7-char Base62 short key
  4. Web Server saves { short_uri, long_uri } to Database
  5. Web Server returns 201 Created with the short_uri
```

```
  Client ──► [Load Balancer] ──► [Web Servers]
                                      │   │
                              [ZooKeeper]  [Database]
                              (range assign)(store mapping)
```

---

## GET /{short-uri} — Flow

```
  1. Client sends GET /{short-uri}
  2. Load Balancer routes to a Web Server
  3. Web Server checks Distributed Cache (Redis) for the short_uri
     - Cache HIT  → return 301 Redirect to long_uri immediately
     - Cache MISS → query Database for long_uri
                  → store result in Cache
                  → return 301 Redirect
```

```
  Client ──► [Load Balancer] ──► [Web Servers]
                                      │   │
                             [Redis Cache] [Database]
                             (check first) (fallback)
```

---

## Database Choice

Use a **NoSQL key-value store** (e.g., DynamoDB, Cassandra):
- Access pattern is simple: lookup by `short_uri` — no joins needed
- Needs to scale to billions of records horizontally
- High write throughput (1000 writes/sec)

Shard by `short_uri` — every read/write hits exactly one shard.

---

## Caching Strategy

- Cache the most frequently accessed short URLs in **Redis**
- Use **LRU eviction** — evict least recently used URLs when cache is full
- Use **analytics (click_count)** to proactively warm cache for popular URLs

> Reads vastly outnumber writes in a URL shortener. Cache hit rate is the key metric.

---

## Additional Considerations

### Rate Limiting
- Limit URL creation per IP / API key to prevent abuse and DDoS
- Implemented at the Load Balancer or API Gateway layer

### Security — Unpredictable Keys
- Do not use sequential integers as the public short URL
  - Sequential: `aaaaaa1`, `aaaaaa2` → attacker can enumerate all URLs
- Always convert to Base62 (non-sequential appearance) or add a random salt before encoding
- This prevents attackers from guessing private/sensitive shortened URLs

### Analytics
- Increment `click_count` asynchronously (via a message queue) on every redirect
- Avoids write latency on the critical redirect path
- Use click counts to decide which URLs to pre-warm in cache

### URL Expiry (optional feature)
- Add `expires_at` field to schema
- Background job periodically deletes expired entries from DB and invalidates cache

### Custom Aliases (optional feature)
- Allow users to choose their own short key (e.g., `short.ly/myproduct`)
- Check if alias already exists before storing
- Store in same DB table

---

## Key Takeaways

- 7-char Base62 gives 3.5 trillion unique keys — sufficient for decades of scale
- ZooKeeper range assignment eliminates key collision across distributed servers
- Redirect flow is read-heavy — Redis cache is the critical performance lever
- Use 301 for caching (lower load), 302 for analytics (every hit tracked)
- Rate limit creation; randomise keys to prevent URL enumeration attacks
