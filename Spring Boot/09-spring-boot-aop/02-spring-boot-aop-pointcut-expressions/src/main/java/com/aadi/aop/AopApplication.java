package com.aadi.aop;

import com.aadi.aop.dao.AccountDAO;
import com.aadi.aop.dao.MembershipDAO;
import org.springframework.boot.CommandLineRunner;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.context.annotation.Bean;

import java.util.List;

@SpringBootApplication
public class AopApplication {

	public static void main(String[] args) {
		SpringApplication.run(AopApplication.class, args);
	}


	@Bean
	public CommandLineRunner commandLineRunner(AccountDAO accountDAO, MembershipDAO membershipDAO){
		return runner -> {

			demoTheBeforeAdvice(accountDAO,membershipDAO);
		};
	}

	private void demoTheBeforeAdvice(AccountDAO accountDAO, MembershipDAO membershipDAO) {
		accountDAO.addAccount();
		accountDAO.doWork();

		membershipDAO.addAccount();

		// call to getter and setter methods
		accountDAO.setName("Teslaaa");
		accountDAO.setServiceCode("Spacexx");

		accountDAO.getName();
		accountDAO.getServiceCode();
	}
}
