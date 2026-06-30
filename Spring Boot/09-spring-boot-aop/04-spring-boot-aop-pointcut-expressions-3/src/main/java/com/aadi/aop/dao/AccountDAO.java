package com.aadi.aop.dao;


import com.aadi.aop.Account;

import java.util.List;

public interface AccountDAO {

    List<Account> findAccounts();

    void addAccount();

    boolean doWork();

    public String getName();

    public void setName(String name);

    public String getServiceCode();

    public void setServiceCode(String serviceCode);
}
