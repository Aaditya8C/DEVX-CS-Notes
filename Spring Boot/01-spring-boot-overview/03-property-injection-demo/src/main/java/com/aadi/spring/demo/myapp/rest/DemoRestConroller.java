package com.aadi.spring.demo.myapp.rest;


import org.springframework.beans.factory.annotation.Value;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RestController;

@RestController
public class DemoRestConroller {

//    injecting properties
    @Value("${brand.name}")
    private String brandName;

    @Value("${stock.name}")
    private String stockName;

    @GetMapping("tradeInfo")
    public String getTradeInfo(){
        return "Your Brand:" + brandName + " and stock is: " + stockName;
    }
    @GetMapping("/")
    public String sayHello(){
        return "Hello Aaditya";
    }

    @GetMapping("/workout")
    public String getWorkout(){
        return "Hey do workout";
    }

}
