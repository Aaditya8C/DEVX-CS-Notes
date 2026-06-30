package com.aadi.aop.dao;


public interface AccountDAO {

    List<Account> findAccounts();
    
    void addAccount();




    public void setName(String name);

    public String getServiceCode();

    public void setServiceCode(String serviceCode);
}
