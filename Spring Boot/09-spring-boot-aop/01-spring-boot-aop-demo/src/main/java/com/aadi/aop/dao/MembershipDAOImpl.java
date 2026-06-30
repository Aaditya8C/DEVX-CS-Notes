package com.aadi.aop.dao;


import org.springframework.stereotype.Repository;

@Repository // for component scanning
public class MembershipDAOImpl implements MembershipDAO {
    @Override
    public void addAccount() {
        System.out.println(getClass() + ": Adding a membership account");
    }
}
