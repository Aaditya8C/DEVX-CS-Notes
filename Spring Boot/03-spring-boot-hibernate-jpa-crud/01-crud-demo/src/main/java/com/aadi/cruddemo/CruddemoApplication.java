package com.aadi.cruddemo;
import com.aadi.cruddemo.dao.StudentDAO;
import com.aadi.cruddemo.entity.Student;
import org.springframework.boot.CommandLineRunner;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.context.annotation.Bean;

import java.util.List;

@SpringBootApplication
public class CruddemoApplication {

	public static void main(String[] args) {
		SpringApplication.run(CruddemoApplication.class, args);
	}

	// this will be executed after the spring beans have been created
	@Bean
	public CommandLineRunner commandLineRunner(StudentDAO studentDAO){
		return runner ->{
//			createStudent(studentDAO);
//			createMultipleStudents(studentDAO);
//			readStudent(studentDAO);
//			readAllStudents(studentDAO);
//			readStudenByLastName(studentDAO);
//			updateStudent(studentDAO);
//			deleteStudent(studentDAO);
			deleteAllStudents(studentDAO);
		};


	}

	private void deleteAllStudents(StudentDAO studentDAO) {
		int studentsDeleted = studentDAO.deleteAll();
		System.out.println("Total deleted students: "+ studentsDeleted);
	}

	private void deleteStudent(StudentDAO studentDAO) {
		int id = 1;
		studentDAO.delete(id);
		readAllStudents(studentDAO);

	}

	private void updateStudent(StudentDAO studentDAO) {
		int studId = 1;
		System.out.println("Getting student with id " + studId);
		Student myStud = studentDAO.findById(studId);
		System.out.println("Updating student id");

		myStud.setFirst_name("Mihika");
		studentDAO.update(myStud);

		System.out.println("Updated student: "+myStud);
	}

	private void readStudenByLastName(StudentDAO studentDAO) {
		List<Student> students = studentDAO.findByLastName("Padte");

		for(Student tempStud: students){
			System.out.println(tempStud);
		}
	}

	private void readAllStudents(StudentDAO studentDAO) {
		List<Student> students = studentDAO.findAll();

		for(Student tempStud: students){
			System.out.println(tempStud);
		}
	}

	private void readStudent(StudentDAO studentDAO) {
		System.out.println("Creating new stud obj....");
		Student temp = new Student("Pradnya","Naik","prad@gmail.com");


		//save the stud obj
		System.out.println("Saving the stud...");
		studentDAO.save(temp);

		//display id of saved stud
		System.out.println("Saved student. Generated id: " + temp.getId());


		Student myStud = studentDAO.findById(temp.getId());
		System.out.println("Found the student: "+ myStud);
	}

	private void createMultipleStudents(StudentDAO studentDAO) {
		System.out.println("Creating 3 stud objss....");
		Student temp1 = new Student("Aaditya","Padte","aadi@gmail.com");
		Student temp2 = new Student("Aaditya","Mhatre","aadi@gmail.com");
		Student temp3 = new Student("Aaditya","Patil","aadi@gmail.com");

		// saving objects
		System.out.println("Saving the students...");
		studentDAO.save(temp1);
		studentDAO.save(temp2);
		studentDAO.save(temp3);

	}

	private void createStudent(StudentDAO studentDAO) {
		//create the student obj
		System.out.println("Creating new stud obj....");
		Student temp = new Student("Aaditya","Padte","aadi@gmail.com");


		//save the stud obj
		System.out.println("Saving the stud...");
		studentDAO.save(temp);

		//display id of saved stud
		System.out.println("Saved student. Generated id: " + temp.getId());
	}
}
