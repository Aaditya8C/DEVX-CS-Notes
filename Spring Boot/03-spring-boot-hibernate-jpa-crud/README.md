# Spring Boot — Hibernate / JPA + Jackson

This chapter introduces database access using JPA (Java Persistence API) with Hibernate as the ORM provider. It also covers how Jackson automatically serializes Java POJOs to JSON. Projects here demonstrate entity mapping, the DAO pattern, JPQL queries, and full CRUD operations against a MySQL database.

---

# 01-crud-demo

## Concept
This project demonstrates full CRUD (Create, Read, Update, Delete) database operations using JPA and Hibernate with the DAO (Data Access Object) pattern. Spring Boot auto-configures the `EntityManager` and transaction management from `application.properties`. The `EntityManager` is the primary JPA API for interacting with the persistence context.

## Why We Use It
JPA provides a standard, database-agnostic API for ORM. You write Java objects and queries — Hibernate translates them to SQL. You avoid raw JDBC boilerplate.

## Important Annotations / Classes
- `@Entity` → marks a class as a JPA entity (mapped to a DB table)
- `@Table(name="student")` → maps the entity to a specific table
- `@Id` → marks the primary key field
- `@GeneratedValue(strategy = GenerationType.IDENTITY)` → auto-increment ID from DB
- `@Column(name="first_name")` → maps a field to a specific column
- `@Repository` → marks the DAO class as a Spring bean and enables exception translation
- `@Transactional` → wraps the method in a DB transaction (required for INSERT/UPDATE/DELETE)
- `EntityManager` → JPA interface for all persistence operations
- `TypedQuery<T>` → type-safe JPQL query
- `CommandLineRunner` → runs code after the Spring context is loaded (used here to test DAO methods)

## Flow / Working
1. `application.properties` provides DB connection (MySQL URL, username, password).
2. Spring Boot auto-creates the `EntityManager` and datasource bean.
3. `StudentDAOImpl` gets the `EntityManager` injected via constructor.
4. Operations:
   - **Save**: `entityManager.persist(student)` — INSERT
   - **Read by ID**: `entityManager.find(Student.class, id)` — SELECT by PK
   - **Read all**: JPQL `FROM Student` via `TypedQuery` — SELECT all
   - **Query by field**: JPQL with named param `:theData`
   - **Update**: `entityManager.merge(student)` — UPDATE
   - **Delete**: `find` then `entityManager.remove(student)` — DELETE
   - **Delete all**: JPQL `DELETE from Student` via `executeUpdate()`
5. `CommandLineRunner` bean in main class invokes the DAO methods on startup.

## Critical Code Snapshot
```java
// Entity
@Entity
@Table(name = "student")
public class Student {
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    @Column(name = "id")
    private int id;

    @Column(name = "first_name")
    private String first_name;
    // ... other fields, getters, setters
}

// DAO Implementation
@Repository
public class StudentDAOImpl implements StudentDAO {

    private EntityManager entityManager;

    @Autowired
    public StudentDAOImpl(EntityManager entityManager) {
        this.entityManager = entityManager;
    }

    @Override
    @Transactional
    public void save(Student theStudent) {
        entityManager.persist(theStudent);
    }

    @Override
    public List<Student> findAll() {
        TypedQuery<Student> query = entityManager.createQuery("FROM Student", Student.class);
        return query.getResultList();
    }

    @Override
    public List<Student> findByLastName(String lastName) {
        TypedQuery<Student> query = entityManager.createQuery(
                "FROM Student WHERE last_name=:theData", Student.class);
        query.setParameter("theData", lastName);
        return query.getResultList();
    }

    @Override
    @Transactional
    public void update(Student theStudent) {
        entityManager.merge(theStudent);
    }

    @Override
    @Transactional
    public void delete(Integer id) {
        Student stud = entityManager.find(Student.class, id);
        entityManager.remove(stud);
    }
}
```

```properties
# application.properties
spring.datasource.url=jdbc:mysql://localhost:3306/student_tracker
spring.datasource.username=springstudent
spring.datasource.password=springstudent
```

> **Interview Notes:**
> - JPQL uses **entity class names and field names**, NOT table/column names.
> - `@Transactional` is required on write operations. Read-only queries work without it.
> - `entityManager.merge()` — if ID=0 or not found: INSERT; if ID exists: UPDATE.
> - `@Repository` enables Spring's `PersistenceExceptionTranslationPostProcessor` to convert JPA exceptions to Spring's `DataAccessException`.

---

# 02-pojo-jackson

## Concept
This project demonstrates how Jackson (the JSON library bundled with Spring Boot) automatically converts Java POJOs to JSON and back. When a `@RestController` method returns a Java object or a `List`, Jackson serializes it to JSON automatically. No extra configuration is needed.

## Why We Use It
REST APIs need to communicate in JSON. Jackson handles the conversion invisibly — you just return your Java objects and Spring+Jackson does the rest.

## Important Annotations / Classes
- `@RestController` → combines `@Controller` + `@ResponseBody`; return value is written as JSON to the HTTP response body
- `@RequestMapping("/api")` → base URL prefix for all methods in the controller
- `@GetMapping("/students")` → maps GET requests
- Jackson `ObjectMapper` → the underlying class doing serialization (works automatically in Spring Boot)

## Flow / Working
1. Client sends `GET /api/students`.
2. Spring routes the request to `getStudents()`.
3. The method creates a `List<Student>` (a plain Java object with getters/setters).
4. Spring's `HttpMessageConverter` detects Jackson on the classpath.
5. Jackson calls getters on each `Student` object and converts the list to a JSON array.
6. The JSON array is written to the HTTP response body.

## Critical Code Snapshot
```java
// Plain POJO — no annotations needed for Jackson to work
public class Student {
    private String firstName;
    private String lastName;

    // constructors, getters, setters required
    public String getFirstName() { return firstName; }
    public String getLastName() { return lastName; }
}

// Controller — returns POJO, Jackson auto-converts to JSON
@RestController
@RequestMapping("/api")
public class StudentRestController {

    @GetMapping("/students")
    public List<Student> getStudents() {
        List<Student> students = new ArrayList<>();
        students.add(new Student("Aadi", "Padte"));
        students.add(new Student("Mahesh", "Kale"));
        return students;  // Jackson converts this to JSON array
    }
}
```

**Response:**
```json
[
  { "firstName": "Aadi", "lastName": "Padte" },
  { "firstName": "Mahesh", "lastName": "Kale" }
]
```

> **Interview Notes:**
> - Jackson uses **getter method names** to determine JSON field names. `getFirstName()` → `"firstName"`.
> - If you need a different JSON field name, use `@JsonProperty("first_name")` on the getter.
> - Jackson requires either getters or public fields to serialize. Private fields with no getters = invisible to Jackson.
> - `@ResponseBody` is implicit in `@RestController` — you don't need to add it manually.
