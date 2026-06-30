package com.aadi.springcoredemo.common;

import org.springframework.stereotype.Component;

//Component annotation marks this class as a bean allowing it to inject as a dep
@Component
public class TennisCoach implements Coach {
    @Override
    public String getDailyWorkout() {
        return "Practice your backhand volley.";
    }
}
