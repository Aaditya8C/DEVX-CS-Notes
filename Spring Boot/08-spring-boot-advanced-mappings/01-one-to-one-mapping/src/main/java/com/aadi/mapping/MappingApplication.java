package com.aadi.mapping;

import com.aadi.mapping.DAO.AppDAO;
import com.aadi.mapping.entity.Instructor;
import com.aadi.mapping.entity.InstructorDetail;
import org.springframework.boot.CommandLineRunner;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.context.annotation.Bean;

@SpringBootApplication
public class MappingApplication {

	public static void main(String[] args) {
		SpringApplication.run(MappingApplication.class, args);
	}


	@Bean
	public CommandLineRunner commandLineRunner(AppDAO appDAO) {
		return runner -> {
			createInstructor(appDAO);
			fetchInstructor(appDAO);
			fetchInstructorDetail(appDAO);
		};
	}

	private void fetchInstructorDetail(AppDAO appDAO) {
		int id = 1;
		System.out.println("Fetching instructor details for " + id);
		InstructorDetail  instructorDetail = appDAO.findInstructorDetailById(id);
		System.out.println("Details" + instructorDetail);
		System.out.println("Associated Instructor: " + instructorDetail.getInstructor());

	}

	// This Function represents the bidirectional association between Instructor and InstructorDetail
	private void fetchInstructor(AppDAO appDAO) {
		int id = 1;
		Instructor instructor = appDAO.findInstructorById(id);
		System.out.println("Instructor Found: " + instructor);
		System.out.println("Associated Instructor Details: " + instructor.getInstructorDetail());

	}

	private void createInstructor(AppDAO appDAO) {
		Instructor instructor =
				new Instructor("Aaditya","Padte","aa@gmail.com");

		InstructorDetail instructorDetail =
				new InstructorDetail("http://www.ajpadte.com/youtube",
						"Singing");

		// associating the objects
		instructor.setInstructorDetail(instructorDetail);

		// this will also save the details obj bcoz of cascadetype.all
		System.out.println("Saving instructor:  " + instructor);
		appDAO.save(instructor);

	}

}
