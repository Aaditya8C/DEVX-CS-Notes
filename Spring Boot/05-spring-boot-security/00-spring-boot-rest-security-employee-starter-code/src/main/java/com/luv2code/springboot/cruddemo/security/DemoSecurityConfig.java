package com.luv2code.springboot.cruddemo.security;

import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.http.HttpMethod;
import org.springframework.security.config.Customizer;
import org.springframework.security.config.annotation.web.builders.HttpSecurity;
import org.springframework.security.provisioning.JdbcUserDetailsManager;
import org.springframework.security.provisioning.UserDetailsManager;
import org.springframework.security.web.SecurityFilterChain;

import javax.sql.DataSource;

@Configuration
public class DemoSecurityConfig {

    // support for jdbc...no hardcoding
//    @Bean
//    public UserDetailsManager userDetailsManager(DataSource dataSource) {
////        JdbcUserDetailsManager - tells the spring security to use JDBC authentication with our data source
//        return new JdbcUserDetailsManager(dataSource);
//    }


// Custom Tables with custom fields
@Bean
public UserDetailsManager userDetailsManager(DataSource dataSource) {
//        JdbcUserDetailsManager - tells the spring security to use JDBC authentication with our data source
    JdbcUserDetailsManager jdbcUserDetailsManager = new JdbcUserDetailsManager(dataSource);

    // defining query to retrieve a user by username
    jdbcUserDetailsManager.setUsersByUsernameQuery(
            "select user_id, pw, active from members where user_id=?"
    );
    // defining query to retrieve roles/autorities by username
    jdbcUserDetailsManager.setAuthoritiesByUsernameQuery(
            "select user_id, role from roles where user_id=?"
    );
    return jdbcUserDetailsManager;
}





    @Bean
    public SecurityFilterChain filterChain(HttpSecurity http)  throws Exception {
//        /** means for a single employee. It covers all the subpaths
        http.authorizeHttpRequests(configurer ->
                configurer
                        .requestMatchers(HttpMethod.GET,"/api/employees").hasRole("EMPLOYEE")
                        .requestMatchers(HttpMethod.GET,"/api/employees/**").hasRole("EMPLOYEE")
                        .requestMatchers(HttpMethod.POST,"/api/employees").hasRole("MANAGER")
                        .requestMatchers(HttpMethod.PUT,"/api/employees").hasRole("MANAGER")
                        .requestMatchers(HttpMethod.PATCH,"/api/employees/**").hasRole("MANAGER")
                        .requestMatchers(HttpMethod.DELETE,"/api/employees/**").hasRole("ADMIN")
        );

//        use http basic authentication
        http.httpBasic(Customizer.withDefaults());

//        disable csrf
//        it's not req for stateless REST Apis that uses POST,PUT,DELETE and/or PATCH
        http.csrf(csrf -> csrf.disable());

        return http.build();
    }

    /*
    //    Since users are defined here, spring won't use user and pass from the application properties.
    @Bean
    public InMemoryUserDetailsManager inMemoryUserDetailsManager() {
        UserDetails aaditya = User.builder()
                .username("aaditya")
                .password("{noop}test1")
                .roles("EMPLOYEE")
                .build();

        UserDetails vedant = User.builder()
                .username("vedant")
                .password("{noop}test1")
                .roles("EMPLOYEE","MANAGER")
                .build();

        UserDetails aniket = User.builder()
                .username("aniket")
                .password("{noop}test1")
                .roles("EMPLOYEE","MANAGER","ADMIN")
                .build();

        return new InMemoryUserDetailsManager(aaditya, vedant, aniket);
    }
    */
}
