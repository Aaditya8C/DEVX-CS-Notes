# Spring Boot Overview

This chapter covers the foundational building blocks of Spring Boot — how the framework starts up, what tools help during development, how to expose endpoints via Actuator, and how to inject external configuration values through `application.properties`.

---

# 01-sb-demo

## Concept
The very first "Hello World" Spring Boot project. It demonstrates how Spring Boot auto-configures an embedded Tomcat server, bootstraps the application context, and exposes a REST endpoint with minimal setup.

## Why We Use It
Spring Boot removes all boilerplate XML configuration and server setup. You write a class, add one annotation, and the app runs.

## Important Annotations / Classes
- `@SpringBootApplication` → meta-annotation that enables auto-configuration, component scanning, and bean registration
- `@RestController` → marks a class as a REST controller; responses are written directly to the HTTP body
- `@GetMapping("/")` → maps GET HTTP requests to a handler method
- `SpringApplication.run()` → bootstraps and starts the Spring context

## Flow / Working
1. `main()` calls `SpringApplication.run()`.
2. Spring Boot auto-configures an embedded Tomcat server on port 8080.
3. Component scan picks up `@RestController` and registers `DemoRestController` as a bean.
4. On `GET /`, Spring calls `sayHello()` and returns the string as a plain-text HTTP response.

## Critical Code Snapshot
```java
@SpringBootApplication
public class MyappApplication {
    public static void main(String[] args) {
        SpringApplication.run(MyappApplication.class, args);
    }
}

@RestController
public class DemoRestController {
    @GetMapping("/")
    public String sayHello() {
        return "Hello Aaditya";
    }
}
```

> **Interview Note:** `@SpringBootApplication` = `@Configuration` + `@EnableAutoConfiguration` + `@ComponentScan`. Spring Boot does NOT require a `web.xml` or external server.

---

# 02-dev-tools-actuator-spring-security-demo

## Concept
Demonstrates three productivity/observability tools:
- **Spring Boot DevTools** — auto-restarts the app on code changes during development.
- **Spring Boot Actuator** — exposes management/monitoring HTTP endpoints (`/actuator/health`, `/actuator/beans`, etc.).
- **Spring Security** — secures Actuator endpoints (basic auth applied automatically when security is on classpath).

## Why We Use It
- DevTools speeds up the dev cycle (no manual restart needed).
- Actuator gives visibility into the running app without writing extra code.
- Security protects sensitive endpoints from public access.

## Important Annotations / Classes
- `management.endpoints.web.exposure.include=*` → exposes all actuator endpoints
- `management.info.env.enabled=true` → enables `/actuator/info` to show custom app info
- `info.app.*` properties → values shown at `/actuator/info`

## Flow / Working
1. Add `spring-boot-devtools`, `spring-boot-starter-actuator`, `spring-boot-starter-security` dependencies.
2. On app start, Actuator auto-registers all management endpoints.
3. `GET /actuator/health` → returns `{"status":"UP"}`.
4. `GET /actuator/info` → returns custom info from `application.properties`.
5. Security auto-generates a password (printed in console) and prompts for basic auth.

## Critical Code Snapshot
```properties
# application.properties
management.endpoints.web.exposure.include=*
management.info.env.enabled=true

info.app.name=My Java AI App
info.app.description=It's an AI bot application
info.app.version=1.0.0
```

> **Interview Note:** By default Actuator only exposes `/health`. To expose all, use `include=*`. DevTools is **not** included in production builds — it is automatically disabled when the JAR is run as a fully packaged app.

---

# 03-property-injection-demo

## Concept
Shows how to read custom key-value pairs from `application.properties` into Spring beans using `@Value`. Also demonstrates how to configure the server port and context path from properties.

## Why We Use It
Externalizing config values (API keys, URLs, feature flags) via properties avoids hardcoding and makes the app environment-aware without code changes.

## Important Annotations / Classes
- `@Value("${property.key}")` → injects the value of a property key into a field
- `server.port` → changes the default port (8080)
- `server.servlet.context-path` → sets a URL prefix for all endpoints

## Flow / Working
1. Define custom keys in `application.properties` (e.g., `brand.name=Zerodha`).
2. Use `@Value("${brand.name}")` on a field in a Spring bean.
3. Spring resolves the value from properties at startup and injects it.
4. The field is ready to use in any handler method.

## Critical Code Snapshot
```properties
# application.properties
brand.name=Zerodha
stock.name=Tata

server.port=5050
server.servlet.context-path=/myapp
```

```java
@RestController
public class DemoRestController {

    @Value("${brand.name}")
    private String brandName;

    @Value("${stock.name}")
    private String stockName;

    @GetMapping("/tradeInfo")
    public String getTradeInfo() {
        return "Your Brand: " + brandName + " and stock is: " + stockName;
    }
}
```

> **Interview Note:** `@Value` works with any Spring-managed bean. The property key must match exactly (case-sensitive). If the key is missing, Spring throws `IllegalArgumentException` at startup — use `${key:defaultValue}` syntax for fallback defaults.
