#pragma once

#include <iostream>
#include <new>
#include <algorithm>
#include <vector>
#include <stack>
#include <iterator>
#include <fstream>
#include <cstdlib>
#include <stdexcept>

namespace Trees
{
    template <typename T>
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

            Node(T key) : key_(key) {}

            void update ()
            {
                int leftHeight  = (left_  ? left_->height_  : 0);
                int rightHeight = (right_ ? right_->height_ : 0);
                balanceFactor_  = leftHeight - rightHeight;
                height_         = std::max(leftHeight, rightHeight) + 1;
            }
        };

        std::vector <Node*> nodes_ {};
        Node* root_ = nullptr;

    public:
        SearchTree () : nodes_({}), root_(nullptr) {}

        SearchTree (std::vector<T>& data) : nodes_({}), root_(nullptr)
        {
            for (auto&& elem : data) insert (elem);
        }

        ~SearchTree ()
        {
            for (auto* node : nodes_) delete node;
        }

// -----
        void tree_swap (SearchTree& rhs)
        {
            std::swap(this->root_, rhs.root_);
            std::swap(this->nodes_, rhs.nodes_);
        }

        SearchTree (const SearchTree& rhs) : nodes_(rhs.nodes_.size()), root_(nullptr)
        {
            std::vector <Node*> tmp_nodes_ {};

            tmp_nodes_.reserve(rhs.nodes_.size());

            for (int i = 0; i < rhs.nodes_.size(); i++)
            {
                tmp_nodes_.emplace_back(new Node {rhs.nodes_[i]->key_ });
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

            Node* tmp_root_ = nodes_[rhs.root_->indx_];

            std::swap(tmp_nodes_, nodes_);
            std::swap(tmp_root_, root_);
        }

        SearchTree& operator= (SearchTree& rhs)
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
        Node* rotate_left (Node* node)
        {
            Node* tmp    = node->right_;
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

        Node* rotate_right (Node* node)
        {
            Node* tmp = node->left_;
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

        Node* balance (Node* node)
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
        Node* insert_node (T key, Node* node)
        {
            if (!node)
            {
                node = new Node {key};
                node->indx_ = nodes_.size();
                nodes_.push_back(node);
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

        Node* extract_min (Node* node)
        {
            if (node->left_)
            {
                node->left_ = extract_min (node->left_);
                return balance (node);
            }
            return node->right_;
        }

        Node* extract_node (T key, Node* node)
        {
            if (!node) return nullptr;

            if (key < node->key_)
                node->left_ = extract_node (key, node->left_);
            else if (key > node->key_)
                node->right_ = extract_node (key, node->right_);
            else
            {
                Node* left  = node->left_;
                Node* right = node->right_;

                nodes_.back()->indx_ = node->indx_;
                nodes_[node->indx_] = nodes_.back();
                nodes_.pop_back();

                if (node == root_) root_ = nullptr;

                delete node;

                if (!right) return left;

                Node* min = right;
                for (; min->left_ != nullptr; min = min->left_);

                min->right_ = extract_min (right);
                min->left_  = left;

                return balance (min);
            }
        }

        void insert (T key)
        {
            try
            {
                Node* inserted_node = insert_node (key, root_);
                if (nodes_.size() == 1) root_ = inserted_node;
            }
            catch (const char* error_message)
            {
                std::cout << error_message << std::endl;
                exit (1);
            }
        }

        void extract (const T& key)
        {
            Node* extracted_node = extract_node (key, root_);
            if (!root_) root_ = extracted_node;
        }

// -----
        class iterator;
        friend iterator get_iterator (const SearchTree* tree, Node* node, std::stack <Node*>&& prevStack);

        iterator begin ()
        {
            Node* res = root_;
            std::stack<Node*> prevStack {};

            if (res != nullptr)
                for (; res->left_ != nullptr; res = res->left_)
                    prevStack.push(res);

            return get_iterator (this, res, std::move(prevStack));
        }

        iterator end ()
        {
            std::stack<Node*> prevStack {};
            return get_iterator (this, nullptr, std::move(prevStack));
        }

        iterator lower_bound (const T& key)
        {
            Node* cur  = root_;
            Node* prev = nullptr;
            bool prevMoveIsLeft = false;
            std::stack<Node*> prevStack{};

            while (cur)
            {
                if (key < cur->key_)
                {
                    prev = cur;
                    prevStack.push(cur);
                    cur = cur->left_;

                    prevMoveIsLeft = true;
                }
                else if (key > cur->key_)
                {
                    prev = cur;
                    cur = cur->right_;

                    prevMoveIsLeft = false;
                }
                else
                {
                    return get_iterator (this, cur, std::move(prevStack));
                }
            }

            if (!prevStack.empty() && prevMoveIsLeft) prevStack.pop();

            iterator res = get_iterator (this, prev, std::move(prevStack));

            return (prev->key_ > key) ? res : ++res;
        }

        iterator upper_bound (const T& key)
        {
            iterator res = lower_bound (key);

            if (res && *res == key) ++res;

            return res;
        }

        class iterator
        {
        private:
            SearchTree* tree_ = nullptr;
            Node* node_ = nullptr;
            std::stack<Node*> prevStack_ {};

            iterator (const SearchTree* tree, Node* node, std::stack <Node*>&& prevStack) :
                     tree_(const_cast<SearchTree*>(tree)),
                     node_(node)
            {
                std::swap(prevStack, prevStack_);
                Node* cur = node;

                if (cur) cur = cur->right_;

                while (cur)
                {
                    prevStack_.push(cur);
                    cur = cur->left_;
                }
            }

            friend iterator get_iterator (const SearchTree* tree, Node* node, std::stack <Node*>&& prevStack)
            {
                return iterator {tree, node, std::move(prevStack)};
            }

        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using pointer = Node*;
            using reference = T&;

            iterator() : node_ (nullptr), tree_ (nullptr) {}

            explicit operator bool () { return node_; }

            const T operator* () const
            {
                if (node_) { return node_->key_; }

                throw std::runtime_error("null pointer dereference");
                return 0;
            }

            const T* operator-> () const { return node_; }

            iterator operator++ ()
            {
                if (!prevStack_.empty())
                {
                    node_ = prevStack_.top();
                    prevStack_.pop();

                    for (Node* cur = node_->right_; cur != nullptr; cur = cur->left_)
                    {
                        prevStack_.push(cur);
                    }
                }
                else
                {
                    node_ = nullptr;
                }

                return *this;
            }

            iterator operator++ (int)
            {
                iterator res = *this;
                (*this)++;
                return res;
            }

            friend bool operator== (const iterator& lhs, const iterator& rhs)
            {
                return lhs.node_ == rhs.node_;
            }

            friend bool operator!= (const iterator& lhs, const iterator& rhs)
            {
                return !(lhs == rhs);
            }
        };

// -----
        void tree_dump (const std::string& filename) const
        {
            std::ofstream file(filename);
            file << "digraph G {" << std::endl << "node [shape = record];" << std::endl;
            if (root_)
            {
                file << root_->indx_ << "[shape = doubleoctagon, style = filled, fillcolor = cornflowerblue "
                                        "label = \"" << root_->key_ << "\"]" << std::endl;
                node_dump (file, root_);
            }
            file << "}";
            file.close();

            std::string command = "dot -T png " + filename + " -o ./pic/tree.png";
            std::system (command.c_str());
        }

        void node_dump (std::ofstream& file, Node* node) const
        {
            if (!node) { return; }
            if (node->left_)
            {
                file << node->left_->indx_ << "[shape = doubleoctagon, style = filled, fillcolor = cornflowerblue "
                                              "label = \"" << node->left_->key_ << "\"]" << std::endl;
                file << node->indx_ << " -> " << node->left_->indx_ << std::endl;
                node_dump (file, node->left_);
            }
            if (node->right_)
            {
                file << node->right_->indx_ << "[shape = doubleoctagon, style = filled, fillcolor = cornflowerblue "
                                               "label = \"" << node->right_->key_ << "\"]" << std::endl;
                file << node->indx_ << " -> " << node->right_->indx_ << std::endl;
                node_dump (file, node->right_);
            }
        }
    };
}
