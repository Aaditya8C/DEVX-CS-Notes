package com.aadi.springcoredemo.common;

import org.springframework.context.annotation.Lazy;
import org.springframework.stereotype.Component;

//Component annotation marks this class as a bean allowing it to inject as a dep
@Component
public class TrackCoach implements Coach {

    public TrackCoach(){
        System.out.println("In constructor: " + getClass().getSimpleName());
    }

    @Override
    public String getDailyWorkout() {
        return "Run a hard for 10 mins daily";
    }
}
