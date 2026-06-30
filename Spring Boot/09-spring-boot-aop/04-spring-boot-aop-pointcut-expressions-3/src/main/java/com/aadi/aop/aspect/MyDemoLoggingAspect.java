package com.aadi.aop.aspect;

import com.aadi.aop.Account;
import org.aspectj.lang.JoinPoint;
import org.aspectj.lang.ProceedingJoinPoint;
import org.aspectj.lang.annotation.*;
import org.springframework.stereotype.Component;

import java.util.List;

@Aspect // java class for collection of related advices
@Component

public class MyDemoLoggingAspect {

    @Around("execution(* com.aadi.aop.service.TrafficFortuneService.getFortune(..))")
    public Object aroundGetFortune(
            ProceedingJoinPoint proceedingJoinPoint) throws Throwable{

        String method = proceedingJoinPoint.getSignature().toShortString();
        System.out.println("\n Executing @Around on: "+method);

        // begin timestamp
        long begin = System.currentTimeMillis();

        // execute the method
        Object result = proceedingJoinPoint.proceed();

        // end timestamp
        long end = System.currentTimeMillis();

        // calculate duration
        long duration = end - begin;
        System.out.println("\n Duration: "+ duration / 1000.0 + " seconds");

        return result;
    }

    // New advice for @AfterReturning on findAccounts
    @AfterReturning(pointcut = "execution(* com.aadi.aop.dao.AccountDAO.findAccounts(..))", returning = "result")
    public void afterReturningFindAccountsAdvice(JoinPoint joinPoint, List<Account> result) {
        String method = joinPoint.getSignature().toShortString();
        System.out.println("\n ===>>> Executing @AfterRunning on menthod: " + method);

        System.out.println("\n ===>>> Results are: " + result);

    }

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
