package com.aadi.mapping.DAO;


import com.aadi.mapping.entity.Instructor;
import com.aadi.mapping.entity.InstructorDetail;

public interface AppDAO {
    void save(Instructor theInstructor);

    Instructor findInstructorById(int instructorId);

    InstructorDetail findInstructorDetailById(int instructorId);
}
