package com.aadi.springboot.cruddemo.rest;

//import com.aadi.springboot.cruddemo.dao.EmployeeDAO;
import com.aadi.springboot.cruddemo.entity.Employee;
import com.aadi.springboot.cruddemo.service.EmployeeService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;
import tools.jackson.databind.json.JsonMapper;

import java.util.List;
import java.util.Map;

@RestController
@RequestMapping("/api")
public class EmployeeRestController {

//    inject emp dao
//    private EmployeeDAO employeeDAO;
    private EmployeeService employeeService;
    private JsonMapper jsonMapper;

//    public EmployeeRestController(EmployeeDAO theEmployeeDAO) {
//        employeeDAO = theEmployeeDAO;
//    }

    @Autowired
    public EmployeeRestController(EmployeeService theEmployeeService,JsonMapper theJsonMapper) {
        employeeService = theEmployeeService;
        jsonMapper = theJsonMapper;
    }

    @GetMapping("/employees")
    public List<Employee> findAll(){
//        return employeeDAO.findAll();
        return employeeService.findAll();
    }

    @GetMapping("/employees/{employeedId}")
    public Employee getEmployee(@PathVariable int employeedId){
        Employee emp =  employeeService.findById(employeedId);
        if(emp == null){
            throw new RuntimeException("Employee not found");
        }

        return emp;
    }


    @PostMapping("/employees")
    public Employee addEmployee(@RequestBody Employee theEmployee){

        theEmployee.setId(0);
        Employee dbEmployee = employeeService.save(theEmployee);
        return       dbEmployee;
    }

    @PutMapping("/employees")
    public Employee updateEmployee(@RequestBody Employee theEmployee){
        Employee dbEmployee = employeeService.save(theEmployee);

        return dbEmployee;
    }

    @DeleteMapping("/employees/{employeeId}")
    public void deleteEmployee(@PathVariable int employeeId){
        employeeService.deleteById(employeeId);
    }

    // Patch mapping helps to make partial updates
    @PatchMapping("/employees/{employeeId}")
    public Employee patchEmployee(@PathVariable int employeeId,
                                  @RequestBody Map<String,Object> patchPayload){
        Employee tempEmployee = employeeService.findById(employeeId);
        if(tempEmployee == null){
            throw new RuntimeException("Employee not found");
        }

        // throw exception if request body contains "id" field
        if(patchPayload.containsKey("id")){
            throw  new RuntimeException("Employee id not allowed in request body");
        }

        Employee patchedEmployee = jsonMapper.updateValue(tempEmployee,patchPayload);
        Employee dbEmployee = employeeService.save(patchedEmployee);
        return dbEmployee;
    }


}
