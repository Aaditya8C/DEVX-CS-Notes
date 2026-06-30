# Spring Boot — REST CRUD APIs

This chapter builds a complete REST API for an Employee resource, progressively improving the architecture across four projects: raw JPA with a service layer → PATCH with Jackson JsonMapper → Spring Data JPA repository → Spring Data REST (zero-controller approach).

---

# 01-spring-boot-rest-crud-employee

## Concept
A full REST CRUD API for `Employee` using the **Controller → Service → DAO → EntityManager** layered architecture. The service layer owns `@Transactional` so the DAO stays clean. Jackson automatically converts `Employee` objects to/from JSON.

## Why We Use It
Proper layered architecture separates concerns: controller handles HTTP, service handles business logic + transactions, DAO handles DB access. This makes the code testable and maintainable.

## Important Annotations / Classes
- `@RestController` + `@RequestMapping("/api")` → base REST controller
- `@GetMapping`, `@PostMapping`, `@PutMapping`, `@DeleteMapping` → HTTP method mapping
- `@PathVariable` → extracts value from the URL path (e.g., `/employees/{id}`)
- `@RequestBody` → deserializes the JSON request body into a Java object
- `@Service` → marks the service layer bean
- `@Repository` → marks the DAO bean
- `@Transactional` → placed on service methods that write to DB
- `EntityManager.merge()` → INSERT if id=0, UPDATE if id exists

## Flow / Working
```
HTTP Request
     ↓
EmployeeRestController  (@RestController)
     ↓
EmployeeService         (@Service + @Transactional)
     ↓
EmployeeDAOJpaImpl      (@Repository)
     ↓
EntityManager           (JPA / Hibernate)
     ↓
MySQL Database
```

1. Client sends request (e.g., `GET /api/employees`).
2. Controller calls `employeeService.findAll()`.
3. Service delegates to `employeeDAO.findAll()`.
4. DAO runs JPQL: `entityManager.createQuery("from Employee", Employee.class)`.
5. Result returned up the chain; Jackson serializes to JSON.

**For POST (add new):** Controller sets `id=0` before calling `save()` to force INSERT.

## Critical Code Snapshot
```java
// Controller
@RestController
@RequestMapping("/api")
public class EmployeeRestController {

    private EmployeeService employeeService;

    @Autowired
    public EmployeeRestController(EmployeeService theEmployeeService) {
        employeeService = theEmployeeService;
    }

    @GetMapping("/employees")
    public List<Employee> findAll() {
        return employeeService.findAll();
    }

    @PostMapping("/employees")
    public Employee addEmployee(@RequestBody Employee theEmployee) {
        theEmployee.setId(0);  // force INSERT, not UPDATE
        return employeeService.save(theEmployee);
    }

    @PutMapping("/employees")
    public Employee updateEmployee(@RequestBody Employee theEmployee) {
        return employeeService.save(theEmployee);
    }

    @DeleteMapping("/employees/{employeeId}")
    public void deleteEmployee(@PathVariable int employeeId) {
        employeeService.deleteById(employeeId);
    }
}

// Service — owns @Transactional
@Service
public class EmployeeServiceImpl implements EmployeeService {
    @Transactional
    @Override
    public Employee save(Employee employee) {
        return employeeDAO.save(employee);
    }
}

// DAO — @Transactional NOT placed here (delegated to service)
@Repository
public class EmployeeDAOJpaImpl implements EmployeeDAO {
    @Override
    public Employee save(Employee employee) {
        return entityManager.merge(employee);
        // id=0  → INSERT; id>0 → UPDATE
    }
}
```

> **Interview Notes:**
> - Why `setId(0)` before save on POST? `entityManager.merge()` checks the ID: 0 or null = INSERT, existing = UPDATE. Prevents client from overwriting an existing record by sending a fake ID.
> - `@Transactional` on service (not DAO) is a best practice — service layer is the logical transaction boundary.

---

# 02-spring-boot-json-mapper-patch-req

## Concept
Extends the Employee CRUD API with a `PATCH` endpoint for **partial updates**. Unlike `PUT` (which replaces the whole object), `PATCH` updates only the fields provided in the request body. This is implemented using Jackson's `JsonMapper.updateValue()` which merges a `Map<String, Object>` payload onto an existing object.

## Why We Use It
`PUT` requires sending the complete object even if only one field changed. `PATCH` lets the client send only the fields to update, which is more efficient and less error-prone.

## Important Annotations / Classes
- `@PatchMapping("/employees/{id}")` → maps HTTP PATCH requests
- `JsonMapper` → Jackson class; `updateValue(target, patchMap)` merges map fields into the target object
- `Map<String, Object>` → holds the partial update payload from the request body
- `@RequestBody Map<String, Object>` → parses the incoming JSON as a generic map

## Flow / Working
1. Client sends `PATCH /api/employees/3` with body `{ "email": "new@email.com" }`.
2. Controller reads `employeeId` from the path and the partial payload as a `Map`.
3. Fetches the existing `Employee` from DB by ID.
4. Validates that `"id"` is not in the payload (prevent ID tampering).
5. `jsonMapper.updateValue(tempEmployee, patchPayload)` — applies only the provided fields.
6. Saves the merged employee back to DB.

## Critical Code Snapshot
```java
@PatchMapping("/employees/{employeeId}")
public Employee patchEmployee(@PathVariable int employeeId,
                              @RequestBody Map<String, Object> patchPayload) {

    Employee tempEmployee = employeeService.findById(employeeId);
    if (tempEmployee == null) throw new RuntimeException("Employee not found");

    // Prevent ID from being changed via PATCH
    if (patchPayload.containsKey("id")) {
        throw new RuntimeException("Employee id not allowed in request body");
    }

    // Merge only provided fields into the existing Employee object
    Employee patchedEmployee = jsonMapper.updateValue(tempEmployee, patchPayload);
    return employeeService.save(patchedEmployee);
}
```

> **Interview Notes:**
> - `PUT` = full replacement; `PATCH` = partial update. Use PATCH when updating a subset of fields.
> - `JsonMapper.updateValue(target, source)` is the key — it overlays only the keys present in the map, leaving other fields unchanged.
> - Always guard against ID injection in the PATCH body — clients should not change the primary key.

---

# 03-spring-boot-rest-crud-employee-with-spring-data-jpa

## Concept
Replaces the manual `EmployeeDAO` + `EntityManager` implementation with **Spring Data JPA**'s `JpaRepository`. By extending `JpaRepository<Employee, Integer>`, you get all CRUD methods for free — no implementation class needed.

## Why We Use It
Eliminates boilerplate DAO code. Spring Data JPA auto-generates the implementation at runtime. Less code, same functionality.

## Important Annotations / Classes
- `JpaRepository<Entity, ID>` → provides `findAll()`, `findById()`, `save()`, `deleteById()` out of the box
- `Optional<T>` → returned by `findById()`, use `.isPresent()` / `.get()` to handle null safely
- No `@Transactional` needed in service for `save()` and `deleteById()` — Spring Data JPA handles it internally

## Flow / Working
```
Controller → Service → EmployeeRepository (extends JpaRepository) → DB
```
1. `EmployeeRepository` is just an interface — no implementation class.
2. Spring Data JPA generates a proxy implementation at runtime.
3. Service calls `employeeRepository.findAll()`, `save()`, `deleteById()` directly.
4. `findById()` returns `Optional<Employee>` — service unwraps it and throws if empty.

## Critical Code Snapshot
```java
// Repository — just an interface, no implementation needed
public interface EmployeeRepository extends JpaRepository<Employee, Integer> {
    // findAll(), findById(), save(), deleteById() are all inherited
}

// Service using the repository
@Service
public class EmployeeServiceImpl implements EmployeeService {

    private EmployeeRepository employeeRepository;

    @Override
    public Employee findById(int id) {
        Optional<Employee> result = employeeRepository.findById(id);
        if (result.isPresent()) {
            return result.get();
        } else {
            throw new RuntimeException("Employee not found");
        }
    }

    @Override
    public Employee save(Employee employee) {
        return employeeRepository.save(employee);
        // save() does INSERT if new, UPDATE if existing — no need to set id=0
    }

    @Override
    public void deleteById(int id) {
        employeeRepository.deleteById(id);
    }
}
```

> **Interview Notes:**
> - `JpaRepository` vs `CrudRepository`: `JpaRepository` extends `CrudRepository` + `PagingAndSortingRepository`. Prefer `JpaRepository` for extra features.
> - Spring Data's `save()` uses `EntityManager.persist()` for new entities and `merge()` for existing. Detection is based on whether the `@Id` field is null/0.
> - You can add custom query methods by just declaring the method signature: `List<Employee> findByLastName(String name)` — no SQL needed.

---

# 04-spring-boot-rest-crud-employee-with-spring-data-rest

## Concept
The most minimal approach — **Spring Data REST** automatically exposes a HATEOAS REST API by scanning `JpaRepository` interfaces. No controller, no service, no DAO implementation class needed. You just have the Entity, the Repository interface, and the dependency — Spring Data REST does the rest.

## Why We Use It
For rapid prototyping or simple CRUD APIs, you can skip writing the controller entirely. Spring Data REST generates all endpoints automatically at runtime.

## Important Annotations / Classes
- `spring-boot-starter-data-rest` dependency → enables Spring Data REST auto-configuration
- `JpaRepository` → Spring Data REST detects this and auto-generates HATEOAS endpoints
- `@RepositoryRestResource(path="members")` → (optional) customizes the URL path for the resource
- Endpoints follow **HATEOAS** format with `_links` in the response

## Flow / Working
1. Add `spring-boot-starter-data-rest` to `pom.xml`.
2. Spring Data REST scans for `JpaRepository` interfaces.
3. Auto-generates REST endpoints at startup:
   - `GET /employees` → list all
   - `GET /employees/{id}` → get by ID
   - `POST /employees` → create
   - `PUT /employees/{id}` → update
   - `DELETE /employees/{id}` → delete
4. No controller or service class required.

## Critical Code Snapshot
```java
// Repository — the ONLY class you need to write
public interface EmployeeRepository extends JpaRepository<Employee, Integer> {
    // Spring Data REST auto-generates all CRUD endpoints from this
}
```

**Auto-generated response format (HATEOAS):**
```json
{
  "_embedded": {
    "employees": [
      { "firstName": "Aadi", "lastName": "Padte", ... }
    ]
  },
  "_links": {
    "self": { "href": "http://localhost:8080/employees" }
  }
}
```

**Custom path (optional):**
```java
@RepositoryRestResource(path = "members")
public interface EmployeeRepository extends JpaRepository<Employee, Integer> { }
// Now accessible at /members instead of /employees
```

> **Interview Notes:**
> - Spring Data REST uses the **entity class name** (lowercased, pluralized) as the default URL path: `Employee` → `/employees`.
> - The response format is **HAL (Hypertext Application Language)** — a subset of HATEOAS.
> - Not suitable for production APIs that need custom business logic — use the Controller + Service approach instead.
> - Pagination and sorting are built-in: `GET /employees?page=0&size=10&sort=lastName,asc`.
