#pragma once

#include <iostream>
#include <algorithm>
#include <vector>
#include <stack>
#include <queue>
#include <iterator>
#include <fstream>
#include <cstdlib>
#include <stdexcept>
#include <memory>
#include <functional>
#include <string_view>
#include <utility>
#include <new>

namespace Trees
{
    template <typename T, class Compare = std::less<T>>
    class SearchTree
    {
    private:
        struct Node
        {
            T key_;

            Node* parent_ = nullptr;
            Node* left_   = nullptr;
            Node* right_  = nullptr;

            int indx_          = -1;
            int height_        =  1;
            int balanceFactor_ =  0;

            Node (const T& key) : key_(key) {}

            template<class... Args>
            Node (Args&&... key) : key_(std::forward<Args>(key)...) {}

            ~Node () noexcept {}

            Node (const Node& rhs)
            {
                key_    = rhs.key_;
                indx_   = rhs.indx_;
                height_ = rhs.height_;
                balanceFactor_ = rhs.balanceFactor_;
            }

            void swap (Node& rhs)
            {
                std::swap(this->key_, rhs.key_);

                std::swap(this->parent_, rhs.parent_);
                std::swap(this->left_, rhs.left_);
                std::swap(this->right_, rhs.right_);

                std::swap(this->indx_, rhs.indx_);
                std::swap(this->height_, rhs.height_);
                std::swap(this->balanceFactor_, rhs.balanceFactor_);
            }

            Node& operator= (const Node& rhs)
            {
                if (this != &rhs)
                {
                    Node tmp {rhs};
                    swap(rhs);
                }

                return *this;
            }

            Node (Node&& rhs) noexcept { swap(rhs); }

            Node& operator= (SearchTree&& rhs)
            {
                if (this != &rhs) swap(rhs);
                return *this;
            }

        };

        Node* root_ = nullptr;
        Node* first_elem_ = nullptr;

        std::vector<std::unique_ptr<Node>> nodes_;

        Compare key_compare;

        void swap (SearchTree& rhs)
        {
            std::swap(this->root_, rhs.root_);
            std::swap(this->first_elem_, rhs.first_elem_);

            this->nodes_.swap(rhs.nodes_);
        }

        void update (Node* node)
        {
            int leftHeight  = (node->left_  ? node->left_->height_  : 0);
            int rightHeight = (node->right_ ? node->right_->height_ : 0);

            node->balanceFactor_ = leftHeight - rightHeight;
            node->height_        = std::max(leftHeight, rightHeight) + 1;
        }

    public:
        SearchTree () : root_(nullptr) {}

        SearchTree (const SearchTree& rhs) : root_(nullptr)
        {
            if (!rhs.root_) return;

            std::queue<Node*> queue;

            std::unique_ptr<Node> tmp_root_ = std::make_unique<Node>((rhs.root_)->key_);

            Node* tmp_root_ptr = tmp_root_.get();
            nodes_.push_back(std::move(tmp_root_));

            tmp_root_ptr->indx_ = rhs.root_->indx_;

            queue.push(rhs.root_);
            queue.push(tmp_root_ptr);

            while (!queue.empty())
            {
                Node* node     = queue.front();
                queue.pop();
                Node* tmp_node = queue.front();
                queue.pop();

                if (node->left_)
                {
                    std::unique_ptr<Node> tmp_node_left_unique = std::make_unique<Node>((node->left_)->key_);

                    tmp_node->left_ = tmp_node_left_unique.get();
                    nodes_.push_back(std::move(tmp_node_left_unique));

                    tmp_node->left_->parent_ = tmp_node;

                    queue.push(node->left_);
                    queue.push(tmp_node->left_);
                }
                if (node->right_)
                {
                    std::unique_ptr<Node> tmp_node_right_unique = std::make_unique<Node>((node->right_)->key_);

                    tmp_node->right_ = tmp_node_right_unique.get();
                    nodes_.push_back(std::move(tmp_node_right_unique));

                    tmp_node->right_->parent_ = tmp_node;

                    queue.push(node->right_);
                    queue.push(tmp_node->right_);
                }
            }

            Node* tmp_first_elem_ = tmp_root_ptr;
            while (tmp_first_elem_->left_) tmp_first_elem_ = tmp_first_elem_->left_;

            root_ = tmp_root_ptr;
            first_elem_ = tmp_first_elem_;
        }

        SearchTree& operator= (const SearchTree& rhs)
        {
            if (this != &rhs)
            {
                SearchTree tmp {rhs};
                swap (tmp);
            }

            return *this;
        }

// -----
        SearchTree (SearchTree&& rhs) noexcept { swap(rhs); }

        SearchTree& operator= (SearchTree&& rhs)
        {
            if (this != &rhs) swap(rhs);
            return *this;
        }

// -----
        int  size  () const { return nodes_.size(); }
        bool empty () const { return nodes_.empty(); }

// -----
    private:
        Node* rotate_left (Node*& node)
        {
            Node* tmp = node->right_;
            tmp->parent_ = node->parent_;

            node->right_ = tmp->left_;
            if (node->right_) node->right_->parent_ = node;

            node->parent_ = tmp;
            tmp->left_    = node;

            update(tmp->left_);
            update(tmp);

            return tmp;
        }

        Node* rotate_right (Node*& node)
        {
            Node* tmp = node->left_;
            tmp->parent_ = node->parent_;

            node->left_ = tmp->right_;
            if (node->left_) node->left_->parent_ = node;

            node->parent_ = tmp;
            tmp->right_   = node;

            update(tmp->right_);
            update(tmp);

            return tmp;
        }

        Node* balance (Node*& node)
        {
            update(node);

            if (node->balanceFactor_ > 1)
            {
                if (node->left_ && node->left_->balanceFactor_ < 0)
                {
                    node->left_ = rotate_left(node->left_);
                }
                return rotate_right(node);
            }
            if (node->balanceFactor_ < -1)
            {
                if (node->right_ && node->right_->balanceFactor_ > 0)
                {
                    node->right_ = rotate_right(node->right_);
                }
                return rotate_left(node);
            }

            return node;
        }

        void rebalance (Node*& inserted_node, Node*& root)
        {
            while (inserted_node != root)
            {
                Node* parent = inserted_node->parent_;

                if (parent->left_)  parent->left_  = balance(parent->left_);
                if (parent->right_) parent->right_ = balance(parent->right_);

                inserted_node = parent;
            }
        }

// -----
    private:
        Node* create_node (const T& key)
        {
            std::unique_ptr<Node> new_node = std::make_unique<Node>(key);
            new_node->indx_ = nodes_.size();

            first_elem_ = (!first_elem_)                        ? new_node.get() : first_elem_;
            first_elem_ = (key_compare(key, first_elem_->key_)) ? new_node.get() : first_elem_;

            Node* new_node_ptr = new_node.get();
            nodes_.push_back(std::move(new_node));

            return new_node_ptr;
        }

        template<class... Args>
        Node* create_node (Args&&... key)
        {
            std::unique_ptr<Node> new_node = std::make_unique<Node>(std::forward<Args>(key)...);
            new_node->indx_ = nodes_.size();

            first_elem_ = (!first_elem_)                                   ? new_node.get() : first_elem_;
            first_elem_ = (key_compare(new_node->key_, first_elem_->key_)) ? new_node.get() : first_elem_;

            Node* new_node_ptr = new_node.get();
            nodes_.push_back(std::move(new_node));

            return new_node_ptr;
        }

        void add_new_node (const T& key, Node*& new_node, Node* curr_node_ptr)
        {
            bool left_less = false;

            while ((left_less = key_compare(key, curr_node_ptr->key_)) && curr_node_ptr->left_ ||
                    key_compare(curr_node_ptr->key_, key) && curr_node_ptr->right_)
            {
                if (left_less) curr_node_ptr = curr_node_ptr->left_;
                else           curr_node_ptr = curr_node_ptr->right_;
            }

            if (left_less)
            {
                curr_node_ptr->left_ = new_node;
                curr_node_ptr->left_->parent_ = curr_node_ptr;
            }
            else
            {
                curr_node_ptr->right_ = new_node;
                curr_node_ptr->right_->parent_ = curr_node_ptr;
            }
        }

        Node* add_new_to_avl (const T& key, Node*& new_node, Node*& root__)
        {
            Node* curr_node_ptr = root__;
            add_new_node (key, new_node, curr_node_ptr);

            Node* root = root__;
            rebalance(curr_node_ptr, root);

            return balance(root__);
        }

        Node* insert_node (const T& key, Node*& node)
        {
            if (!node)
            {
                node = create_node(key);
                return node;
            }

            Node* new_node = create_node(key);

            return add_new_to_avl(key, new_node, node);
        }

        template<class... Args>
        Node* emplace_node (Node*& node, Args&&... key)
        {
            if (!node)
            {
                node = create_node(std::forward<Args>(key)...);
                return node;
            }

            Node* new_node = create_node(std::forward<Args>(key)...);

            return add_new_to_avl(new_node->key_, new_node, node);
        }

    public:
        void insert (const T& key)
        {
            Node* inserted_node = insert_node(key, root_);
            root_ = inserted_node;
        }

        template<class... Args>
        void emplace (Args&&... args)
        {
            Node* inserted_node = emplace_node(root_, std::forward<Args>(args)...);
            root_ = inserted_node;
        }

// -----
        class iterator;

        iterator begin () const { return iterator {first_elem_}; }
        iterator end   () const { return iterator {nullptr};     }

        iterator lower_bound (const T& key) const
        {
            Node* node = root_;
            Node* res  = nullptr;

            while (node)
            {
                if (node->key_ >= key)
                {
                    res = node;
                    node = node->left_;
                }
                else node = node->right_;
            }

            return iterator {res};
        }

        iterator upper_bound (const T& key) const
        {
            Node* node = root_;
            Node* res  = nullptr;

            while (node)
            {
                if (node->key_ > key)
                {
                    res = node;
                    node = node->left_;
                }
                else node = node->right_;
            }

            return iterator {res};
        }

        class iterator
        {
        private:
            const Node* node_;

            void advance_fwd ()
            {
                if (node_->right_)
                {
                    node_ = node_->right_;
                    while (node_->left_) node_ = node_->left_;
                }
                else
                {
                    Node* parent = node_->parent_;
                    while (parent && node_ == parent->right_)
                    {
                        node_ = parent;
                        parent = parent->parent_;
                    }
                    node_ = parent;
                }
            }

            void advance_bwd ()
            {
                if (node_->left_)
                {
                    node_ = node_->left_;
                    while (node_->right_) node_ = node_->right_;
                }
                else
                {
                    Node* parent = node_->parent_;
                    while (parent && node_ == parent->left_)
                    {
                        node_ = parent;
                        parent = parent->parent_;
                    }
                    node_ = parent;
                }
            }

        public:
            using iterator_category = std::bidirectional_iterator_tag;
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using pointer = Node*;
            using reference = T&;

            explicit iterator (const Node* node) : node_(node) {}

            explicit operator bool () { return node_; }

            const T& operator* () const
            {
                if (node_) return node_->key_;

                throw std::runtime_error("null pointer dereference");
            }

            iterator& operator++ ()
            {
                advance_fwd();
                return *this;
            }

            iterator operator++ (int)
            {
                auto tmp = *this;
                advance_fwd();
                return tmp;
            }

            iterator& operator-- ()
            {
                advance_bwd();
                return *this;
            }

            iterator operator-- (int)
            {
                auto tmp = *this;
                advance_bwd();
                return tmp;
            }

            bool operator== (const iterator& rhs) const { return node_ == rhs.node_; }
            bool operator!= (const iterator& rhs) const { return node_ != rhs.node_; }
        };

// -----
        void tree_dump (std::string_view filename) const
        {
            std::ofstream file(filename.data());
            file << "digraph G {" << std::endl << "node [shape = record];" << std::endl;

            if (root_) node_dump(file);

            file << "}";
            file.close();

            std::string command;
            command.append("dot -T png ");
            command.append(filename);
            command.append(" -o ./pic/tree.png");

            std::system (command.c_str());
        }

        void node_dump (std::ofstream& file) const
        {
            if (!root_) return;

            std::stack<Node*> stack;
            stack.push(root_);

            bool is_root = true;
            while (!stack.empty())
            {
                Node* node = stack.top();

                file << node->indx_ << "[shape = doubleoctagon, style = filled, fillcolor = cornflowerblue "
                                       "label = \"" << node->key_ << "\"]" << std::endl;

                if (!is_root) file << node->parent_->indx_ << " -> " << node->indx_ << std::endl;
                else          is_root = false;

                stack.pop();

                if (node->right_) stack.push(node->right_);
                if (node->left_)  stack.push(node->left_);
            }
        }
    };
}
