# JPA / Hibernate Advanced Mappings

This chapter covers entity relationships in JPA and Hibernate, showcasing one-to-one, one-to-many, and many-to-many mappings. It details unidirectional and bidirectional relationships, cascading operations, fetch types (Eager vs Lazy), the N+1 select problem, and join tables.

---

# 01-one-to-one-mapping

## Concept
A one-to-one relationship maps a single record in one entity to exactly one record in another entity (e.g., `Instructor` has one `InstructorDetail`). It can be unidirectional (configured only on the owning side) or bidirectional (configured on both sides).

## Why We Use It
- Keeps entities clean and normalized (e.g., separating primary instructor data from personal details).
- Cascades save/delete operations so that saving or deleting an Instructor automatically saves/deletes their associated details.

## Important Annotations / Classes
- `@OneToOne(cascade = CascadeType.ALL)` → defines the relationship and propagates all lifecycle actions (PERSIST, REMOVE, etc.) to the target entity
- `@JoinColumn(name = "instructor_detail_id")` → specifies the foreign key column on the owning side
- `mappedBy = "instructorDetail"` → placed on the non-owning side to make the relationship bidirectional (points to the property in the owning class)

## Flow / Working
1. **Saving (Cascade)**: Setting `instructor.setInstructorDetail(detail)` and calling `entityManager.persist(instructor)` saves both entities in their respective tables, setting the foreign key on `instructor`.
2. **Retrieving (Bi-directional)**: Finding `InstructorDetail` by ID also allows navigating back to the associated `Instructor` via `detail.getInstructor()`.
3. **Deleting**: With `CascadeType.ALL`, removing the instructor automatically removes the associated details in the database.

## Critical Code Snapshot
```java
// Owning Side
@Entity
public class Instructor {
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private int id;

    @OneToOne(cascade = CascadeType.ALL)
    @JoinColumn(name = "instructor_detail_id")
    private InstructorDetail instructorDetail;
}

// Bi-directional Non-owning Side
@Entity
public class InstructorDetail {
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private int id;

    @OneToOne(mappedBy = "instructorDetail", cascade = CascadeType.ALL)
    private Instructor instructor;
}
```

> **Interview Note:** To delete `InstructorDetail` *without* deleting `Instructor`, you must exclude `CascadeType.REMOVE` (e.g. use `cascade = {CascadeType.DETACH, CascadeType.MERGE, CascadeType.PERSIST, CascadeType.REFRESH}`) and break the bidirectional link in your Java code (`instructorDetail.getInstructor().setInstructorDetail(null)`) before calling `entityManager.remove()`.

---

# 02-one-to-many-mapping

## Concept
A one-to-many bidirectional relationship maps a parent record to multiple child records (e.g., an `Instructor` teaches multiple `Course`s, and each `Course` belongs to one `Instructor`). It also explores Fetch types (Eager vs Lazy) and how to resolve lazy initialization exceptions.

## Why We Use It
- Models parent-child hierarchies where children need a back-reference.
- **Lazy Loading** is used to avoid fetching all associated courses from the database unless explicitly requested, improving performance.

## Important Annotations / Classes
- `@OneToMany(mappedBy = "instructor", fetch = FetchType.LAZY)` → declared on the parent list; defaults to Lazy loading
- `@ManyToOne` → declared on the child entity (owning side); defaults to Eager loading
- `JOIN FETCH` (JPQL) → runs a single query fetching the parent and all children to solve the N+1 query problem and resolve Lazy Loading outside transactions

## Flow / Working
1. **Adding Child**: The parent uses a convenience method (`add(Course)`) to add a course to its list and set the course's instructor reference.
2. **Lazy Loading**: Finding the `Instructor` only queries the `instructor` table. The courses are queried later when calling `instructor.getCourses()`.
3. **Join Fetching**: A query like `SELECT i FROM Instructor i JOIN FETCH i.courses WHERE i.id = :id` retrieves the instructor and their courses in a single database round-trip.

## Critical Code Snapshot
```java
// Parent Class (Instructor)
@Entity
public class Instructor {
    @OneToMany(mappedBy = "instructor", 
               fetch = FetchType.LAZY,
               cascade = {CascadeType.DETACH, CascadeType.MERGE, CascadeType.PERSIST, CascadeType.REFRESH})
    private List<Course> courses;

    // Convenience method
    public void add(Course tempCourse) {
        if (courses == null) { courses = new ArrayList<>(); }
        courses.add(tempCourse);
        tempCourse.setInstructor(this);
    }
}

// Child Class (Course)
@Entity
public class Course {
    @ManyToOne(cascade = {CascadeType.DETACH, CascadeType.MERGE, CascadeType.PERSIST, CascadeType.REFRESH})
    @JoinColumn(name = "instructor_id")
    private Instructor instructor;
}
```

> **Interview Note:** Eager loading retrieves everything immediately at startup/query time, which can cause high memory usage. Lazy loading is more efficient but requires an active Hibernate session (transaction) when accessing the collection. If accessed outside the session, a `LazyInitializationException` is thrown. Use `JOIN FETCH` to load lazy entities in a single step.

---

# 03-one-to-many-mapping-uni

## Concept
A unidirectional one-to-many mapping where the parent has a list of children, but the child has no reference back to the parent (e.g., `Course` has a list of `Review`s, but `Review` is unaware of `Course`).

## Why We Use It
- Simplifies the domain model when the child entity (like a review or comment) has no logical reason to know about its parent.
- Deleting the parent (`Course`) should cascade and delete all associated reviews.

## Important Annotations / Classes
- `@OneToMany(fetch = FetchType.LAZY, cascade = CascadeType.ALL)` → declared on the parent collection
- `@JoinColumn(name = "course_id")` → placed directly on the parent's collection field to specify the foreign key in the child table (`review`)

## Flow / Working
1. **No Back-Reference**: `Review` has no `Course` field.
2. **Database Join**: Hibernate adds a `course_id` column to the `review` table.
3. **Saving**: Saving the `Course` automatically saves all reviews in the collection and populates `course_id` on them.

## Critical Code Snapshot
```java
@Entity
public class Course {
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private int id;

    @OneToMany(fetch = FetchType.LAZY, cascade = CascadeType.ALL)
    @JoinColumn(name = "course_id") // places course_id FK in the review table
    private List<Review> reviews;
}
```

> **Interview Note:** In a unidirectional `@OneToMany` mapping, if you do not specify `@JoinColumn`, Hibernate will default to creating a join table (e.g., `course_reviews`) to link the two tables. This is often less performant than a simple foreign key in the child table. Always specify `@JoinColumn` for unidirectional one-to-many mappings.

---

# 04-many-to-many-mapping

## Concept
A many-to-many mapping links multiple records of one entity to multiple records of another entity (e.g., a `Student` can enroll in multiple `Course`s, and a `Course` can have multiple `Student`s). This requires a separate Join Table mapping the foreign keys of both tables.

## Why We Use It
- Handles complex M:N relationships.
- Cascading deletes are typically disabled so that deleting a Course does not delete its enrolled Students (or vice versa).

## Important Annotations / Classes
- `@ManyToMany` → defines the relationship
- `@JoinTable` → defines the intermediate table joining the two entities
- `joinColumns = @JoinColumn(name = "course_id")` → foreign key pointing to the owning side (`Course`)
- `inverseJoinColumns = @JoinColumn(name = "student_id")` → foreign key pointing to the target side (`Student`)

## Flow / Working
1. **Join Table Creation**: Hibernate manages a joint table called `course_student` with two columns: `course_id` and `student_id`.
2. **Association**: A student is added to a course via `course.addStudent(student)`.
3. **Saving**: Persisting the course writes records to `course` and `course_student` tables, but does not affect the existing data in the `student` table.

## Critical Code Snapshot
```java
// Owning Side (Course)
@Entity
public class Course {
    @ManyToMany(fetch = FetchType.LAZY, cascade = {CascadeType.DETACH, CascadeType.MERGE, CascadeType.PERSIST, CascadeType.REFRESH})
    @JoinTable(
            name = "course_student",
            joinColumns = @JoinColumn(name = "course_id"),
            inverseJoinColumns = @JoinColumn(name = "student_id")
    )
    private List<Student> students;
}

// Target Side (Student)
@Entity
public class Student {
    @ManyToMany(fetch = FetchType.LAZY, cascade = {CascadeType.DETACH, CascadeType.MERGE, CascadeType.PERSIST, CascadeType.REFRESH},
               mappedBy = "students")
    private List<Course> courses;
}
```

> **Interview Note:** In a `@ManyToMany` relationship, never use `CascadeType.REMOVE` or `CascadeType.ALL`. Doing so would mean deleting a course would delete all students enrolled in it, which in turn might delete other courses they are enrolled in, causing a disastrous cascading deletion across the database.
