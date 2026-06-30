package com.aadi.springboot.cruddemo.rest;

//import com.aadi.springboot.cruddemo.dao.EmployeeDAO;
import com.aadi.springboot.cruddemo.entity.Employee;
import com.aadi.springboot.cruddemo.service.EmployeeService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@RestController
@RequestMapping("/api")
public class EmployeeRestController {

//    inject emp dao
//    private EmployeeDAO employeeDAO;
    private EmployeeService employeeService;

//    public EmployeeRestController(EmployeeDAO theEmployeeDAO) {
//        employeeDAO = theEmployeeDAO;
//    }

    @Autowired
    public EmployeeRestController(EmployeeService theEmployeeService) {
        employeeService = theEmployeeService;
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
        return dbEmployee;
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


}
