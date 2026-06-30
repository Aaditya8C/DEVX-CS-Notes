package com.aadi.mapping;

import com.aadi.mapping.DAO.AppDAO;
import com.aadi.mapping.entity.Course;
import com.aadi.mapping.entity.Instructor;
import com.aadi.mapping.entity.InstructorDetail;
import org.springframework.boot.CommandLineRunner;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.context.annotation.Bean;

import java.util.List;

@SpringBootApplication
public class MappingApplication {

	public static void main(String[] args) {
		SpringApplication.run(MappingApplication.class, args);
	}


	@Bean
	public CommandLineRunner commandLineRunner(AppDAO appDAO) {
		return runner -> {
//			createInstructor(appDAO);
//			fetchInstructor(appDAO);
//			fetchInstructorDetail(appDAO);

//			createInstructorWithCourses(appDAO);

//			findInstructorWithCourses(appDAO);

//			findInstructorWithCoursesJoinFetch(appDAO);

			deleteInstructor(appDAO);
		};
	}

	private void deleteInstructor(AppDAO appDAO) {
		int id = 1;
		System.out.println("Deleting the instructor: "+ id);
		appDAO.deleteInstructorById(id);
	}


	// this is the alternative for the lazy approach where we need to write additional query for courses only.
	private void findInstructorWithCoursesJoinFetch(AppDAO appDAO) {

		int id = 1;

		System.out.println("Finding instructor id: "+id);
		Instructor theInstructor = appDAO.findInstructorByIdJoinFetch(id); // will fetch instructor and courses both

		System.out.println("Instructor: "+ theInstructor);
		System.out.println("associated courses: "+ theInstructor.getCourses());
	}

	private void findInstructorWithCourses(AppDAO appDAO) {

		int id = 1;
		System.out.println("Finding instructor id: "+id);
		Instructor theInstructor = appDAO.findInstructorById(id);

		// Eager Loading
		System.out.println("Instructor: "+ theInstructor);
//		System.out.println("associated courses: "+ theInstructor.getCourses());


		// Lazy Loading of Courses
		List<Course> courses = appDAO.findCourseByInstructorId(id);
		theInstructor.setCourses(courses);
		System.out.println("associated courses fetched with lazy loading: "+ theInstructor.getCourses());

		System.out.println("Done!!");

	}

	private void createInstructorWithCourses(AppDAO appDAO) {
		Instructor instructor =
				new Instructor("Pooja","Patil","pooja@gmail.com");

		InstructorDetail instructorDetail =
				new InstructorDetail("http://www.ajpadte.com/youtube",
						"Singing");

		// associating the objects
		instructor.setInstructorDetail(instructorDetail);

		Course course1 = new Course("BDA");
		Course course2 = new Course("ADE");


		// adding courses to instructor
		instructor.add(course1);
		instructor.add(course2);

		// saving the instructor
		System.out.println("Saving instructor" + instructor);

		// this will also save the course
		// bcoz of CascadeType.PERSIST
		appDAO.save(instructor);

		System.out.println("Donee");
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
