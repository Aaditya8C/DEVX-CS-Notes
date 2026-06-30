package com.aadi.aop.dao;


import com.aadi.aop.Account;
import org.springframework.stereotype.Repository;

import java.util.ArrayList;
import java.util.List;

@Repository // for component scanning
public class AccountDAOImpl implements AccountDAO {

    private String name;
    private String serviceCode;

    @Override
    public List<Account> findAccounts() {
        List<Account> myAccounts = new ArrayList<>();

        Account a1 = new Account("magnus","silver");
        Account a2 = new Account("prag","bronze");
        Account a3 = new Account("gukesh","gold");

        myAccounts.add(a1);
        myAccounts.add(a2);
        myAccounts.add(a3);

        return myAccounts;
    }


    @Override
    public void addAccount() {
        System.out.println(getClass() + ": Adding an account");
    }

    @Override
    public boolean doWork() {
        System.out.println("Doing work");
        return false;
    }

    public String getName() {
        System.out.println("get name");
        return name;
    }

    public void setName(String name) {
        System.out.println("set name");
        this.name = name;
    }

    public String getServiceCode() {
        System.out.println("get service code");
        return serviceCode;
    }

    public void setServiceCode(String serviceCode) {
        System.out.println("set service code");
        this.serviceCode = serviceCode;
    }
}
