package com.aadi.aop.aspect;

import org.aspectj.lang.annotation.Aspect;
import org.aspectj.lang.annotation.Before;
import org.springframework.stereotype.Component;

@Aspect // java class for collection of related advices
@Component
public class MyDemoLoggingAspect {

//    @Before("execution (public void addAccount())") // this is a pointcut expression. THis matched any addAccount method in any class
    @Before("execution (public void com.aadi.aop.dao.AccountDAO.addAccount())") // only matches the method in the AccountDAO class
    public void beforeAddAccountAdvice(){
        System.out.println("\n---->>> Executing @Before advice on addAccount()");
    }
}
