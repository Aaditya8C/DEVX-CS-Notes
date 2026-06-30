package com.aadi.mapping.DAO;

import com.aadi.mapping.entity.Course;
import com.aadi.mapping.entity.Instructor;
import com.aadi.mapping.entity.InstructorDetail;
import jakarta.persistence.EntityManager;
import jakarta.persistence.TypedQuery;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Repository;
import org.springframework.transaction.annotation.Transactional;

import java.util.List;

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

    @Override
    public List<Course> findCourseByInstructorId(int instructorId) {
        TypedQuery query = entityManager.createQuery("from Course where instructor.id = :data", Course.class);
        query.setParameter("data",instructorId);

        List<Course> courses = query.getResultList();
        return courses;
    }

    @Override
    public Instructor findInstructorByIdJoinFetch(int instructorId) {
        TypedQuery<Instructor> query = entityManager.createQuery(
                "select i from Instructor i "
                + "JOIN FETCH i.courses "
                + "JOIN FETCH i.instructorDetail " // this eliminates one extra query req to join instructor with instructor
                + "where i.id = :data ",Instructor.class
        );

        query.setParameter("data", instructorId);
        Instructor instructor = query.getSingleResult();
        return instructor;
    }

    @Transactional
    @Override
    public void deleteInstructorById(int instructorId) {
        Instructor instructor = entityManager.find(Instructor.class,instructorId);

        List<Course> courses = instructor.getCourses();


        // this is to break the association between the instructor to be deleted and the associated courses.
        for(Course c: courses){
            c.setInstructor(null);
        }

        entityManager.remove(instructor);
    }


}
