#include "read_buffer_tree.h"

ReadBufferTree::ReadBufferTree() { this->root_ = nullptr; }

ReadBufferTree::~ReadBufferTree() { this->Destroy(this->root_); }

/**
 * 将数据插入到红黑树中
 * */
void ReadBufferTree::Insert(int key) {
    ReadBufferNode *parent, *current;
    parent = current = this->root_;
    /* 首先以普通的排序二叉树的规则插入数据 */
    while (current != nullptr) {
        parent = current;
        if (key < current->key) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    current = new ReadBufferNode(key);
    current->parent = parent;

    if (current->key < parent->key) {
        parent->left = current;
    } else {
        parent->right = current;
    }

    this->InsertRebalance(current);
}

/**
 * 从红黑树中查找指定 key 对应的数据
 * 这是最简单的一步, 因为红黑树也是一棵排序二叉树...
 * */
ReadBufferNode *ReadBufferTree::Find(int key) {
    ReadBufferNode *current = this->root_;
    while (current != nullptr) {
        if (key < current->key) {
            current = current->left;
        } else if (key > current->key) {
            current = current->right;
        } else {
            return current;
        }
    }
    return nullptr;
}

/**
 * 从红黑树中删除 key 对应的数据
 * */
void ReadBufferTree::Remove(int key) {
    ReadBufferNode *current = this->Find(key);
    if (current) {
        this->RemoveRebalance(current);
        delete current;
    }
}

/* TODO */
void ReadBufferTree::InsertRebalance(ReadBufferNode *node) {}
void ReadBufferTree::RemoveRebalance(ReadBufferNode *node) {}

/**
 * 递归清理所有节点
 * */
void ReadBufferTree::Destroy(ReadBufferNode *node) {
    if (node == nullptr) {
        return;
    }
    this->Destroy(node->left);
    this->Destroy(node->right);
    delete node;
}