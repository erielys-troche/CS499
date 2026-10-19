// HashTable.hpp
// Carried over from the original artifact -- not part of the Milestone
// Three enhancement, kept here so the program still compiles and runs
// as a full course planner with all three storage structures available.
#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

#include "Course.hpp"
#include <vector>
#include <list>
#include <iostream>

class HashTable {
private:
    static const int TABLE_SIZE = 10;
    std::vector<std::list<Course>> table;

    unsigned int hash(const std::string& courseId) const {
        unsigned int sum = 0;
        for (char c : courseId) {
            sum += static_cast<unsigned int>(c);
        }
        return sum % TABLE_SIZE;
    }

public:
    HashTable() : table(TABLE_SIZE) {}

    void insert(Course course) {
        unsigned int index = hash(course.courseId);
        table[index].push_back(course);
    }

    void printAll() const {
        for (const auto& bucket : table) {
            for (const auto& course : bucket) {
                std::cout << course.courseId << " - " << course.courseName << std::endl;
            }
        }
    }

    bool contains(const std::string& courseId) const {
        unsigned int index = hash(courseId);
        for (const auto& course : table[index]) {
            if (course.courseId == courseId) return true;
        }
        return false;
    }
};

#endif
