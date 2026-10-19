// main.cpp
// Original artifact from CS 300 (Course Planner).
//
// NOTE (flagged in the Milestone One review): validateCourses() only
// checks that each listed prerequisite exists somewhere in the course
// list -- it cannot detect a circular prerequisite chain (e.g. Course A
// requires Course B, and Course B requires Course A). This is addressed
// in the Milestone Three enhancement.
#include "Course.hpp"
#include "BinarySearchTree.hpp"
#include "HashTable.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

std::vector<Course> courseVector;
BinarySearchTree courseTree;
HashTable courseHashTable;

void loadCourses(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error: File not found." << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> tokens;
        while (std::getline(ss, token, ',')) {
            tokens.push_back(token);
        }
        if (tokens.size() < 2) continue;

        Course course;
        course.courseId = tokens[0];
        course.courseName = tokens[1];
        for (size_t i = 2; i < tokens.size(); i++) {
            course.prerequisites.push_back(tokens[i]);
        }

        courseVector.push_back(course);
        courseTree.insert(course);
        courseHashTable.insert(course);
    }

    std::cout << "Courses loaded successfully." << std::endl;
    std::cout << "(BST height after loading: " << courseTree.getHeight()
              << " for " << courseVector.size() << " courses)" << std::endl;
}

bool validateCourses() {
    for (const auto& course : courseVector) {
        for (const auto& prereq : course.prerequisites) {
            bool found = false;
            for (const auto& c : courseVector) {
                if (c.courseId == prereq) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                std::cout << "ERROR: Prerequisite " << prereq
                          << " missing for " << course.courseId << std::endl;
                return false;
            }
        }
    }
    std::cout << "All prerequisites are valid." << std::endl;
    return true;
}

void searchCourse(const std::string& courseId) {
    for (const auto& course : courseVector) {
        if (course.courseId == courseId) {
            std::cout << course.courseId << " - " << course.courseName << std::endl;
            std::cout << "Prerequisites: ";
            if (course.prerequisites.empty()) {
                std::cout << "None";
            }
            for (size_t i = 0; i < course.prerequisites.size(); i++) {
                std::cout << course.prerequisites[i];
                if (i != course.prerequisites.size() - 1) std::cout << ", ";
            }
            std::cout << std::endl;
            return;
        }
    }
    std::cout << "Error: Course not found." << std::endl;
}

void printCoursesSorted() {
    std::vector<Course> sorted = courseVector;
    std::sort(sorted.begin(), sorted.end(), [](const Course& a, const Course& b) {
        return a.courseId < b.courseId;
    });
    for (const auto& c : sorted) {
        std::cout << c.courseId << " - " << c.courseName << std::endl;
    }
}

int main() {
    std::string filename = "courses.txt";
    int choice = 0;

    while (choice != 9) {
        std::cout << "\nMenu:\n";
        std::cout << "1. Load Courses\n";
        std::cout << "2. Validate Courses\n";
        std::cout << "3. Search Course\n";
        std::cout << "4. Print All Courses (Sorted)\n";
        std::cout << "9. Exit\n";
        std::cout << "Choice: ";
        if (!(std::cin >> choice)) break;

        if (choice == 1) {
            loadCourses(filename);
        } else if (choice == 2) {
            validateCourses();
        } else if (choice == 3) {
            std::string id;
            std::cout << "Enter course ID: ";
            std::cin >> id;
            searchCourse(id);
        } else if (choice == 4) {
            printCoursesSorted();
        } else if (choice == 9) {
            std::cout << "Goodbye" << std::endl;
        } else {
            std::cout << "Invalid option" << std::endl;
        }
    }

    return 0;
}
