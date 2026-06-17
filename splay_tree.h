//
// Created by cartercpp on 6/17/26.
//

#ifndef SPLAYTREE_SPLAY_TREE_H
#define SPLAYTREE_SPLAY_TREE_H

#include <initializer_list>
#include <queue>
#include <vector>
#include <ranges>
#include <algorithm>
#include <concepts>
#include <cstddef>

template <std::totally_ordered ValueType>
class splay_tree
{
private:

    struct Node
    {
        ValueType m_value;
        Node* m_parent = nullptr;
        Node* m_left = nullptr;
        Node* m_right = nullptr;
    };

public:

    // CONSTRUCTORS & STUFF

    splay_tree() noexcept
        : m_root{nullptr}, m_size(0)
    {}

    splay_tree(std::initializer_list<ValueType> values)
        : splay_tree{}
    {
        for (const ValueType& value : values)
            insert(value);
    }

    splay_tree(const splay_tree& other)
        : splay_tree{}
    {
        const auto values = other.level_order();
        for (auto&& value : values | std::views::reverse)
            insert(value);
    }

    splay_tree& operator=(const splay_tree& other)
    {
        if (this != &other)
        {
            clear();

            const auto values = other.level_order();
            for (auto&& value : values | std::views::reverse)
                insert(value);
        }

        return *this;
    }

    splay_tree(splay_tree&& other) noexcept
    : m_root{other.m_root}, m_size{other.m_size}
    {
        other.m_root = nullptr;
        other.m_size = 0;
    }

    splay_tree& operator=(splay_tree&& other) noexcept
    {
        if (this != &other)
        {
            clear();

            m_root = other.m_root;
            m_size = other.m_size;

            other.m_root = nullptr;
            other.m_size = 0;
        }

        return *this;
    }

    ~splay_tree()
    {
        clear();
    }

    // METHODS

    std::vector<ValueType> level_order() const
    {
        std::vector<ValueType> output;
        output.reserve(m_size);

        std::queue<const Node*> nodes;
        nodes.emplace(m_root);

        while (!nodes.empty())
        {
            const Node* node = nodes.front();
            nodes.pop();

            if (node)
            {
                output.emplace_back(node->m_value);
                nodes.emplace(node->m_left);
                nodes.emplace(node->m_right);
            }
        }

        return output;
    }

    void clear()
    {
        auto func = [](this const auto& self, Node* node) -> void {
            if (node)
            {
                self(node->m_left);
                self(node->m_right);
                delete node;
            }
        };

        func(m_root);
        m_root = nullptr;
        m_size = 0;
    }

    void insert(const ValueType& value)
    {
        if (m_root)
        {
            Node* node = m_root;
            while (true)
            {
                if (value < node->m_value)
                {
                    if (!node->m_left)
                    {
                        node->m_left = new Node;
                        node->m_left->m_parent = node;
                        node = node->m_left;
                        break;
                    }
                    else
                        node = node->m_left;
                }
                else if (value > node->m_value)
                {
                    if (!node->m_right)
                    {
                        node->m_right = new Node;
                        node->m_right->m_parent = node;
                        node = node->m_right;
                        break;
                    }
                    else
                        node = node->m_right;
                }
                else
                    return;
            }

            node->m_value = value;
            Splay(node);
        }
        else
        {
            m_root = new Node;
            m_root->m_value = value;
        }

        ++m_size;
    }

    bool contains(const ValueType& value)
    {
        Node* node = m_root;

        while (node)
        {
            if (value < node->m_value)
                node = node->m_left;
            else if (value > node->m_value)
                node = node->m_right;
            else
                break;
        }

        if (node && (node->m_value == value))
        {
            Splay(node);
            return true;
        }

        return false;
    }

    std::size_t size() const noexcept
    {
        return m_size;
    }

    bool empty() const noexcept
    {
        return m_size == 0;
    }

private:

    void Splay(Node* node)
    {
        if (!node)
            return;

        Node* parent = node->m_parent;
        while (parent)
        {
            Node* grandparent = parent->m_parent;

            if (!grandparent)
            {
                if (parent->m_left == node)
                    RightRotate(parent);
                else
                    LeftRotate(parent);
            }
            else if ((grandparent->m_left == parent) && (parent->m_left == node))
            {
                RightRotate(grandparent);
                RightRotate(parent);
            }
            else if ((grandparent->m_right == parent) && (parent->m_right == node))
            {
                LeftRotate(grandparent);
                LeftRotate(parent);
            }
            else if ((parent->m_left == node) && (grandparent->m_right == parent))
            {
                LeftRotate(parent);
                RightRotate(grandparent);
            }
            else
            {
                RightRotate(parent);
                LeftRotate(grandparent);
            }

            parent = node->m_parent;
        }
    }

    /*
             x              ->            y
            / \                          / \
           a   y                        x   c
              / \                      / \
             b   c                    a   b
     */
    Node* LeftRotate(Node* x) noexcept
    {
        if (!x)
            return x;

        Node* y = x->m_right;
        if (y)
        {
            if (x->m_parent)
            {
                if (x->m_parent->m_left == x)
                    x->m_parent->m_left = y;
                else
                    x->m_parent->m_right = y;
            }
            else
                m_root = y;

            y->m_parent = x->m_parent;

            x->m_right = y->m_left;
            if (x->m_right)
                x->m_right->m_parent = x;

            y->m_left = x;
            x->m_parent = y;
        }

        return y;
    }

    /*
             x              ->            y
            / \                          / \
           y   c                        a   x
          / \                              / \
         a   b                            b   c
     */
    Node* RightRotate(Node* x) noexcept
    {
        if (!x)
            return x;

        Node* y = x->m_left;
        if (y)
        {
            if (x->m_parent)
            {
                if (x->m_parent->m_left == x)
                    x->m_parent->m_left = y;
                else
                    x->m_parent->m_right = y;
            }
            else
                m_root = y;

            y->m_parent = x->m_parent;
            
            x->m_left = y->m_right;
            if (x->m_left)
                x->m_left->m_parent = x;

            y->m_right = x;
            x->m_parent = y;
        }

        return y;
    }

    Node* m_root;
    std::size_t m_size;
};

#endif //SPLAYTREE_SPLAY_TREE_H
