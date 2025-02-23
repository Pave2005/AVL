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

namespace Trees
{
    template <typename T, class Compare = std::less<T>>
    class SearchTree
    {
    private:
        struct Node
        {
            T key_;

            Node* parent_                = nullptr;
            std::unique_ptr<Node> left_  = nullptr;
            std::unique_ptr<Node> right_ = nullptr;

            int indx_          = -1;
            int height_        =  1;
            int balanceFactor_ =  0;

            Node (const T& key) : key_(key) {}

            template<class... Args>
            Node (Args&&... key) : key_(std::forward<Args>(key)...) {}

            Node (const Node& rhs)
            {
                key_    = rhs.key_;
                indx_   = rhs.indx_;
                height_ = rhs.height_;
                balanceFactor_ = rhs.balanceFactor_;
            }

            void node_swap (Node& rhs)
            {
                std::swap(this->key_, rhs.key_);
                std::swap(this->indx_, rhs.indx_);
                std::swap(this->height_, rhs.height_);
                std::swap(this->balanceFactor_, rhs.balanceFactor_);
            }

            Node& operator= (const Node& rhs)
            {
                if (this != &rhs)
                {
                    Node tmp {rhs};
                    node_swap (rhs);
                }

                return *this;
            }
        };

        std::unique_ptr<Node> root_ {};
        Node* first_elem_ = nullptr;

        size_t size_ = 0;

        Compare key_compare;

        void tree_swap (SearchTree& rhs)
        {
            std::swap(this->size_, rhs.size_);
            std::swap(this->root_, rhs.root_);
            std::swap(this->first_elem_, rhs.first_elem_);
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

        ~SearchTree ()
        {
            Node* node_ptr = root_.get();

            if (!node_ptr) return;

            while (node_ptr->left_ || node_ptr->right_ || node_ptr->parent_)
            {
                while (node_ptr->left_) node_ptr = node_ptr->left_.get();

                if (!node_ptr->right_)
                {
                    while (!node_ptr->right_ && node_ptr->parent_)
                    {
                        node_ptr = node_ptr->parent_;
                        if (node_ptr->left_)
                        {
                            node_ptr->left_.reset();
                        }
                        else if (node_ptr->right_)
                        {
                            node_ptr->right_.reset();
                        }
                    }

                    if (node_ptr->right_) node_ptr = node_ptr->right_.get();
                }
                else
                {
                    node_ptr = node_ptr->right_.get();
                }
            }
        }

        SearchTree (const SearchTree& rhs) : root_(nullptr)
        {
            if (!rhs.root_) return;

            std::queue<Node*> queue;

            std::unique_ptr<Node> tmp_root_ = std::make_unique<Node>((rhs.root_)->key_);
            tmp_root_->indx_ = rhs.root_->indx_;

            queue.push(rhs.root_.get());
            queue.push(tmp_root_.get());

            while (!queue.empty())
            {
                Node* node     = queue.front();
                queue.pop();
                Node* tmp_node = queue.front();
                queue.pop();

                if (node->left_)
                {
                    tmp_node->left_ = std::make_unique<Node>((node->left_)->key_);
                    tmp_node->left_->parent_ = tmp_node;

                    queue.push(node->left_.get());
                    queue.push(tmp_node->left_.get());
                }
                if (node->right_)
                {
                    tmp_node->right_ = std::make_unique<Node>((node->right_)->key_);
                    tmp_node->right_->parent_ = tmp_node;

                    queue.push(node->right_.get());
                    queue.push(tmp_node->right_.get());
                }
            }

            size_t tmp_size_ = rhs.size_;

            Node* tmp_first_elem_ = tmp_root_.get();
            while (tmp_first_elem_->left_) tmp_first_elem_ = tmp_first_elem_->left_.get();

            size_ = tmp_size_;
            root_ = std::move(tmp_root_);
            first_elem_ = tmp_first_elem_;
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
        SearchTree (SearchTree&& rhs) noexcept { tree_swap(rhs); }

        SearchTree& operator= (SearchTree&& rhs)
        {
            if (this != &rhs) tree_swap (rhs);
            return *this;
        }

// -----
        int  size  () const { return size_;      }
        bool empty () const { return size_ == 0; }

// -----
    private:
        std::unique_ptr<Node> rotate_left (std::unique_ptr<Node>& node)
        {
            std::unique_ptr<Node> tmp = std::move(node->right_);
            tmp->parent_ = node->parent_;

            node->right_ = std::move(tmp->left_);
            if (node->right_) node->right_->parent_ = node.get();

            node->parent_ = tmp.get();
            tmp->left_    = std::move(node);

            update((tmp->left_).get());
            update(tmp.get());

            return std::move(tmp);
        }

        std::unique_ptr<Node> rotate_right (std::unique_ptr<Node>& node)
        {
            std::unique_ptr<Node> tmp = std::move(node->left_);
            tmp->parent_ = node->parent_;

            node->left_ = std::move(tmp->right_);
            if (node->left_) node->left_->parent_ = node.get();

            node->parent_ = tmp.get();
            tmp->right_   = std::move(node);

            update((tmp->right_).get());
            update(tmp.get());

            return std::move(tmp);
        }

        std::unique_ptr<Node> balance (std::unique_ptr<Node>& node)
        {
            update(node.get());

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

            return std::move(node);
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
        std::unique_ptr<Node> create_node (const T& key)
        {
            std::unique_ptr<Node> new_node = std::make_unique<Node>(key);
            new_node->indx_ = size_;
            ++size_;

            first_elem_ = (!first_elem_)                        ? new_node.get() : first_elem_;
            first_elem_ = (key_compare(key, first_elem_->key_)) ? new_node.get() : first_elem_;

            return std::move(new_node);
        }

        template<class... Args>
        std::unique_ptr<Node> create_node (Args&&... key)
        {
            std::unique_ptr<Node> new_node = std::make_unique<Node>(std::forward<Args>(key)...);
            new_node->indx_ = size_;
            ++size_;

            first_elem_ = (!first_elem_)                                   ? new_node.get() : first_elem_;
            first_elem_ = (key_compare(new_node->key_, first_elem_->key_)) ? new_node.get() : first_elem_;

            return std::move(new_node);
        }

        void add_new_node (const T& key, std::unique_ptr<Node>& new_node, Node* curr_node_ptr)
        {
            bool left_less = false;

            while ((left_less = key_compare(key, curr_node_ptr->key_)) && curr_node_ptr->left_ ||
                    key_compare(curr_node_ptr->key_, key) && curr_node_ptr->right_)
            {
                if (left_less) curr_node_ptr = curr_node_ptr->left_.get();
                else           curr_node_ptr = curr_node_ptr->right_.get();
            }

            if (left_less)
            {
                curr_node_ptr->left_ = std::move(new_node);
                curr_node_ptr->left_->parent_ = curr_node_ptr;
            }
            else
            {
                curr_node_ptr->right_ = std::move(new_node);
                curr_node_ptr->right_->parent_ = curr_node_ptr;
            }
        }

        std::unique_ptr<Node> add_new_to_avl (const T& key, std::unique_ptr<Node>& new_node, std::unique_ptr<Node>& root__)
        {
            Node* curr_node_ptr = root__.get();
            add_new_node (key, new_node, curr_node_ptr);

            Node* root = root__.get();
            rebalance(curr_node_ptr, root);

            return balance(root__);
        }

        std::unique_ptr<Node> insert_node (const T& key, std::unique_ptr<Node>& node)
        {
            if (!node)
            {
                node = create_node(key);
                return std::move(node);
            }

            std::unique_ptr<Node> new_node = create_node(key);

            return add_new_to_avl (key, new_node, node);
        }

        template<class... Args>
        std::unique_ptr<Node> emplace_node (std::unique_ptr<Node>& node, Args&&... key)
        {
            if (!node)
            {
                node = create_node(std::forward<Args>(key)...);
                return std::move(node);
            }

            std::unique_ptr<Node> new_node = create_node(std::forward<Args>(key)...);

            return add_new_to_avl (new_node->key_, new_node, node);
        }

    public:
        void insert (const T& key)
        {
            std::unique_ptr<Node> inserted_node = insert_node(key, root_);
            root_ = std::move(inserted_node);
        }

        template<class... Args>
        void emplace (Args&&... args)
        {
            std::unique_ptr<Node> inserted_node = emplace_node(root_, std::forward<Args>(args)...);
            root_ = std::move(inserted_node);
        }

// -----
        class iterator;

        iterator begin () const { return iterator {first_elem_}; }
        iterator end   () const { return iterator {nullptr};     }

        iterator lower_bound (const T& key) const
        {
            Node* node = root_.get();
            Node* res  = nullptr;

            while (node)
            {
                if (node->key_ >= key)
                {
                    res = node;
                    node = node->left_.get();
                }
                else node = node->right_.get();
            }

            return iterator {res};
        }

        iterator upper_bound (const T& key) const
        {
            Node* node = root_.get();
            Node* res  = nullptr;

            while (node)
            {
                if (node->key_ > key)
                {
                    res = node;
                    node = node->left_.get();
                }
                else node = node->right_.get();
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
                    node_ = node_->right_.get();
                    while (node_->left_) node_ = node_->left_.get();
                }
                else
                {
                    Node* parent = node_->parent_;
                    while (parent && node_ == parent->right_.get())
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
                    node_ = node_->left_.get();
                    while (node_->right_) node_ = node_->right_.get();
                }
                else
                {
                    Node* parent = node_->parent_;
                    while (parent && node_ == parent->left_.get())
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
                advance_fwd ();
                return *this;
            }

            iterator operator++ (int)
            {
                auto tmp = *this;
                advance_fwd ();
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
                advance_bwd ();
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
            stack.push(root_.get());

            bool is_root = true;
            while (!stack.empty())
            {
                Node* node = stack.top();

                file << node->indx_ << "[shape = doubleoctagon, style = filled, fillcolor = cornflowerblue "
                                       "label = \"" << node->key_ << "\"]" << std::endl;

                if (!is_root) file << node->parent_->indx_ << " -> " << node->indx_ << std::endl;
                else          is_root = false;

                stack.pop();

                if (node->right_) stack.push(node->right_.get());
                if (node->left_)  stack.push(node->left_.get());
            }
        }
    };
}
