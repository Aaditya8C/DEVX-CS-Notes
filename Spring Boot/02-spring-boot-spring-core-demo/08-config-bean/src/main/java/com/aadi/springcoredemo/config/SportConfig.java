package com.aadi.springcoredemo.config;

import com.aadi.springcoredemo.common.Coach;
import com.aadi.springcoredemo.common.SwimCoach;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

//this config class defines the bean manually through code and by creating the object of the target class
@Configuration
public class SportConfig {

    @Bean
    public Coach swimCoach(){ // bean id will defaults to this method name
        return new SwimCoach();
    }
}
