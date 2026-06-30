package com.aadi.demo.rest;


import com.aadi.demo.entity.Student;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import java.util.ArrayList;
import java.util.List;

@RestController
@RequestMapping("/api")
public class StudentRestController {

    @GetMapping("/students")
    public List<Student> getStudents() {
        List<Student> students =  new ArrayList<>();
        students.add(new Student("Aadi","Padte"));
        students.add(new Student("Mahesh","Kale"));
        students.add(new Student("Vedant","Mhatre"));

        // jackson internally converts the student array list to json data
        return students;
    }



}
