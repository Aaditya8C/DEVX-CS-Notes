package com.aadi.spring.demo.myapp.rest;


import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RestController;

@RestController
public class DemoRestConroller {
    @GetMapping("/")
    public String sayHello(){
        return "Hello Aaditya";
    }
}
