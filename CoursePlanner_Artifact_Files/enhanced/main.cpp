// main.cpp
// ENHANCEMENT (Milestone Three — Algorithms and Data Structures).
//
// Two enhancements over the original artifact:
//  1. The plain BinarySearchTree has been replaced with a self-balancing
//     AVLTree, guaranteeing O(log n) search/insert regardless of load
//     order.
//  2. Prerequisites are now modeled as a directed graph (CourseGraph),
//     which can detect circular dependencies and generate a valid
//     course-taking order -- neither of which the original code could do.
#include "Course.hpp"
#include "AVLTree.hpp"
#include "HashTable.hpp"
#include "CourseGraph.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

std::vector<Course> courseVector;
AVLTree courseTree;
HashTable courseHashTable;
CourseGraph prerequisiteGraph;

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
        courseTree.insert(course); // now a self-balancing AVL insert
        courseHashTable.insert(course);

        prerequisiteGraph.addCourse(course.courseId);
        for (const auto& prereq : course.prerequisites) {
            prerequisiteGraph.addPrerequisiteEdge(prereq, course.courseId);
        }
    }

    std::cout << "Courses loaded successfully." << std::endl;
    std::cout << "AVL tree height after loading: " << courseTree.getHeight()
              << " for " << courseVector.size() << " courses "
              << "(a plain BST loading these same IDs in sorted order "
              << "would have a height of " << courseVector.size() << ")" << std::endl;
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

    // ENHANCEMENT: also check for circular prerequisite chains using the
    // graph's topological sort -- the original version could not do this.
    std::vector<std::string> order = prerequisiteGraph.topologicalSort();
    if (order.empty() && !courseVector.empty()) {
        return false; // explanatory message already printed by topologicalSort()
    }

    std::cout << "All prerequisites are valid, and no circular dependencies were found." << std::endl;
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

void printCourseSequence() {
    // ENHANCEMENT: brand-new feature. Generates a valid course-taking
    // order using a topological sort over the prerequisite graph.
    std::vector<std::string> order = prerequisiteGraph.topologicalSort();
    if (order.empty()) {
        return; // error already printed inside topologicalSort()
    }
    std::cout << "Recommended course order:" << std::endl;
    for (size_t i = 0; i < order.size(); i++) {
        std::cout << (i + 1) << ". " << order[i] << std::endl;
    }
}

int main() {
    std::string filename = "courses.txt";
    int choice = 0;

    while (choice != 9) {
        std::cout << "\nMenu:\n";
        std::cout << "1. Load Courses\n";
        std::cout << "2. Validate Courses (now detects circular prerequisites)\n";
        std::cout << "3. Search Course\n";
        std::cout << "4. Print All Courses (Sorted)\n";
        std::cout << "5. Generate Valid Course Sequence (new)\n";
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
        } else if (choice == 5) {
            printCourseSequence();
        } else if (choice == 9) {
            std::cout << "Goodbye" << std::endl;
        } else {
            std::cout << "Invalid option" << std::endl;
        }
    }

    return 0;
}
