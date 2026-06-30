package com.aadi.springboot.cruddemo.dao;

import com.aadi.springboot.cruddemo.entity.Employee;
import org.springframework.data.jpa.repository.JpaRepository;

//JpaRepo priovides built in CRUD operations
public interface EmployeeRepository extends JpaRepository<Employee, Integer> {
}
