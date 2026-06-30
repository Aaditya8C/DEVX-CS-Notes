# Spring Boot MVC — Form Validation & Data Binding

This chapter covers form validation and data binding in Spring MVC. It explains how to validate user inputs on the server side using the Jakarta Bean Validation API (formerly JSR-380) with Hibernate Validator as the implementation provider. It also details custom validation rules and error message rendering in Thymeleaf templates.

---

# 00-spring-boot-thymeleaf (Validation & Forms Reference)

## Concept
Binding web form inputs directly to Java backing objects (Command Objects) and validating fields before processing. If validation fails, errors are captured and shown back to the user on the same form.

## Why We Use It
- Prevents invalid or malformed data from reaching the database.
- Implements standard validation annotations (`@NotNull`, `@Size`, `@Min`, `@Pattern`) without writing verbose `if-else` statements.
- Trims whitespace from input strings automatically using a preprocessor.
- Provides support for custom business rules via custom validation annotations.

## Important Annotations / Classes
- `@Valid` → tells Spring to perform validation on the bound model object
- `BindingResult` → holds the results of validation and binding (must immediately follow the validated object in the method signature)
- `@InitBinder` → pre-processes all web requests (used here to register a `StringTrimmerEditor` to convert empty strings/spaces to `null`)
- `@NotNull` → validation constraint that the field must not be null
- `@Size(min=x, max=y)` → checks if string length falls in range
- `@Min(value)` / `@Max(value)` → enforces numeric range limits
- `@Pattern(regexp)` → validates field against a regular expression pattern
- `@Constraint` → marks custom annotations as validation constraints
- `ConstraintValidator<A, T>` → interface implemented by custom validators to specify validation logic

## Flow / Working
1. **String Trimming**: An `@InitBinder` method trims leading/trailing whitespace from form inputs. If an input is empty/only spaces, it is set to `null`.
2. **Form Rendering**: The user requests a form. The controller adds a new model object (e.g., `Customer`) to hold form data. Thymeleaf binds inputs using `th:field="*{fieldName}"`.
3. **Submission**: The user submits the form.
4. **Validation Check**:
   - The controller method receives the input object annotated with `@Valid` followed by `BindingResult`.
   - If validation errors are present (`bindingResult.hasErrors()`), the controller returns the form view name again.
   - Thymeleaf renders error messages using `th:if="${#fields.hasErrors('fieldName')}"` or `th:errors="*{fieldName}"`.
5. **Success**: If no errors are found, the data is processed or saved.

## Critical Code Snapshot
**Controller with InitBinder and Validation:**
```java
@Controller
@RequestMapping("/customer")
public class CustomerController {

    // Pre-process all web requests to trim strings
    @InitBinder
    public void initBinder(WebDataBinder dataBinder) {
        StringTrimmerEditor stringTrimmerEditor = new StringTrimmerEditor(true);
        dataBinder.registerCustomEditor(String.class, stringTrimmerEditor);
    }

    @RequestMapping("/showForm")
    public String showForm(Model theModel) {
        theModel.addAttribute("customer", new Customer());
        return "customer-form";
    }

    @RequestMapping("/processForm")
    public String processForm(
            @Valid @ModelAttribute("customer") Customer theCustomer,
            BindingResult theBindingResult) {

        if (theBindingResult.hasErrors()) {
            return "customer-form"; // return to form if validation fails
        } else {
            return "customer-confirmation";
        }
    }
}
```

**Backing POJO (Customer) with Validation Annotations:**
```java
public class Customer {

    private String firstName;

    @NotNull(message = "is required")
    @Size(min = 1, message = "is required")
    private String lastName;

    @NotNull(message = "is required")
    @Min(value = 0, message = "must be greater than or equal to zero")
    @Max(value = 10, message = "must be less than or equal to 10")
    private Integer freePasses;

    @Pattern(regexp = "^[a-zA-Z0-9]{5}$", message = "only 5 chars/digits allowed")
    private String postalCode;

    // Custom Validation Annotation Usage
    @CourseCode(value = "LUV", message = "must start with LUV")
    private String courseCode;

    // Getters and setters...
}
```

**Custom Validation Annotation Definition (`@CourseCode`):**
```java
@Constraint(validatedBy = CourseCodeConstraintValidator.class)
@Target({ElementType.METHOD, ElementType.FIELD})
@Retention(RetentionPolicy.RUNTIME)
public @interface CourseCode {
    public String value() default "LUV";
    public String message() default "must start with LUV";
    public Class<?>[] groups() default {};
    public Class<? extends Payload>[] payload() default {};
}
```

**Custom Validator Class:**
```java
public class CourseCodeConstraintValidator implements ConstraintValidator<CourseCode, String> {

    private String coursePrefix;

    @Override
    public void initialize(CourseCode theCourseCode) {
        coursePrefix = theCourseCode.value();
    }

    @Override
    public boolean isValid(String theCode, ConstraintValidatorContext theConstraintValidatorContext) {
        if (theCode != null) {
            return theCode.startsWith(coursePrefix);
        }
        return true; // if null, check it with @NotNull if needed
    }
}
```

**Thymeleaf Validation Form:**
```html
<form th:action="@{/customer/processForm}" th:object="${customer}" method="POST">
    First Name: <input type="text" th:field="*{firstName}" /><br>
    
    Last Name (*): <input type="text" th:field="*{lastName}" />
    <!-- Show error if validation fails -->
    <span th:if="${#fields.hasErrors('lastName')}" th:errors="*{lastName}" class="error"></span><br>

    Free Passes: <input type="text" th:field="*{freePasses}" />
    <span th:if="${#fields.hasErrors('freePasses')}" th:errors="*{freePasses}" class="error"></span><br>

    Postal Code: <input type="text" th:field="*{postalCode}" />
    <span th:if="${#fields.hasErrors('postalCode')}" th:errors="*{postalCode}" class="error"></span><br>

    Course Code: <input type="text" th:field="*{courseCode}" />
    <span th:if="${#fields.hasErrors('courseCode')}" th:errors="*{courseCode}" class="error"></span><br>

    <input type="submit" value="Submit" />
</form>
```

> **Interview Notes:**
> - `@NotNull` vs `@NotEmpty` vs `@NotBlank`: `@NotNull` checks if value is not null. `@NotEmpty` checks if not null and size/length > 0. `@NotBlank` checks if not null and trimmed length > 0 (excludes whitespaces).
> - **BindingResult parameter order**: The `BindingResult` parameter **must** immediately follow the model attribute parameter (e.g. `@ModelAttribute("customer") Customer theCustomer, BindingResult theBindingResult`). If any other parameter is placed between them, validation results will not bind correctly and Spring will throw an exception.
> - `@InitBinder` pre-processes every web request. Using `StringTrimmerEditor(true)` ensures that any form field containing only white space gets converted to a null value, which can then be validated correctly by `@NotNull` or `@Size(min=1)`.
