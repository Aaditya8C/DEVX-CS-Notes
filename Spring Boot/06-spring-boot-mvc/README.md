# Spring Boot MVC — Thymeleaf Integration

This chapter covers the Model-View-Controller (MVC) architecture using Spring Boot and Thymeleaf as the HTML template engine. Projects here demonstrate controller setup, sharing data with the view layer via the `Model` object, rendering dynamic HTML templates, and processing forms.

---

# 00-spring-boot-thymeleaf

## Concept
A standard Spring MVC web application where requests are routed to a controller. The controller prepares the data (Model) and selects an HTML template (View) to render. Thymeleaf processes the template on the server side to produce static HTML sent back to the browser.

## Why We Use It
- Separation of concerns: business logic in Controllers, data structure in Model, and presentation in HTML Views.
- Thymeleaf files are natural HTML templates — they can be opened in browser as static files and still work as server-rendered pages when processed by Spring.
- Out-of-the-box integration with Spring Boot templates folder (`src/main/resources/templates`).

## Important Annotations / Classes
- `@Controller` → marks a class as a Spring MVC controller (returns a view name rather than JSON body)
- `@RequestMapping` → maps web requests to handler methods (class level or method level)
- `@GetMapping` → specific mapping shortcut for HTTP GET requests
- `Model` → container map passed to controller methods to carry data to the view

## Flow / Working
1. **Dynamic Rendering**:
   - Client sends GET request to `/hello`.
   - `DemoController.hello()` is invoked.
   - The method adds the current server date to the `Model` using `theModel.addAttribute("theDate", LocalDateTime.now())`.
   - The controller returns the view name `"helloworld"`.
   - Thymeleaf engine looks for `src/main/resources/templates/helloworld.html` and parses `th:text="'Time on server is, ' + ${theDate}"` to output the current date.
2. **Form Submission**:
   - Client requests `/showForm` → returns `helloworld-form.html`.
   - The user inputs a name and submits the form mapped to GET `/processForm`.
   - `HelloWorldController.processForm()` is invoked, and the request parameter is mapped.
   - `helloworld-process-form.html` displays the submitted name using `${param.studentName}`.

## Critical Code Snapshot
**Controller:**
```java
@Controller
public class DemoController {

    @GetMapping("/hello")
    public String hello(Model theModel) {
        theModel.addAttribute("theDate", java.time.LocalDateTime.now());
        return "helloworld"; // renders templates/helloworld.html
    }
}

@Controller
public class HelloWorldController {

    @RequestMapping("/showForm")
    public String showForm() {
        return "helloworld-form";
    }

    @RequestMapping("/processForm")
    public String processForm() {
        return "helloworld-process-form";
    }
}
```

**Thymeleaf Templates:**
```html
<!-- helloworld.html -->
<html xmlns:th="http://www.thymeleaf.org">
    <body>
        <p th:text="'Time on server is, ' + ${theDate} + '!'" />
    </body>
</html>

<!-- helloworld-form.html -->
<form th:action="@{/processForm}" method="GET">
    <input type="text" name="studentName" />
    <input type="submit" />
</form>

<!-- helloworld-process-form.html -->
Student name: <span th:text="${param.studentName}"></span>
```

> **Interview Notes:**
> - `@Controller` vs `@RestController`: `@Controller` returns a logical view name (rendered as HTML), while `@RestController` returns raw data (JSON/XML) directly to the response body because it is a combination of `@Controller` and `@ResponseBody`.
> - Spring Boot automatically maps view names to templates under `src/main/resources/templates/` with the `.html` extension.
> - Thymeleaf uses `th:text` for text replacement and `th:action="@{...}"` to compute context-relative URLs for actions.
