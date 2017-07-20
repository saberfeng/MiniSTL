//
// Created by Sabre Sever on 2017/7/17.
//

#ifndef MINISTL_RB_TREE_H
#define MINISTL_RB_TREE_H


#include <iostream>
#include "allocator.h"
#include "construct.h"
#include <memory>

namespace mini {

    typedef bool rb_tree_color_type;
    const rb_tree_color_type rb_tree_red = false;
    const rb_tree_color_type rb_tree_black = true;

    template<typename ValueType>
    class rb_tree_node {
    public:
        rb_tree_color_type color;
        rb_tree_node *parent;
        rb_tree_node *left;
        rb_tree_node *right;
        ValueType value;
    };


    template<typename Key, typename Value, typename Compare, typename Alloc=allocator <rb_tree_node<Value>>>
    class rb_tree {

    public:
        typedef rb_tree_node<Value> *node_ptr;
        typedef const rb_tree_node<Value> *const_node_ptr;
        typedef std::size_t size_type;
        typedef Value &reference;
        typedef const Value &const_reference;

    private:

        allocator <rb_tree_node<Value>> rb_tree_node_allocator;
        std::allocator<rb_tree_node<Value>> debug_tree_node_allocator;

        node_ptr get_node() {
            //return debug_tree_node_allocator.allocate(1);
            return rb_tree_node_allocator.allocate(1);
        }

        void put_node(node_ptr p) {
            //debug_tree_node_allocator.deallocate(p, 1);
            rb_tree_node_allocator.deallocate(p, 1);
        }

        //TODO: all construction may give rise to exception
        node_ptr create_node(const_reference val) {
            node_ptr ptr = get_node();
            construct(&(ptr->value), val);
            return ptr;
        }

        node_ptr clone_node(node_ptr node) {
            node_ptr clone = create_node(node->value);
            clone->color = node->color;
            clone->left = nullptr;
            clone->right = nullptr;
            clone->parent = nullptr;
        }

        void destroy_node(node_ptr node) {
            destroy(node);
            put_node(node);
        }

    private:

        size_type node_count;
        node_ptr nil;
        //node_ptr root;

        Compare key_compare;

        node_ptr &root() const { return nil->parent; }

        node_ptr &left_most() const { return nil->left; }

        node_ptr &right_most() const { return nil->right; }

        static node_ptr &left(node_ptr node) { return node->left; }

        static node_ptr &right(node_ptr node) { return node->right; }

        static node_ptr &parent(node_ptr node) { return node->parent; }

        static reference value(node_ptr node) { return node->value; }

        static rb_tree_color_type &color(node_ptr node) { return node->color; }

        void init() {
            nil = get_node();
            color(nil) = rb_tree_black;
            parent(nil) = nil;
            left(nil) = nil;
            right(nil) = nil;

            node_count = 0;
        }

        void rb_left_rotate(node_ptr x);

        void rb_right_rotate(node_ptr x);

        void rb_insert_fixup(node_ptr node);

        void rb_transplant(node_ptr old_node, node_ptr new_node);

        void rb_delete_fixup(node_ptr x);

        node_ptr rb_minimum(node_ptr root) {
            while (root->left != nil)
                root = root->left;
            return root;
        }

        node_ptr rb_maximum(node_ptr root) {
            while (root->right != nil)
                root = root->right;
            return root;
        }

        void clear() {

        }

        node_ptr create_tmp_nil() {
            node_ptr tmp_nil = create_node(0);
            tmp_nil->color = rb_tree_black;
            tmp_nil->left = tmp_nil;
            tmp_nil->right = tmp_nil;
            return tmp_nil;
        }

    public:
        rb_tree(const Compare &comp = Compare()) : node_count(0), key_compare(comp) { init(); }

        ~rb_tree() {
            clear();
            put_node(nil);
        }

        size_type size() { return node_count; }


        void insert_unique(const_reference v);

        void erase(node_ptr x);

        node_ptr find(const_reference val);

        //--------------------------for Debugging----------------------------
        void DebugMidorderTraverseCore(node_ptr node) {
            if (node == nil)
                return;
            DebugMidorderTraverseCore(node->left);
            std::cout << node->value << " ";
            DebugMidorderTraverseCore(node->right);
        }

        void DebugMidorderTraverse() {
            DebugMidorderTraverseCore(root());
            std::cout << std::endl;
        }

        void DebugPreorderTraverseCore(node_ptr node) {
            if (node == nil)
                return;
            std::cout << node->value << " ";
            DebugPreorderTraverseCore(node->left);
            DebugPreorderTraverseCore(node->right);
        }

        void DebugPreorderTraverse() {
            DebugPreorderTraverseCore(root());
            std::cout << std::endl;
        }

        rb_tree_color_type DebugGetColor(node_ptr node) { return node->color; }

        reference DebugGetValue(node_ptr node) { return node->value; }
        //--------------------------------------------------------------------

    };

    template<typename Key, typename Value, typename Compare, typename Alloc>
    typename rb_tree<Key, Value, Compare, Alloc>::node_ptr rb_tree<Key, Value, Compare, Alloc>::
    find(const_reference val) {
        node_ptr prev = root();
        node_ptr node = prev;
        while (node != nil) {
            prev = node;
            if (node->value == val)
                return node;
            else if (val < node->value)
                node = node->left;
            else
                node = node->right;
        }
        return nullptr;
    }

    template<typename Key, typename Value, typename Compare, typename Alloc>
    void rb_tree<Key, Value, Compare, Alloc>::
    rb_left_rotate(node_ptr x) {
        node_ptr y = x->right;
        x->right = y->left;
        if (x->right != nil)
            x->right->parent = x;
        y->parent = x->parent;
        if (y->parent == nil)
            nil->parent = y;
        else if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;
        y->left = x;
        x->parent = y;
    }

    template<typename Key, typename Value, typename Compare, typename Alloc>
    void rb_tree<Key, Value, Compare, Alloc>::
    rb_right_rotate(node_ptr x) {
        node_ptr y = x->left;
        x->left = y->right;
        if (x->left != nil)
            x->left->parent = x;
        y->parent = x->parent;
        if (x->parent == nil)
            nil->parent = y;
        else if (x->parent->left == x)
            x->parent->left = y;
        else
            x->parent->right = y;
        y->right = x;
        x->parent = y;
    }

    template<typename Key, typename Value, typename Compare, typename Alloc>
    void rb_tree<Key, Value, Compare, Alloc>::
    rb_insert_fixup(node_ptr node) {
        while (node->parent->color == rb_tree_red) {
            if (node->parent == node->parent->parent->left) {
                node_ptr uncle = node->parent->parent->right;
                if (uncle->color == rb_tree_red) {
                    node->parent->color = rb_tree_black;
                    uncle->color = rb_tree_black;
                    node->parent->parent->color = rb_tree_red;
                    node = node->parent->parent;
                } else {
                    if (node == node->parent->right) {
                        node = node->parent;
                        rb_left_rotate(node);
                    }
                    node->parent->color = rb_tree_black;
                    node->parent->parent->color = rb_tree_red;
                    rb_right_rotate(node->parent->parent);
                }
            } else {
                node_ptr uncle = node->parent->parent->left;
                if (uncle->color == rb_tree_red) {
                    node->parent->color = rb_tree_black;
                    uncle->color = rb_tree_black;
                    node->parent->parent->color = rb_tree_red;
                    node = node->parent->parent;
                } else {
                    if (node == node->parent->left) {
                        node = node->parent;
                        rb_right_rotate(node);
                    }
                    node->parent->color = rb_tree_black;
                    node->parent->parent->color = rb_tree_red;
                    rb_left_rotate(node->parent->parent);
                }
            }

        }
        root()->color = rb_tree_black;
    }

    template<typename Key, typename Value, typename Compare, typename Alloc>
    void rb_tree<Key, Value, Compare, Alloc>::
    rb_transplant(node_ptr old_node, node_ptr new_node) {
        if (new_node != nil)
            new_node->parent = old_node->parent;
        if (old_node->parent == nil)
            nil->parent = new_node;
        else if (old_node == old_node->parent->left)
            old_node->parent->left = new_node;
        else
            old_node->parent->right = new_node;
    }

    template<typename Key, typename Value, typename Compare, typename Alloc>
    void rb_tree<Key, Value, Compare, Alloc>::
    insert_unique(const_reference v) {
        node_ptr new_node = create_node(v);
        color(new_node) = rb_tree_red;

        node_ptr y = nil;
        node_ptr x = root();
        while (x != nil) {
            y = x;
            if (v < x->value)
                x = x->left;
            else
                x = x->right;
        }
        new_node->parent = y;
        if (y == nil)
            nil->parent = new_node;
        else if (v < y->value)
            y->left = new_node;
        else
            y->right = new_node;
        new_node->left = nil;
        new_node->right = nil;

        rb_insert_fixup(new_node);

        ++node_count;
    }

    template<typename Key, typename Value, typename Compare, typename Alloc>
    void rb_tree<Key, Value, Compare, Alloc>::
    rb_delete_fixup(node_ptr x) {
        while (x != root() && x->color == rb_tree_black) {
            if (x == x->parent->left) {
                node_ptr w = x->parent->right;
                if (w->color == rb_tree_red) {
                    w->color = rb_tree_black;
                    x->parent->color = rb_tree_red;
                    rb_left_rotate(x->parent);
                    w = x->parent->right;
                }
                if (w->left->color == rb_tree_black && w->right->color == rb_tree_black) {
                    w->color = rb_tree_red;
                    x = x->parent;
                } else {
                    if (w->right->color == rb_tree_black) {
                        w->left->color = rb_tree_black;
                        w->color = rb_tree_red;
                        rb_right_rotate(w);
                        w = x->parent->right;
                    }
                    w->color = x->parent->color;
                    x->parent->color = rb_tree_black;
                    w->right->color = rb_tree_black;
                    rb_left_rotate(x->parent);

                    if (x->left == x)
                        rb_transplant(x, nil);
                    put_node(x);
                    x = root();
                }
            } else {
                node_ptr w = x->parent->left;
                if (w->color == rb_tree_red) {
                    w->color = rb_tree_black;
                    x->parent->color = rb_tree_red;
                    rb_right_rotate(x->parent);
                    w = x->parent->left;
                }
                if (x->left->color == rb_tree_black && w->right->color == rb_tree_black) {
                    w->color = rb_tree_red;
                    x = x->parent;
                } else {
                    if (w->left->color == rb_tree_black) {
                        w->right->color = rb_tree_black;
                        w->color = rb_tree_red;
                        rb_left_rotate(w);
                        w = x->parent->left;
                    }
                    w->color = x->parent->color;
                    x->parent->color = rb_tree_black;
                    w->left->color = rb_tree_black;
                    rb_right_rotate(x->parent);

                    if (x->left == x)
                        rb_transplant(x, nil);
                    put_node(x);
                    x = root();
                }
            }
        }
        if (x != nil && x->left == x) {
            rb_transplant(x, nil);
            put_node(x);
            x = root();
        }
        x->color = rb_tree_black;
    };

    template<typename Key, typename Value, typename Compare, typename Alloc>
    void rb_tree<Key, Value, Compare, Alloc>::
    erase(node_ptr z) {
        node_ptr y = z;
        rb_tree_color_type y_original_color = y->color;
        node_ptr x = nullptr;

        if (z->left == nil && z->right != nil) {
            x = z->right;
            rb_transplant(z, x);
        } else if (z->right == nil && z->left != nil) {
            x = z->left;
            rb_transplant(z, x);
        } else if (z->left == nil && z->right == nil) {
            if (y_original_color == rb_tree_black)
                x = create_tmp_nil();
            else
                x = nil;
            rb_transplant(z, x);
        } else {
            y = rb_minimum(z->right);
            y_original_color = y->color;
            x = y->right;
            if (x == nil && y_original_color ==
                            rb_tree_black) { //only when need to fixup we create temporary nil node,otherwise tmp_nil won't be deallocated
                x = create_tmp_nil();
                x->parent = y;
                y->right = x;
            }
            if (y->parent != z) {
                rb_transplant(y, x);
                y->right = z->right;
                y->right->parent = y;
            }
            rb_transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }
        if (y_original_color == rb_tree_black)
            rb_delete_fixup(x);
        put_node(z);
    }


}


#endif //MINISTL_RB_TREE_H
