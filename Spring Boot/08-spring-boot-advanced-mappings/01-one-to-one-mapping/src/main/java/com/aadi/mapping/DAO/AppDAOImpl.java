package com.aadi.mapping.DAO;

import com.aadi.mapping.entity.Instructor;
import com.aadi.mapping.entity.InstructorDetail;
import jakarta.persistence.EntityManager;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Repository;
import org.springframework.transaction.annotation.Transactional;

@Repository
public class AppDAOImpl implements AppDAO {

    private EntityManager entityManager;

    @Autowired
    public AppDAOImpl(EntityManager entityManager) {
        this.entityManager = entityManager;
    }

    @Override
    @Transactional
    public void save(Instructor theInstructor) {
        entityManager.persist(theInstructor); // this will also save the details object since we used cascade all
    }


    @Override
    public Instructor findInstructorById(int instructorId) {
        return entityManager.find(Instructor.class,instructorId);  // this will also retrieve instructor details object
    }

    @Override
    public InstructorDetail findInstructorDetailById(int instructorId) {
        return entityManager.find(InstructorDetail.class,instructorId); // also retrive associated instructor
    }


}
