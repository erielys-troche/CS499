// CourseGraph.hpp
// ENHANCEMENT (Milestone Three — Algorithms and Data Structures).
//
// Models course prerequisites as a directed graph (an edge from a
// prerequisite to the course that requires it) and implements Kahn's
// algorithm for topological sort. This is new functionality that did
// not exist in the original artifact:
//   1. It can detect a circular prerequisite dependency (e.g. Course A
//      requires Course B, and Course B requires Course A), which the
//      original validateCourses() could not do.
//   2. It generates a valid course-taking order for students/advisors.
#ifndef COURSE_GRAPH_HPP
#define COURSE_GRAPH_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <iostream>

class CourseGraph {
private:
    std::unordered_map<std::string, std::vector<std::string>> adjacency;
    std::unordered_map<std::string, int> inDegree;

public:
    void addCourse(const std::string& courseId) {
        if (adjacency.find(courseId) == adjacency.end()) {
            adjacency[courseId] = {};
            inDegree[courseId] = 0;
        }
    }

    void addPrerequisiteEdge(const std::string& prereqId, const std::string& courseId) {
        addCourse(prereqId);
        addCourse(courseId);
        adjacency[prereqId].push_back(courseId);
        inDegree[courseId]++;
    }

    // Returns a valid course order using Kahn's algorithm, or an empty
    // vector (with an explanatory message) if a cycle is detected.
    std::vector<std::string> topologicalSort() const {
        std::unordered_map<std::string, int> localInDegree = inDegree;
        std::queue<std::string> ready;

        for (const auto& pair : localInDegree) {
            if (pair.second == 0) {
                ready.push(pair.first);
            }
        }

        std::vector<std::string> order;
        while (!ready.empty()) {
            std::string current = ready.front();
            ready.pop();
            order.push_back(current);

            for (const auto& neighbor : adjacency.at(current)) {
                localInDegree[neighbor]--;
                if (localInDegree[neighbor] == 0) {
                    ready.push(neighbor);
                }
            }
        }

        if (order.size() != adjacency.size()) {
            std::cout << "Error: circular prerequisite dependency detected. "
                      << "Cannot generate a valid course order." << std::endl;
            return {};
        }

        return order;
    }

    size_t courseCount() const {
        return adjacency.size();
    }
};

#endif
