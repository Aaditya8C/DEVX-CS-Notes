package com.aadi.aop.dao;


import org.springframework.stereotype.Repository;

@Repository // for component scanning
public class AccountDAOImpl implements AccountDAO {
    @Override
    public void addAccount() {
        System.out.println(getClass() + ": Adding an account");
    }
}
