package com.aadi.mapping.DAO;


import com.aadi.mapping.entity.Course;
import com.aadi.mapping.entity.Instructor;
import com.aadi.mapping.entity.InstructorDetail;

import java.util.List;

public interface AppDAO {
    void save(Instructor theInstructor);

    Instructor findInstructorById(int instructorId);

    InstructorDetail findInstructorDetailById(int instructorId);

    List<Course> findCourseByInstructorId(int instructorId);

    Instructor findInstructorByIdJoinFetch(int instructorId);

    void deleteInstructorById(int instructorId);
}
