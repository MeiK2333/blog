#include "read_buffer_tree.h"

/**
 * 红黑树的四个性质
 * 1. 每个结点要么是红的，要么是黑的；
 * 2. 根结点是黑色的；
 * 3. 如果一个结点是红色的，则它的两个孩子都是黑色的；
 * 4. 对于任意一个结点，其到叶子结点的每条路径上都包含相同数目的黑色结点。
 * 这四条性质可以保证, 到达任何一个叶节点的路径不会比其他任何一个叶节点的路径长一倍
 * 最长的路径: 黑红相间, 每个黑色节点后都跟着一个红色节点
 * 最短的路径: 全为黑色节点
 * */
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

/**
 * 插入节点后重新平衡
 * */
void ReadBufferTree::InsertRebalance(ReadBufferNode *node) {
    /* 如果插入的节点为根节点或者插入节点的父节点为黑色节点, 那么什么也不用做 */
    while (node != this->root_ && node->parent->color == red) {
        /* 首先处理插入节点的父节点为左儿子的情况, 另一种情况的处理类似 */
        if (node->parent == node->parent->parent->left) {
            ReadBufferNode *uncle = node->parent->parent->right;  // 叔节点
            if (uncle && uncle->color == red) {
                /* 
                如果父节点与叔节点都是红的
                那么直接将父节点与叔节点改为黑色, 祖父节点改为红色
                此时祖父节点的子节点都满足红黑树的结构
                然后将祖父节点作为新的当前节点, 继续平衡结构
                */
                node->parent->color = black;
                uncle->color = black;
                node->parent->parent->color = red;
                node = node->parent->parent;
            } else {
                /*
                如果叔节点为黑色或者不存在, 则父节点改为黑色, 祖父节点改为红色
                进行旋转使得祖父节点变成父节点的另一个子节点
                此时恢复红黑树的结构
                */
                if (node == node->parent->right) {
                    node = node->parent;
                    this->RotateLeft(node);
                }
                node->parent->color = black;
                node->parent->parent->color = red;
                this->RotateRight(node->parent->parent);
            }
        } else {  /* 镜像同上 */
            ReadBufferNode *uncle = node->parent->parent->left;
            if (uncle && uncle->color == red) {
                node->parent->color = black;
                uncle->parent->color = black;
                node->parent->parent->color = red;
                node = node->parent->parent;
            } else {
                if (node == node->parent->left) {
                    node = node->parent;
                    this->RotateRight(node);
                }
                node->parent->color = black;
                node->parent->parent->color = red;
                this->RotateLeft(node->parent->parent);
            }
        }
    }
    this->root_->color = black;
}

void ReadBufferTree::RemoveRebalance(ReadBufferNode *node) {}

/**
 * 左旋
 *        node                     r
 *       /    \                   / \
 *      l      r      --->      node rr
 *            / \              /    \
 *           rl  rr           l     rl
 * */
void ReadBufferTree::RotateLeft(ReadBufferNode *node) {
    ReadBufferNode *r = node->right;
    node->right = r->left;
    if (r->left) {
        r->left->parent = node;
    }
    r->parent = node->parent;

    if (node == this->root_) {
        this->root_ = r;
    } else if (node == node->parent->left) {
        node->parent->left = r;
    } else {
        node->parent->right = r;
    }
    r->left = node;
    node->parent = r;
}

/**
 * 右旋
 *        node                     l
 *       /    \                   / \
 *      l      r      --->      ll  node
 *     / \                         /    \
 *    ll  lr                      lr     r
 * */
void ReadBufferTree::RotateRight(ReadBufferNode *node) {
    ReadBufferNode *l = node->left;
    node->left = l->right;
    if (l->right) {
        l->right->parent = node;
    }
    l->parent = node->parent;

    if (node == this->root_) {
        this->root_ = l;
    } else if (node == node->parent->left) {
        node->parent->left = l;
    } else {
        node->parent->right = l;
    }
    l->right = node;
    node->parent = l;
}

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