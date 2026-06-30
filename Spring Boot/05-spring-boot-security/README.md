# Spring Boot Security — User Authentication & Authorization

This chapter covers securing REST APIs with Spring Security. It explores user authentication using In-Memory users, JDBC database users with the default Spring schema, and JDBC database users with custom tables/fields. It also covers role-based authorization restricting CRUD operations to specific roles (`EMPLOYEE`, `MANAGER`, `ADMIN`).

---

# 00-spring-boot-rest-security-employee-starter-code

## Concept
Securing an Employee REST API by applying Spring Security filters. Authentication validates user credentials (who they are) against a database or in-memory list, while Authorization controls permissions (what they can do) using roles like `EMPLOYEE`, `MANAGER`, and `ADMIN`.

## Why We Use It
- Secures REST endpoints from unauthorized access.
- Restricts destructive operations (like DELETE) to highly privileged users (`ADMIN`), updates/inserts to `MANAGER`, and read operations to `EMPLOYEE`.
- Avoids hardcoding credentials by using JDBC authentication connected to a database.
- Supports encrypted passwords using BCrypt rather than storing plain text.

## Important Annotations / Classes
- `@Configuration` → registers the class as a configuration source for Spring beans
- `SecurityFilterChain` → configuration bean where HTTP requests are matched and security filters are applied
- `UserDetailsManager` / `JdbcUserDetailsManager` → Spring Security service for managing user details in memory or via JDBC
- `DataSource` → represents the database connection pool injected for JDBC Authentication
- `http.authorizeHttpRequests()` → configures URL paths and roles required for each HTTP method
- `http.httpBasic(Customizer.withDefaults())` → enables HTTP Basic Authentication
- `http.csrf(csrf -> csrf.disable())` → disables Cross-Site Request Forgery (CSRF) protection (safe for stateless REST APIs)

## Flow / Working
1. **Request Interception**: Incoming requests to `/api/employees/**` are intercepted by the Spring Security filter chain.
2. **Authentication**:
   - For **In-Memory**: Spring checks credentials against users configured via `InMemoryUserDetailsManager`.
   - For **JDBC (Default Schema)**: Spring queries the `users` and `authorities` tables using the default schema.
   - For **JDBC (Custom Schema)**: Spring executes custom SQL queries (`setUsersByUsernameQuery`, `setAuthoritiesByUsernameQuery`) to find credentials in tables named `members` and `roles`.
3. **Authorization**:
   - `GET /api/employees` and `GET /api/employees/**` are checked for role `EMPLOYEE`.
   - `POST /api/employees`, `PUT /api/employees`, and `PATCH /api/employees/**` are checked for role `MANAGER`.
   - `DELETE /api/employees/**` is checked for role `ADMIN`.
4. **Result**: Valid authenticated requests with correct roles proceed to `EmployeeRestController`. Otherwise, `401 Unauthorized` or `403 Forbidden` is returned.

## Critical Code Snapshot
```java
@Configuration
public class DemoSecurityConfig {

    @Bean
    public UserDetailsManager userDetailsManager(DataSource dataSource) {
        JdbcUserDetailsManager jdbcUserDetailsManager = new JdbcUserDetailsManager(dataSource);

        // Custom queries for non-standard user and role tables
        jdbcUserDetailsManager.setUsersByUsernameQuery(
                "select user_id, pw, active from members where user_id=?"
        );
        jdbcUserDetailsManager.setAuthoritiesByUsernameQuery(
                "select user_id, role from roles where user_id=?"
        );
        return jdbcUserDetailsManager;
    }

    @Bean
    public SecurityFilterChain filterChain(HttpSecurity http) throws Exception {
        http.authorizeHttpRequests(configurer ->
                configurer
                        .requestMatchers(HttpMethod.GET, "/api/employees").hasRole("EMPLOYEE")
                        .requestMatchers(HttpMethod.GET, "/api/employees/**").hasRole("EMPLOYEE")
                        .requestMatchers(HttpMethod.POST, "/api/employees").hasRole("MANAGER")
                        .requestMatchers(HttpMethod.PUT, "/api/employees").hasRole("MANAGER")
                        .requestMatchers(HttpMethod.PATCH, "/api/employees/**").hasRole("MANAGER")
                        .requestMatchers(HttpMethod.DELETE, "/api/employees/**").hasRole("ADMIN")
        );

        http.httpBasic(Customizer.withDefaults());
        http.csrf(csrf -> csrf.disable());

        return http.build();
    }
}
```

> **Interview Notes:**
> - **CSRF (Cross-Site Request Forgery)** is typically disabled in REST APIs because they are stateless (use tokens/Basic Auth and don't use cookies for session tracking).
> - Roles in the database must have the prefix `ROLE_` (e.g., `ROLE_EMPLOYEE`). When using `.hasRole("EMPLOYEE")` in Java code, Spring automatically appends the `ROLE_` prefix under the hood. If your DB has `EMPLOYEE` (without prefix), use `.hasAuthority("EMPLOYEE")` instead.
> - Password encoders are specified by prefixes in the DB password column, such as `{bcrypt}` or `{noop}` (for plaintext).
