/**
 * @Author(s):      Pravin and Kassi
 * @filename:       AvlTree.h
 * @date:           04-11-2022
 * @description:    Implementation of an AVL tree
 */

#ifndef INC_22S_FINAL_PROJ_AVLTREE_H
#define INC_22S_FINAL_PROJ_AVLTREE_H

#include <algorithm>
#include <iostream>
#include <queue>
#include "Pair.h"

template<typename K, typename V>
class AvlTree {
private:
    int total_articles = 0;
    int total_tokens = 0;

    //AvlNode class declaration
    class AvlNode {
    public:
        K key;
        std::vector<V> values;
        AvlNode *left;
        AvlNode *right;
        int height;

        explicit AvlNode(const K &key, AvlNode *lt = nullptr, AvlNode *rt = nullptr,
                         int h = 0) : key(key),
                                      left(lt),
                                      right(rt),
                                      height(h) {}
    };

    /// \description    -> Return the node's height.
    int height(AvlNode *&node) { return node != nullptr ? node->height : -1; }

    /// \description    -> Inserts a new node into the AVL tree
    void insert_node(const K &key, const V &value, AvlNode *&node);

    /// \description    -> Search node in the AVL tree
    std::vector<V> *search_node(const K &key, AvlNode *node) const;

    /// \param node     -> A node in the AVL tree
    /// \description    -> Internal function responsible for emptying a node subtrees
    void make_empty(AvlNode *&node);

    /// \param node     -> A node in the AVL tree
    /// \description    -> responsible for cloning subtree
    AvlNode *clone(AvlNode *node) const;

    /// \param alpha     -> Node of imbalance
    /// \description    -> Performs "case 1" rotation
    void rotate_with_left_child(AvlNode *&alpha);


    /// \param alpha     -> Node of imbalance
    /// \description    -> Performs "case 2" rotation
    void double_with_left_child(AvlNode *&alpha);

    /// \param alpha     -> Node of imbalance
    /// \description    -> Performs "case 3" rotation
    void double_with_right_child(AvlNode *&alpha);

    /// \param node     -> Node of imbalance
    /// \description    -> Performs "case 4" rotation
    void rotate_with_right_child(AvlNode *&alpha);


    /// \param  node    -> Node of imbalance
    /// \description    -> balances AVL tree
    void balance(AvlNode *&node);

    //AVL tree root node
    AvlNode *root;
public:

    //constructors
    AvlTree() : root(nullptr) {}

    AvlTree(const AvlTree<K, V> &tree) : root(nullptr) {
        root = clone(tree.root);
    }

    AvlTree(AvlTree<K, V> &&tree) noexcept: root(nullptr) {
        *this = std::move(tree);
    }

    ~AvlTree() {
        make_empty(root);
    }

    AvlTree<K, V> &operator=(AvlTree<K, V> &&tree) noexcept {
        total_articles = tree.total_articles;
        total_tokens = tree.total_tokens;

        if (this != &tree) {
            this->~AvlTree();
            this->root = tree.root;
            tree.root = nullptr;
        }
        return *this;
    }

    /// \param value    -> Value to be added to AVL tree
    /// \description    -> Insert a new node into the AVL tree
    void insert(const K &key, const V &value) {
        insert_node(key, value, root);
    }

    /// \param value    -> Element to find
    /// \return T*      -> Pointer to value or NULL
    /// \description    -> Search
    std::vector<V> *search(const K &key) const {
        return search_node(key,  root);
    }

    /// \param          -> N/A
    /// \return         -> Total documents
    /// \description    -> returns total number of documents in the tree
    int get_total_articles() {     return total_articles;     }

    /// \description    -> Updates the total number of documents in the tree
    void set_total_articles(int new_total_document){
        total_articles = new_total_document;
    }

    /// \param          -> N/A
    /// \description    -> returns total number of documents in the tree
    float get_word_article_ratio() {
        return total_tokens / total_articles;
    }

    /// \param number   -> number to increase total number by
    /// \description    -> Increases total_tokens variable
    void add_tokens(int number) { total_tokens += number; }

    /// \param node             -> Starting node
    /// \return None            -> N/A
    /// \desciption             -> Implement Pre Order traversal method, and populate priority queue for each visit
    void pre_order(AvlNode *node, std::priority_queue<Pair>& queue);

    /// \param None             -> N/A
    /// \return None            ->
    /// \description            -> Prints a list of the most 25 frequent words in the AVL tree to the console
    void proposition_279();
};

template<typename K, typename V>
void AvlTree<K, V>::make_empty(AvlTree::AvlNode *&node) {
    if (node != nullptr) {
        make_empty(node->left);
        make_empty(node->right);
        delete node;
    }
    node = nullptr;
}

template<typename K, typename V>
typename AvlTree<K, V>::AvlNode *AvlTree<K, V>::clone(AvlNode *node) const {
    if (node == nullptr)
        return nullptr;
    else {
        return new AvlNode{node->key, clone(node->left), clone(node->right), node->height};
    }
}

//insert_node implementation
template<typename K, typename V>
void AvlTree<K, V>::insert_node(const K &key, const V &value, AvlNode *&node) {
    if (node == nullptr) {
        node = new AvlNode(key);
        node->values.emplace_back(value);
    } else if (key < node->key) {
        insert_node(key, value, node->left);
    } else if (node->key < key) {
        insert_node(key, value, node->right);
    } else {
        node->values.emplace_back(value);
    }
    balance(node);
}

template<typename K, typename V>
std::vector<V> *AvlTree<K, V>::search_node(const K &key, AvlTree::AvlNode *node) const {
    if (node == nullptr) return nullptr;
    if (node->key < key) {
        return search_node(key, node->right);
    } else if (key < node->key) {
        return search_node(key, node->left);
    } else {
        return &node->values;
    }
}

template<typename K, typename V>
void AvlTree<K, V>::balance(AvlTree::AvlNode *&node) {
    if (node == nullptr) {
        return;
    }
    // Is the height of the left subtree greater than the right subtree?
    if (height(node->left) - height(node->right) > 1) {
        // If so, check if the LL is the node of imbalance
        if (height(node->left->left) >= height(node->left->right)) {
            rotate_with_left_child(node);
        } // Else, the LR node is the node of imbalance
        else {
            double_with_left_child(node);
        }
    }
        // Is the height of the right subtree greater than the left subtree?
    else if (height(node->right) - height(node->left) > 1) {
        if (height(node->right->right) >= height(node->right->left)) {
            // Check if RR is the node of imbalance
            if (height(node->right->right) >= height(node->right->left)) {
                rotate_with_right_child(node);
            } // Else the RL node is the node of imbalance
            else {
                double_with_right_child(node);
            }
        }
    }
    node->height = std::max(height(node->left), height(node->right)) + 1;
}

template<typename K, typename V>
void AvlTree<K, V>::rotate_with_left_child(AvlTree::AvlNode *&alpha) {
    AvlNode *beta = alpha->left;
    alpha->left = beta->right;
    beta->right = alpha;
    alpha->height = std::max(height(alpha->left), height(alpha->right)) + 1;
    beta->height = std::max(height(beta->left), alpha->height) + 1;
    alpha = beta;
}

template<typename K, typename V>
void AvlTree<K, V>::double_with_left_child(AvlTree::AvlNode *&alpha) {
    rotate_with_right_child(alpha->left);
    rotate_with_left_child(alpha);
}

template<typename K, typename V>
void AvlTree<K, V>::double_with_right_child(AvlTree::AvlNode *&alpha) {
    rotate_with_left_child(alpha->right);
    rotate_with_right_child(alpha);
}


template<typename K, typename V>
void AvlTree<K, V>::rotate_with_right_child(AvlTree::AvlNode *&alpha) {
    AvlNode *beta = alpha->right;
    alpha->right = beta->left;
    beta->left = alpha;
    alpha->height = std::max(height(alpha->left), height(alpha->right)) + 1;
    beta->height = std::max(height(beta->right), alpha->height) + 1;
    alpha = beta;
}

template<typename K, typename V>
void AvlTree<K, V>::pre_order(AvlNode *node, std::priority_queue<Pair>& queue){
    //1- Visit all nodes in AVL tree starting from root (PreOrder Traversal)
    if(node != nullptr){
        //2- For each (Make a "pair" object) && (Add to "queue")
        Pair p(node->key, node->values.size());
        queue.push(p);

        //continue visiting and adding to "queue"
        pre_order(node->left, queue);
        pre_order(node->right, queue);
    }
}

template<typename K, typename V>
void AvlTree<K, V>::proposition_279() {
    std::priority_queue<Pair> p_queue;
    pre_order(root, p_queue);
    //3- 25x (Print priority queue top, then pop)

    for(int i = 0; i < 25; i++){
        std::cout << p_queue.top().word << " -> " << p_queue.top().articles << '\n';
        p_queue.pop();
    }
}

#endif //INC_22S_FINAL_PROJ_AVLTREE_H
