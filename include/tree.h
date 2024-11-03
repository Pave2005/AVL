#pragma once

#include <iostream>
#include <algorithm>
#include <vector>
#include <stack>
#include <iterator>
#include <fstream>
#include <cstdlib>
#include <stdexcept>
#include <memory>

namespace Trees
{
    template <typename T>
    class SearchTree
    {
    private:
        struct Node
        {
            T key_;

            std::shared_ptr<Node> parent_ = nullptr;
            std::shared_ptr<Node> left_   = nullptr;
            std::shared_ptr<Node> right_  = nullptr;

            int indx_          = -1;
            int height_        =  1;
            int balanceFactor_ =  0;

            Node(T key) : key_(key) {}

            void update ()
            {
                int leftHeight  = (left_  ? left_->height_  : 0);
                int rightHeight = (right_ ? right_->height_ : 0);
                balanceFactor_  = leftHeight - rightHeight;
                height_         = std::max(leftHeight, rightHeight) + 1;
            }

            std::partial_ordering operator<=> (const T& rhs) { return rhs <=> key_; }
        };

        std::vector <std::shared_ptr<Node>> nodes_ {};
        std::shared_ptr<Node> root_ = nullptr;

        std::shared_ptr<Node> first_elem_ = nullptr;

    public:
        SearchTree () : nodes_({}), root_(nullptr) {}

        SearchTree (std::vector<T>& data) : nodes_({}), root_(nullptr)
        {
            for (auto&& elem : data) insert (elem);
        }

        ~SearchTree () {}

// -----
        void tree_swap (SearchTree& rhs)
        {
            std::swap(this->root_, rhs.root_);
            std::swap(this->nodes_, rhs.nodes_);
        }

        SearchTree (const SearchTree& rhs) : nodes_(rhs.nodes_.size()), root_(nullptr)
        {
            std::vector <std::shared_ptr<Node>> tmp_nodes_ {};

            tmp_nodes_.reserve(rhs.nodes_.size());

            for (int i = 0; i < rhs.nodes_.size(); i++)
            {
                std::shared_ptr<Node> node {std::make_shared<Node>(rhs.nodes_[i]->key_)};
                tmp_nodes_.emplace_back(node);
                tmp_nodes_[i]->indx_ = i;
            }

            for (auto&& node : tmp_nodes_)
            {
                if (rhs.nodes_[node->indx_]->parent_)
                    node->parent_ = nodes_[rhs.nodes_[node->indx_]->parent_->indx_];

                if (rhs.nodes_[node->indx_]->left_)
                    node->left_ = nodes_[rhs.nodes_[node->indx_]->left_->indx_];

                if (rhs.nodes_[node->id_]->right_)
                    node->right_ = nodes_[rhs.nodes_[node->indx_]->right_->indx_];
            }

            std::shared_ptr<Node> tmp_root_ = nodes_[rhs.root_->indx_];

            std::swap(tmp_nodes_, nodes_);
            std::swap(tmp_root_, root_);
        }

        SearchTree& operator= (const SearchTree& rhs)
        {
            if (this != &rhs)
            {
                SearchTree tmp {rhs};
                tree_swap (tmp);
            }
            return *this;
        }

// -----
        SearchTree (SearchTree&& rhs) noexcept { tree_swap (rhs); }

        SearchTree& operator= (SearchTree&& rhs)
        {
            if (this != &rhs) tree_swap (rhs);
            return *this;
        }

// -----
        int  size  () const { return nodes_.size();  }
        bool empty () const { return nodes_.empty(); }

// -----
        std::shared_ptr<Node> rotate_left (std::shared_ptr<Node> node)
        {
            std::shared_ptr<Node> tmp = node->right_;
            tmp->parent_ = node->parent_;

            node->right_ = tmp->left_;
            if (tmp->left_) tmp->left_->parent_ = node;

            tmp->left_    = node;
            node->parent_ = tmp;

            if (node == root_) root_ = tmp;

            node->update ();
            tmp->update  ();

            return tmp;
        }

        std::shared_ptr<Node> rotate_right (std::shared_ptr<Node> node)
        {
            std::shared_ptr<Node> tmp = node->left_;
            tmp->parent_ = node->parent_;

            node->left_ = tmp->right_;
            if (tmp->right_) tmp->right_->parent_ = node;

            tmp->right_   = node;
            node->parent_ = tmp;

            if (node == root_) root_ = tmp;

            node->update ();
            tmp->update  ();

            return tmp;
        }

        std::shared_ptr<Node> balance (std::shared_ptr<Node> node)
        {
            node->update ();

            if (node->balanceFactor_ > 1)
            {
                if (node->left_ && node->left_->balanceFactor_ < 0)
                {
                    node->left_ = rotate_left (node->left_);
                }
                return rotate_right (node);
            }
            if (node->balanceFactor_ < -1)
            {
                if (node->right_ && node->right_->balanceFactor_ > 0)
                {
                    node->right_ = rotate_right (node->right_);
                }
                return rotate_left (node);
            }
            return node;
        }

// -----
        std::shared_ptr<Node> insert_node (T key, std::shared_ptr<Node> node)
        {
            if (!node)
            {
                node = std::make_shared<Node>(key);
                node->indx_ = nodes_.size();
                nodes_.push_back(node);

                first_elem_ = (!first_elem_) ? node : first_elem_;
                first_elem_ = (first_elem_->key_ > key) ? node : first_elem_;

                return node;
            }

            if (key < node->key_)
            {
                node->left_ = insert_node (key, node->left_);
                node->left_->parent_ = node;
            }
            else if (key > node->key_)
            {
                node->right_ = insert_node (key, node->right_);
                node->right_->parent_ = node;
            }

            return balance (node);
        }

        std::shared_ptr<Node> extract_min (std::shared_ptr<Node> node)
        {
            if (node->left_)
            {
                node->left_ = extract_min (node->left_);
                return balance (node);
            }
            return node->right_;
        }

        std::shared_ptr<Node> extract_node (T key, std::shared_ptr<Node> node)
        {
            if (!node) return nullptr;

            if (key < node->key_)      node->left_ = extract_node (key, node->left_);
            else if (key > node->key_) node->right_ = extract_node (key, node->right_);
            else
            {
                std::shared_ptr<Node> left  = node->left_;
                std::shared_ptr<Node> right = node->right_;

                nodes_.back()->indx_ = node->indx_;
                nodes_[node->indx_] = nodes_.back();
                nodes_.pop_back();

                if (node == root_) root_ = nullptr;

                if (!right) return left;

                std::shared_ptr<Node> min = right;
                for (; min->left_ != nullptr; min = min->left_);

                min->right_ = extract_min (right);
                min->left_  = left;

                return balance (min);
            }
        }

        void insert (T key)
        {
            std::shared_ptr<Node> inserted_node = insert_node (key, root_);
            if (nodes_.size() == 1) root_ = inserted_node;
        }

        void extract (const T& key)
        {
            std::shared_ptr<Node> extracted_node = extract_node (key, root_);
            if (!root_) root_ = extracted_node;
        }

// -----
        class iterator;

        iterator begin () const { return iterator {this, first_elem_}; }
        iterator end   () const { return iterator {this, nullptr};     }

        iterator lower_bound (const T& key) const
        {
            std::shared_ptr<Node> node = root_;
            std::shared_ptr<Node> res  = nullptr;

            while (node)
            {
                if (node->key_ >= key)
                {
                    res = node;
                    node = node->left_;
                }
                else node = node->right_;
            }

            return iterator {this, res};
        }

        iterator upper_bound (const T& key) const
        {
            std::shared_ptr<Node> node = root_;
            std::shared_ptr<Node> res  = nullptr;

            while (node)
            {
                if (node->key_ > key)
                {
                    res = node;
                    node = node->left_;
                }
                else node = node->right_;
            }

            return iterator {this, res};
        }

        class iterator
        {
        private:
            SearchTree* tree_ = nullptr;
            std::shared_ptr<Node> node_;

            void advance_fwd ()
            {
                if (node_->right_)
                {
                    node_ = node_->right_;
                    while (node_->left_) node_ = node_->left_;
                }
                else
                {
                    std::shared_ptr<Node> parent = node_->parent_;
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
                    std::shared_ptr<Node> parent = node_->parent_;
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
            using pointer = std::shared_ptr<Node>;
            using reference = T&;

            explicit iterator (const SearchTree* tree, std::shared_ptr<Node> node) : tree_(const_cast<SearchTree*>(tree)), node_(node) {}

            explicit operator bool () { return node_; }

            const T& operator* () const
            {
                if (node_) return node_->key_;

                throw std::runtime_error("null pointer dereference");
            }

            iterator& operator++ ()
            {
                advance_fwd ();
                return *this;
            }

            iterator operator++ (int)
            {
                auto tmp = *this;
                (*this)++;
                return tmp;
            }

            iterator& operator-- ()
            {
                advance_bwd ();
                return *this;
            }

            iterator operator-- (int)
            {
                auto tmp = *this;
                (*this)--;
                return tmp;
            }

            bool operator== (const iterator& rhs) const { return node_ == rhs.node_; }
            bool operator!= (const iterator& rhs) const { return node_ != rhs.node_; }
        };

// -----
        void tree_dump (const std::string& filename) const
        {
            std::ofstream file(filename);
            file << "digraph G {" << std::endl << "node [shape = record];" << std::endl;

            if (root_) node_dump (file, root_);

            file << "}";
            file.close();

            std::string command = "dot -T png " + filename + " -o ./pic/tree.png";
            std::system (command.c_str());
        }

        void node_dump (std::ofstream& file, std::shared_ptr<Node> root) const
        {
            if (!root) return;

            std::stack<std::shared_ptr<Node>> stack;
            stack.push(root);

            bool is_root = true;
            while (!stack.empty())
            {
                std::shared_ptr<Node> node = stack.top();

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
