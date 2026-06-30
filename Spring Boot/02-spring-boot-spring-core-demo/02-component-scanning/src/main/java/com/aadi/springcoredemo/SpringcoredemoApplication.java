package com.aadi.springcoredemo;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

//@SpringBootApplication(
//		// when there are multiple base packages in the project
//		// we need to define all of them here in this option
//		// in order to scan all the packages
//		scanBasePackages = {"com.aadi.springcoredemo",				"com.aadi.util"}
//)

@SpringBootApplication
public class SpringcoredemoApplication {

	public static void main(String[] args) {
//		SpringApplication - bootstrap our spring boot application
		SpringApplication.run(SpringcoredemoApplication.class, args);
	}

}
