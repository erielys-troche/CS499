// Course.hpp
// Original artifact from CS 300 (Course Planner).
#ifndef COURSE_HPP
#define COURSE_HPP

#include <string>
#include <vector>

struct Course {
    std::string courseId;
    std::string courseName;
    std::vector<std::string> prerequisites;

    Course() : courseId(""), courseName("") {}
};

#endif
