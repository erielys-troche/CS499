// AVLTree.hpp
// ENHANCEMENT (Milestone Three — Algorithms and Data Structures).
//
// Replaces the original plain, unbalanced BinarySearchTree. This tree
// tracks a height at every node and performs rotations on insert to
// keep itself balanced, guaranteeing O(log n) search and insert no
// matter what order courses are loaded in -- including the sorted-ID
// worst case that broke the original implementation.
#ifndef AVL_TREE_HPP
#define AVL_TREE_HPP

#include "Course.hpp"
#include <algorithm>
#include <iostream>

class AVLTree {
private:
    struct Node {
        Course course;
        Node* left;
        Node* right;
        int height;
        explicit Node(Course c) : course(c), left(nullptr), right(nullptr), height(1) {}
    };

    Node* root;

    // Frees every node in the tree (children first, then the parent).
    void destroy(Node* node) {
        if (node == nullptr) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

    int height(Node* node) const {
        return node == nullptr ? 0 : node->height;
    }

    int balanceFactor(Node* node) const {
        return node == nullptr ? 0 : height(node->left) - height(node->right);
    }

    void updateHeight(Node* node) {
        node->height = 1 + std::max(height(node->left), height(node->right));
    }

    Node* rotateRight(Node* y) {
        Node* x = y->left;
        Node* t2 = x->right;
        x->right = y;
        y->left = t2;
        updateHeight(y);
        updateHeight(x);
        return x;
    }

    Node* rotateLeft(Node* x) {
        Node* y = x->right;
        Node* t2 = y->left;
        y->left = x;
        x->right = t2;
        updateHeight(x);
        updateHeight(y);
        return y;
    }

    Node* insertRecursive(Node* node, Course course) {
        if (node == nullptr) {
            return new Node(course);
        }
        if (course.courseId < node->course.courseId) {
            node->left = insertRecursive(node->left, course);
        } else {
            node->right = insertRecursive(node->right, course);
        }

        updateHeight(node);
        int balance = balanceFactor(node);

        // Left-Left case
        if (balance > 1 && node->left != nullptr && course.courseId < node->left->course.courseId) {
            return rotateRight(node);
        }
        // Right-Right case
        if (balance < -1 && node->right != nullptr && course.courseId > node->right->course.courseId) {
            return rotateLeft(node);
        }
        // Left-Right case
        if (balance > 1 && node->left != nullptr && course.courseId > node->left->course.courseId) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        // Right-Left case
        if (balance < -1 && node->right != nullptr && course.courseId < node->right->course.courseId) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
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

public:
    AVLTree() : root(nullptr) {}
    ~AVLTree() { destroy(root); }

    // A copy would share the same nodes and free them twice, so copying is
    // turned off. (The program only ever uses one tree.)
    AVLTree(const AVLTree&) = delete;
    AVLTree& operator=(const AVLTree&) = delete;

    void insert(Course course) {
        root = insertRecursive(root, course);
    }

    void printInOrder() const {
        printInOrderHelper(root);
    }

    bool contains(const std::string& courseId) const {
        return findNode(root, courseId) != nullptr;
    }

    int getHeight() const {
        return height(root);
    }
};

#endif
