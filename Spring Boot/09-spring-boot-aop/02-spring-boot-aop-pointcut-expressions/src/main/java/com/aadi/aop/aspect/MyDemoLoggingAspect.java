package com.aadi.aop.aspect;

import com.aadi.aop.Account;
import org.aspectj.lang.JoinPoint;
import org.aspectj.lang.annotation.AfterReturning;
import org.aspectj.lang.annotation.Aspect;
import org.aspectj.lang.annotation.Before;
import org.aspectj.lang.annotation.Pointcut;
import org.springframework.stereotype.Component;

import java.util.List;

@Aspect // java class for collection of related advices
@Component
public class MyDemoLoggingAspect {

   





    // reusability of pointcut expressions by declaring them once.
    @Pointcut("execution(* com.aadi.aop.dao.*.*(..))")
    private void forDaoPackage(){}

//    create a pointcut for getter method
    @Pointcut("execution(* com.aadi.aop.dao.*.get*(..))")
    private void getter(){}


//    create a pointcut for setter method
    @Pointcut("execution(* com.aadi.aop.dao.*.set*(..))")
    private void setter(){}


//    create a pointcut: include package....exclude getter/setter -- so no advices will be applied to getter and setter methods
    @Pointcut("forDaoPackage() && !(getter() || setter())")
    private void forDaoPackageNoGetterSetter(){}

    @Before("forDaoPackageNoGetterSetter()")
    public void beforeAddAccountAdvice(){
        System.out.println("\n---->>> Executing @Before advice on addAccount()");
    }

    @Before("forDaoPackageNoGetterSetter()")
    public void performApiAnalytics(){
        System.out.println("\n---->>> Performing API Anlysis");
    }

}
