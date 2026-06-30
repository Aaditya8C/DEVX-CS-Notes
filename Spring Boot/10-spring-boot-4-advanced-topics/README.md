# Spring Boot 4 & 3.x Advanced Topics

This chapter covers the latest advanced features of modern Spring Boot (transitioning from Spring Boot 3.x to 4.x) and details production-grade implementation strategies for REST API versioning.

---

## 1. Advanced Features in Modern Spring Boot (3.x / 4.x)

Modern Spring Boot focuses heavily on scalability, cloud-native deployments, performance optimizations, and developer efficiency.

### A. Virtual Threads (Project Loom)
Traditionally, Spring MVC uses a **Thread-per-Request** model (using Tomcat's platform thread pool). If a thread blocks on an I/O operation (e.g., database query or external API call), that OS-level thread is idle but still consumes system memory (~1MB per thread).

**Virtual Threads** are lightweight threads managed by the JVM rather than the operating system. They allow running millions of concurrent tasks on a limited number of OS threads. When a virtual thread blocks on I/O, the JVM suspends it, frees up the underlying OS thread, and runs another task on it.

```mermaid
graph TD
    subgraph Tomcat Thread Pool (Traditional Platform Threads)
        T1[OS Thread 1] -->|Blocked on DB| R1(Request 1)
        T2[OS Thread 2] -->|Blocked on External API| R2(Request 2)
        T3[OS Thread 3] -->|Blocked on File I/O| R3(Request 3)
    end
    
    subgraph Project Loom (Virtual Threads)
        O1[Carrier OS Thread 1] -.-> VT1[Virtual Thread 1]
        O1 -.-> VT2[Virtual Thread 2]
        O1 -.-> VT3[Virtual Thread 3]
        VT1 -->|Blocks| JVM[JVM Suspends VT1 & runs VT2 on Carrier OS Thread]
    end
```

#### How to Enable Virtual Threads
From Spring Boot 3.2+, you can enable virtual threads for web servers and task execution with a single configuration property:
```properties
spring.threads.virtual.enabled=true
```
No code changes are required. Tomcat, scheduled tasks, and asynchronous tasks will automatically use virtual threads.

---

### B. GraalVM Native Image & AOT Compilation
Normally, the JVM compiles bytecode to machine code at runtime (JIT - Just-In-Time compilation). This causes slow startup times and high memory consumption.

**GraalVM Ahead-of-Time (AOT) compilation** compiles the application directly into a standalone platform-specific binary executable.
* **Pros**:
  * Startup times drop from seconds to milliseconds (e.g., 0.05s).
  * Extremely low memory usage (no JIT compiler overhead).
  * Perfect for Serverless (AWS Lambda, Google Cloud Run) and Kubernetes container environments.
* **Cons**:
  * Build times are very slow and resource-intensive.
  * Lacks runtime JIT optimizations.
  * Reflection, dynamic proxies, and runtime classpath inspection require explicit static metadata configuration.

#### How to Build a Native Image
```powershell
# Build a native container image using Cloud Native Buildpacks (Docker required)
./mvnw spring-boot:build-image -Pnative

# Or compile to a local native binary
./mvnw -Pnative native:compile
```

---

### C. Fluent REST Clients (`RestClient` & HTTP Interfaces)

#### 1. `RestClient` (Synchronous Fluent Client)
Introduced in Spring Boot 3.2 to offer a modern, fluent alternative to `RestTemplate` without requiring the reactive dependency of `WebClient`.

```java
@Service
public class UserService {
    private final RestClient restClient;

    public UserService(RestClient.Builder builder) {
        this.restClient = builder.baseUrl("https://api.example.com").build();
    }

    public User getUserById(Long id) {
        return restClient.get()
                .uri("/users/{id}", id)
                .accept(MediaType.APPLICATION_JSON)
                .retrieve()
                .body(User.class);
    }
}
```

#### 2. Declarative HTTP Clients
Define an interface, annotate it, and let Spring generate the implementation proxy.

```java
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.service.annotation.GetExchange;
import org.springframework.web.service.annotation.HttpExchange;

@HttpExchange("/users")
public interface UserClient {

    @GetExchange("/{id}")
    User getUserById(@PathVariable("id") Long id);
}
```
Define a bean to wire the client:
```java
@Configuration
public class ClientConfig {
    @Bean
    public UserClient userClient(RestClient.Builder builder) {
        RestClient client = builder.baseUrl("https://api.example.com").build();
        RestClientAdapter adapter = RestClientAdapter.create(client);
        HttpServiceProxyFactory factory = HttpServiceProxyFactory.builderFor(adapter).build();
        return factory.createClient(UserClient.class);
    }
}
```

---

### D. ConnectionDetails Abstraction
`ConnectionDetails` decouples configuration properties from external service connections. Before, Spring Boot relied on specific properties (like `spring.datasource.url`). Now, configuration values can be supplied dynamically.
This allows seamless, zero-config integration with tools like **Spring Boot Docker Compose Support** and **Testcontainers** at development/test time.

---

### E. SSL Bundles
Centralizes SSL trust material and key stores into a single configuration block under `spring.ssl.bundle`.
* Easily share trust stores between HTTP servers and clients.
* Reload certificates **without restarting** the application (Hot-reloading).

---

### F. Class Data Sharing (CDS)
Allows caching metadata of JVM classes during build or first boot, dramatically reducing JVM startup times and footprint on future container starts.

---
---

## 2. Practical Guide to API Versioning

API versioning is crucial for maintaining backward compatibility as your system evolves over time. There are four primary strategies for versioning REST APIs in Spring Boot.

### 1. URI Path Versioning
The version is explicitly placed in the URL path.
* **Example**: `http://localhost:8080/api/v1/accounts` vs. `http://localhost:8080/api/v2/accounts`

#### Code Implementation
```java
@RestController
@RequestMapping("/api/v1/accounts")
public class AccountControllerV1 {

    @GetMapping
    public List<AccountV1> getAccounts() {
        return List.of(new AccountV1("Tesla"), new AccountV1("SpaceX"));
    }
}

@RestController
@RequestMapping("/api/v2/accounts")
public class AccountControllerV2 {

    @GetMapping
    public List<AccountV2> getAccounts() {
        // V2 includes extra details like 'status' and 'type'
        return List.of(new AccountV2("Tesla", "ACTIVE", "ENTERPRISE"), 
                       new AccountV2("SpaceX", "ACTIVE", "GOVERNMENT"));
    }
}
```

* **Pros**: Highly visible, easy to test in browsers, simple caching routing.
* **Cons**: Clutters the URI space (URI should represent resource identity, not schema state).

---

### 2. Request Parameter Versioning
The client passes a query parameter to select the version.
* **Example**: `http://localhost:8080/api/accounts?version=1` vs. `http://localhost:8080/api/accounts?version=2`

#### Code Implementation
```java
@RestController
@RequestMapping("/api/accounts")
public class AccountParamController {

    // Matches /api/accounts?version=1
    @GetMapping(params = "version=1")
    public List<AccountV1> getAccountsV1() {
        return List.of(new AccountV1("Tesla"));
    }

    // Matches /api/accounts?version=2
    @GetMapping(params = "version=2")
    public List<AccountV2> getAccountsV2() {
        return List.of(new AccountV2("Tesla", "ACTIVE", "ENTERPRISE"));
    }
}
```

* **Pros**: Keeps the URI resource path clean.
* **Cons**: Query parameters are easily stripped by intermediary systems, harder to cache cleanly.

---

### 3. Custom Headers Versioning
The client passes a custom header containing the version.
* **Example**: `http://localhost:8080/api/accounts` with Header `X-API-VERSION: 1` vs. `X-API-VERSION: 2`

#### Code Implementation
```java
@RestController
@RequestMapping("/api/accounts")
public class AccountHeaderController {

    // Matches request if Header 'X-API-VERSION' is '1'
    @GetMapping(headers = "X-API-VERSION=1")
    public List<AccountV1> getAccountsV1() {
        return List.of(new AccountV1("Tesla"));
    }

    // Matches request if Header 'X-API-VERSION' is '2'
    @GetMapping(headers = "X-API-VERSION=2")
    public List<AccountV2> getAccountsV2() {
        return List.of(new AccountV2("Tesla", "ACTIVE", "ENTERPRISE"));
    }
}
```

* **Pros**: Keeps the URI path clean; follows standard HTTP styling guidelines.
* **Cons**: Cannot test directly in web browsers (requires Postman or curl); requires header-based routing in reverse proxies.

---

### 4. Media Type (Content Negotiation) Versioning
Also known as the **Accept Header** or **MIME-Type** versioning. The client uses the standard `Accept` header to negotiate the exact schema representation version.
* **Example**: `http://localhost:8080/api/accounts` with Header `Accept: application/vnd.company.app-v1+json` vs. `Accept: application/vnd.company.app-v2+json`

#### Code Implementation
```java
@RestController
@RequestMapping("/api/accounts")
public class AccountMediaTypeController {

    // Matches if Accept Header is 'application/vnd.company.app-v1+json'
    @GetMapping(produces = "application/vnd.company.app-v1+json")
    public List<AccountV1> getAccountsV1() {
        return List.of(new AccountV1("Tesla"));
    }

    // Matches if Accept Header is 'application/vnd.company.app-v2+json'
    @GetMapping(produces = "application/vnd.company.app-v2+json")
    public List<AccountV2> getAccountsV2() {
        return List.of(new AccountV2("Tesla", "ACTIVE", "ENTERPRISE"));
    }
}
```

* **Pros**: The most REST-compliant approach (implements Content Negotiation); keeps URIs clean and maintains strict resource naming rules.
* **Cons**: Complicated to debug; hard to verify in browsers without browser extensions or terminal utilities.

---

## 3. Comparison & Decision Matrix

| Strategy | URI Cleanliness | Caching Friendliness | Browser Viewable | REST Compliance |
| :--- | :--- | :--- | :--- | :--- |
| **URI Path** | ❌ No (Dirty) | ✅ Excellent | ✅ Yes | ❌ Poor |
| **Request Param** | ⚠️ Moderate | ⚠️ Moderate | ✅ Yes | ❌ Poor |
| **Custom Header** | ✅ Yes (Clean) | ❌ Complex (requires Vary header) | ❌ No | ⚠️ Moderate |
| **Media Type (Accept)**| ✅ Yes (Clean) | ❌ Complex (requires Vary header) | ❌ No | ✅ Perfect |

> [!TIP]
> **Which one should you choose?**
> * **URI Versioning** is the industry standard (used by Stripe, Google, Twitter). If you need simplicity, easy caching, and frictionless client usage, use URI versioning.
> * **Media Type / Accept Header** versioning is the favorite of REST purists (used by GitHub). Choose this if you are building enterprise platforms where resources strictly represent data entities, and you want to maintain long-term compliance with content negotiation standards.
