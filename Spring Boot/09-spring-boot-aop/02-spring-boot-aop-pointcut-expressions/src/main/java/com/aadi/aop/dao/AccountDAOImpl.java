package com.aadi.aop.dao;

import com.aadi.aop.Account;
import java.util.ArrayList;
import java.util.List;

import org.springframework.stereotype.Repository;

import com.aadi.aop.Account;

import java.util.ArrayList;
import java.util.List;

@Repository // for component scanning
public class AccountDAOImpl implements AccountDAO {

    private String name;
    private String serviceCode;

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
