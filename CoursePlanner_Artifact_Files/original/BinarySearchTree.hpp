// BinarySearchTree.hpp
// Original artifact from CS 300 (Course Planner).
//
// NOTE (flagged in the Milestone One code review): this is a plain,
// unbalanced binary search tree. If courses load in sorted order (which
// they often do, since course IDs are naturally sorted in most catalog
// files), this tree degenerates into a straight line and search/insert
// degrade from O(log n) to O(n). This is addressed in the Milestone
// Three enhancement by converting this into a self-balancing AVL tree.
#ifndef BINARY_SEARCH_TREE_HPP
#define BINARY_SEARCH_TREE_HPP

#include "Course.hpp"
#include <iostream>

class BinarySearchTree {
private:
    struct Node {
        Course course;
        Node* left;
        Node* right;
        explicit Node(Course c) : course(c), left(nullptr), right(nullptr) {}
    };

    Node* root;

    void insertRecursive(Node* node, Course course) {
        if (course.courseId < node->course.courseId) {
            if (node->left == nullptr) {
                node->left = new Node(course);
            } else {
                insertRecursive(node->left, course);
            }
        } else {
            if (node->right == nullptr) {
                node->right = new Node(course);
            } else {
                insertRecursive(node->right, course);
            }
        }
    }

    void printInOrderHelper(Node* node) const {
        if (node != nullptr) {
            printInOrderHelper(node->left);
            std::cout << node->course.courseId << " - " << node->course.courseName << std::endl;
            printInOrderHelper(node->right);
        }
    }

    Node* findNode(Node* node, const std::string& courseId) const {
        if (node == nullptr || node->course.courseId == courseId) {
            return node;
        }
        if (courseId < node->course.courseId) {
            return findNode(node->left, courseId);
        }
        return findNode(node->right, courseId);
    }

    int heightHelper(Node* node) const {
        if (node == nullptr) return 0;
        int leftHeight = heightHelper(node->left);
        int rightHeight = heightHelper(node->right);
        return 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
    }

public:
    BinarySearchTree() : root(nullptr) {}

    void insert(Course course) {
        if (root == nullptr) {
            root = new Node(course);
        } else {
            insertRecursive(root, course);
        }
    }

    void printInOrder() const {
        printInOrderHelper(root);
    }

    bool contains(const std::string& courseId) const {
        return findNode(root, courseId) != nullptr;
    }

    int getHeight() const {
        return heightHelper(root);
    }
};

#endif
