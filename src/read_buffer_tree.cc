#include "read_buffer_tree.h"

/**
 * 红黑树的四个性质
 * 1. 每个结点要么是红的，要么是黑的；
 * 2. 根结点是黑色的；
 * 3. 如果一个结点是红色的，则它的两个孩子都是黑色的；
 * 4. 对于任意一个结点，其到叶子结点的每条路径上都包含相同数目的黑色结点。
 * 这四条性质可以保证
 * 到达任何一个叶节点的路径不会比其他任何一个叶节点的路径长一倍
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

    /* 判断是否为根节点 */
    if (parent != nullptr) {
        if (current->key < parent->key) {
            parent->left = current;
        } else {
            parent->right = current;
        }
    } else {
        this->root_ = current;
    }

    /* 前两层不需要重建 */
    if (parent != nullptr && parent->parent != nullptr) {
        this->InsertRebalance(current);
    }
    this->root_->color = black;
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
                break;
            }
        } else { /* 镜像同上 */
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
                break;
            }
        }
    }
}

/**
 * 删除节点后重新平衡
 * 并非是直接删除, 而是找到一个用于替换的"当前节点"
 * 替换要删除的节点, 然后重新平衡红黑树
 * 参考: https://subetter.com/articles/2018/06/rb-tree.html
 * */
void ReadBufferTree::RemoveRebalance(ReadBufferNode *node) {
    ReadBufferNode *x, *y, *x_parent;
    x = x_parent = nullptr;
    y = node;

    /* 首先进行节点的删除 */
    if (y->left == nullptr) {
        x = y->right;
    } else if (y->right == nullptr) {
        x = y->left;
    } else {
        /* 如果两个子节点都有, 则寻找右子树的最小值, 用于之后的替换 */
        y = y->right;
        while (y->left) {
            y = y->left;
        }
        x = y->right;
    }

    if (y != node) { /* 如果要删除的节点有两个子节点 */
        /* 移动 y 以覆盖 node */
        node->left->parent = y;
        y->left = node->left;

        if (y != node->right) {
            x_parent = y->parent;
            if (x) {
                x->parent = y->parent;
            }
            y->parent->left = nullptr;
            y->right = node->right;
            node->right->parent = y;
        } else {
            x_parent = y;
        }

        if (this->root_ == node) {
            this->root_ = y;
        } else if (node->parent->left == node) {
            node->parent->left = y;
        } else {
            node->parent->right = y;
        }
    } else { /* 如果要删除的节点没有子节点或者只有一个 */
        x_parent = y->parent;
        if (x) {
            x->parent = y->parent;
        }

        if (this->root_ == node) {
            this->root_ = x;
        } else if (node->parent->left == node) {
            node->parent->left = x;
        } else {
            node->parent->right = x;
        }
    }

    /* 开始修复红黑树结构, 如果删除的节点为红色节点, 那么对红黑树结构没有影响 */
    if (y->color == black) {
        if (x != this->root_ && (x == nullptr || x->color == black)) {
            if (x == x_parent->left) {
                ReadBufferNode *w = x_parent->right;
                /* Case 1*/
                if (w->color == red) {
                    w->color = black;
                    x_parent->color = red;
                    this->RotateLeft(x_parent);
                    w = x_parent->right;
                }

                /* Case 2 */
                if ((w->left == nullptr || w->left->color == black) &&
                    (w->right == nullptr || w->right->color == black)) {
                    w->color = red;
                    x = x_parent;
                    x_parent = x_parent->parent;
                } else {
                    /* Case 3 */
                    if (w->right == nullptr || w->right->color == black) {
                        if (w->left) {
                            w->left->color = black;
                        }
                        w->color = red;
                        this->RotateRight(w);
                        w = x_parent->right;
                    }

                    /* Case 4 */
                    w->color = x_parent->color;
                    x_parent->color = black;
                    if (w->right) {
                        w->right->color = black;
                    }
                    this->RotateLeft(x_parent);
                }
            } else { /* 镜像操作 */
                ReadBufferNode *w = x_parent->left;

                if (w->color == red) {
                    w->color = black;
                    x_parent->color = red;
                    this->RotateRight(x_parent);
                    w = x_parent->left;
                }

                if ((w->right == nullptr || w->right->color == black) &&
                    (w->left == nullptr || w->left->color == black)) {
                    w->color = red;
                    x = x_parent;
                    x_parent = x_parent->parent;
                } else {
                    if (w->left == nullptr || w->left->color == black) {
                        if (w->right) {
                            w->right->color = black;
                        }
                        w->color = red;
                        this->RotateLeft(w);
                        w = x_parent->left;
                    }

                    w->color = x_parent->color;
                    x_parent->color = black;
                    if (w->left) {
                        w->left->color = black;
                    }
                    this->RotateRight(x_parent);
                }
            }
        }

        if (x) {
            x->color = black;
        }
    }
}

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