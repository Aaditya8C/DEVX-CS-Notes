# Spring Core — Dependency Injection & Bean Management

This chapter covers the Spring IoC container — the engine behind every Spring application. Projects here explore all the ways Spring creates, wires, and manages beans: constructor injection, setter injection, component scanning, qualifiers, lazy initialization, scopes, lifecycle hooks, and manual bean configuration.

---

# 01-constructor-injection

## Concept
Constructor Injection is the **recommended** way to inject dependencies in Spring. Spring calls the class constructor and passes the required dependency as a parameter. The dependency is defined via an interface, and Spring figures out which implementation to inject.

## Why We Use It
- Makes dependencies explicit and immutable (field marked `final` is possible).
- Easier to unit test (just call the constructor manually).
- Spring recommends it over field injection.

## Important Annotations / Classes
- `@Component` → registers the class as a Spring bean; Spring detects it during component scan
- `@RestController` → marks the class as a bean and a REST endpoint handler
- `@Autowired` → tells Spring to inject the dependency (on constructors with single arg, can be omitted in Spring 4.3+)
- `@GetMapping` → maps a GET request to a method

## Flow / Working
1. `Coach` interface defines the contract (`getDailyWorkout()`).
2. `CricketCoach` implements `Coach` and is annotated `@Component` → Spring registers it as a bean.
3. `DemoController` declares `Coach myCoach` and gets it via the constructor annotated with `@Autowired`.
4. At startup, Spring sees one `Coach` bean, injects it into the constructor, and the controller is ready.

## Critical Code Snapshot
```java
@Component
public class CricketCoach implements Coach {
    @Override
    public String getDailyWorkout() {
        return "Practice batting daily for 20 mins.";
    }
}

@RestController
public class DemoController {
    private Coach myCoach;

    @Autowired
    public DemoController(Coach theCoach) {
        myCoach = theCoach;
    }

    @GetMapping("/dailyworkout")
    public String getDailyWorkout() {
        return myCoach.getDailyWorkout();
    }
}
```

> **Interview Note:** Always prefer constructor injection over field injection. Field injection (`@Autowired` directly on a field) makes the class harder to test and hides dependencies.

---

# 02-component-scanning

## Concept
Spring Boot auto-scans for `@Component`, `@Service`, `@Repository`, `@Controller` etc. in the package of the main class and all sub-packages. If your components live in a different package tree, you must declare additional scan paths explicitly.

## Why We Use It
Avoids manually registering every bean. Spring just finds and registers all annotated classes automatically.

## Important Annotations / Classes
- `@SpringBootApplication` → triggers component scanning starting from its package
- `scanBasePackages = {"com.aadi.springcoredemo", "com.aadi.util"}` → extend scanning to extra packages
- `@Component` → marks a class for auto-detection

## Flow / Working
1. `@SpringBootApplication` is on the main class (e.g., `com.aadi.springcoredemo`).
2. Spring scans `com.aadi.springcoredemo` and all sub-packages by default.
3. For classes outside this tree, add `scanBasePackages` to the annotation.
4. All found `@Component`-annotated classes are registered in the application context.

## Critical Code Snapshot
```java
// Default — scans com.aadi.springcoredemo and sub-packages automatically
@SpringBootApplication
public class SpringcoredemoApplication { ... }

// Custom scan paths when components are in different packages
@SpringBootApplication(
    scanBasePackages = {"com.aadi.springcoredemo", "com.aadi.util"}
)
public class SpringcoredemoApplication { ... }
```

> **Interview Note:** If a component is in a package **outside** the main class package and `scanBasePackages` is not set, Spring will NOT find it — the app will start but the bean won't exist and injection will fail.

---

# 03-setter-injection

## Concept
Setter Injection wires a dependency by calling a setter method annotated with `@Autowired`. Unlike constructor injection, the dependency is injected after the object is created.

## Why We Use It
Useful when the dependency is optional or needs to be changed after construction. Less preferred than constructor injection for required dependencies.

## Important Annotations / Classes
- `@Autowired` on a setter method → Spring calls this method after creating the bean and injects the dependency

## Flow / Working
1. Spring creates the `DemoController` object using the no-arg constructor.
2. Spring finds the `@Autowired`-annotated setter `setCoach()`.
3. Spring resolves the `Coach` bean and calls `setCoach(theCricketCoach)`.
4. `myCoach` is now set and ready for use.

## Critical Code Snapshot
```java
@RestController
public class DemoController {
    private Coach myCoach;

    @Autowired
    public void setCoach(Coach theCoach) {
        myCoach = theCoach;
    }

    @GetMapping("/dailyworkout")
    public String getDailyWorkout() {
        return myCoach.getDailyWorkout();
    }
}
```

> **Interview Note:** `@Autowired` can be placed on **any** method, not just setters. Spring will call that method and inject whatever parameters it can resolve. Constructor injection is preferred for required dependencies.

---

# 04-qualifiers

## Concept
When multiple beans implement the same interface, Spring doesn't know which one to inject and throws `NoUniqueBeanDefinitionException`. `@Qualifier` solves this by letting you specify exactly which bean you want by name.

## Why We Use It
Needed whenever there are multiple implementations of the same type registered as beans.

## Important Annotations / Classes
- `@Qualifier("beanId")` → specifies which bean to inject when multiple candidates exist
- Bean ID defaults to the class name with the first letter lowercased (e.g., `TrackCoach` → `"trackCoach"`)

## Flow / Working
1. Multiple classes (`CricketCoach`, `TennisCoach`, `TrackCoach`, `BaseBallCoach`) all implement `Coach` and are `@Component`.
2. Spring finds 4 candidates for the `Coach` type.
3. Without `@Qualifier`, Spring fails with ambiguity error.
4. `@Qualifier("trackCoach")` tells Spring: inject the `TrackCoach` bean specifically.

## Critical Code Snapshot
```java
@Autowired
public DemoController(@Qualifier("trackCoach") Coach theCoach) {
    myCoach = theCoach;
}
```

> **Interview Note:** The qualifier name is the bean ID, which by default is the class name with a lowercase first letter. You can also set a custom name: `@Component("mySpecialCoach")`.

---

# 05-lazy-initialization

## Concept
By default, Spring creates all beans **eagerly** at startup. With `@Lazy`, a bean is created only when it is first requested (on first use), not at app startup.

## Why We Use It
- Speeds up startup time if some beans are heavy and not always needed.
- Useful for beans that are rarely used or have slow initialization.

## Important Annotations / Classes
- `@Lazy` → delays bean creation until the bean is first needed
- `spring.main.lazy-initialization=true` → enables lazy init globally for all beans (in `application.properties`)

## Flow / Working
1. All `@Component` classes print to console in their constructor.
2. `TrackCoach` is annotated `@Lazy`.
3. On startup, `BaseBallCoach`, `CricketCoach`, `TennisCoach` constructors fire → logged.
4. `TrackCoach` constructor does NOT fire until something actually requests a `TrackCoach` bean.

## Critical Code Snapshot
```java
@Component
@Lazy
public class TrackCoach implements Coach {

    public TrackCoach() {
        System.out.println("In constructor: " + getClass().getSimpleName());
    }

    @Override
    public String getDailyWorkout() {
        return "Run hard for 10 mins daily";
    }
}
```

> **Interview Note:** Global lazy init (`spring.main.lazy-initialization=true`) can hide configuration errors — a missing bean won't fail at startup, it'll fail only when the endpoint is first called. Use with care in production.

---

# 06-bean-scopes

## Concept
Bean scope defines how many instances of a bean Spring creates and how long they live. The default scope is **Singleton** (one shared instance for the entire app context). **Prototype** creates a **new instance every time** the bean is requested.

## Why We Use It
- **Singleton** → shared, stateless services (the default, most common).
- **Prototype** → stateful beans that should not be shared across requests.

## Important Annotations / Classes
- `@Scope(ConfigurableBeanFactory.SCOPE_SINGLETON)` → default (one instance, shared)
- `@Scope(ConfigurableBeanFactory.SCOPE_PROTOTYPE)` → new instance per injection
- `ConfigurableBeanFactory` → provides scope constants

## Flow / Working
1. `CricketCoach` is annotated with `@Scope(SCOPE_PROTOTYPE)`.
2. `DemoController` injects `CricketCoach` twice (as `myCoach` and `anotherCoach`).
3. Since scope is prototype, two separate instances are created.
4. `GET /check` compares `myCoach == anotherCoach` → returns `false` (different instances).
5. With singleton, the same check would return `true`.

## Critical Code Snapshot
```java
@Component
@Scope(ConfigurableBeanFactory.SCOPE_PROTOTYPE)
public class CricketCoach implements Coach { ... }

// In controller — verifying scope
@GetMapping("/check")
public String checkScope() {
    return "myCoach == anotherCoach: " + (myCoach == anotherCoach);
    // false → prototype (two instances)
    // true  → singleton (same instance)
}
```

> **Interview Note:** Web-aware scopes: `request` (one per HTTP request), `session` (one per HTTP session), `application` (one per ServletContext). These are only available in web apps.

---

# 07-bean-lifecycle-methods

## Concept
Spring lets you hook into the bean lifecycle — run custom code right after a bean is created (init) or just before it is destroyed (destroy). This is done using `@PostConstruct` and `@PreDestroy`.

## Why We Use It
- Init: load data, open connections, start background threads.
- Destroy: close connections, release resources, flush caches.

## Important Annotations / Classes
- `@PostConstruct` → method runs once, immediately after the bean is created and dependencies are injected
- `@PreDestroy` → method runs once, just before the Spring context is closed/bean destroyed

## Flow / Working
1. Spring creates `CricketCoach` and injects its dependencies.
2. `@PostConstruct` method `doMyStartupStuff()` is called automatically.
3. App runs normally.
4. On shutdown (context close), Spring calls `@PreDestroy` method `doMyCleanupStuff()`.

## Critical Code Snapshot
```java
@Component
public class CricketCoach implements Coach {

    @PostConstruct
    public void doMyStartupStuff() {
        System.out.println("In doMyStartupStuff(): " + getClass().getSimpleName());
    }

    @PreDestroy
    public void doMyCleanupStuff() {
        System.out.println("In doMyCleanupStuff(): " + getClass().getSimpleName());
    }
}
```

> **Interview Note:** `@PreDestroy` is **NOT called** for prototype-scoped beans — Spring does not manage the full lifecycle of prototype beans after creation. For prototype beans, you must handle cleanup manually.

---

# 08-config-bean

## Concept
`@Bean` allows you to manually define a Spring bean inside a `@Configuration` class. This is the Java-based alternative to `@Component` — useful for third-party classes you cannot annotate directly.

## Why We Use It
- The class you want to register as a bean is from a third-party library (can't add `@Component` to it).
- You need fine-grained control over how the bean is created.

## Important Annotations / Classes
- `@Configuration` → marks the class as a source of bean definitions
- `@Bean` → marks a method whose return value is registered as a Spring bean
- Bean ID defaults to the **method name** unless you specify `@Bean("customName")`

## Flow / Working
1. `SwimCoach` has no `@Component` — it's a plain class.
2. `SportConfig` is annotated `@Configuration`.
3. Inside it, `@Bean` method `swimCoach()` creates and returns a `new SwimCoach()`.
4. Spring registers this as a bean with ID `"swimCoach"` (method name).
5. `DemoController` injects it using `@Qualifier("swimCoach")`.

## Critical Code Snapshot
```java
// Plain class — no @Component
public class SwimCoach implements Coach {
    public String getDailyWorkout() {
        return "Swim 1000 meters as a warm up";
    }
}

// Config class — manually registers SwimCoach as a bean
@Configuration
public class SportConfig {
    @Bean
    public Coach swimCoach() {  // bean ID = "swimCoach"
        return new SwimCoach();
    }
}

// Inject by qualifier
@Autowired
public DemoController(@Qualifier("swimCoach") Coach theCoach) {
    myCoach = theCoach;
}
```

> **Interview Note:** `@Bean` methods in `@Configuration` classes are intercepted by Spring (via CGLIB proxy) — calling `swimCoach()` multiple times still returns the same singleton bean. This is NOT the case in a `@Component` class.
