//
// Created by Sabre Sever on 2017/7/17.
//

#ifndef MINISTL_RB_TREE_H
#define MINISTL_RB_TREE_H


#include <iostream>
#include "allocator.h"
#include "construct.h"
#include "iterator.h"
#include "util.h"
#include "algorithm.h"
#include <memory>
#include <utility>
#include <iomanip>

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

    template<class Value, class Reference, class Pointer>
    class rb_tree_iterator : public iterator_base<bidirectional_iterator_tag, Value, ptrdiff_t, Pointer, Reference> {
    public:
        typedef rb_tree_node<Value> *node_ptr;
        typedef rb_tree_iterator<Value, Value &, Value *> iterator;
        typedef rb_tree_iterator<Value, Reference, Pointer> self;

        template<class Key, class Val, class KeyOfValue, class Compare, class Alloc> friend
        class rb_tree;

        rb_tree_iterator() {}

        rb_tree_iterator(node_ptr ptr) { node = ptr; }

        //when Reference is const Value& ,Pointer is const Value*
        //this constructor allows copy from rb_tree_iterator<Value,Value&,Value*>
        //for set and map,this constructor allows const_iterator initialized from non_const iterator
        //otherwise is not allowed
        rb_tree_iterator(const iterator &i) { node = i.node; }

        Reference operator*() const { return node->value; }

        Pointer operator->() const { return &(operator*()); }

        self &operator++() {
            if (is_nil(node))
                return *this;
            if (!is_nil(node->right)) {
                node = node->right;
                while (!is_nil(node->left))
                    node = node->left;
            } else {
                node_ptr parent = node->parent;
                while (!is_nil(parent) && node == parent->right) {
                    node = parent;
                    parent = parent->parent;
                }
                node = parent; // if node is at the last position,point node to nil
            }
            return *this;
        }

        self operator++(int) {
            self tmp = *this;
            operator++();
            return tmp;
        }

        self &operator--() {
            if (is_nil(node)) { //if node is at end(),point it to the last position
                node = node->parent;
                while (!is_nil(node->right))
                    node = node->right;
            } else {
                if (!is_nil(node->left)) {
                    node = node->left;
                    while (!is_nil(node->right))
                        node = node->right;
                } else { // node is not nil and has no left child
                    node_ptr parent = node->parent;
                    node_ptr old_node = node;
                    while (!is_nil(parent) && node == parent->left) {
                        node = parent;
                        parent = parent->parent;
                    }
                    if (is_nil(parent)) // if node is at the first position, stay there
                        node = old_node;
                    else
                        node = parent;
                }
            }
            return *this;
        }

        self operator--(int) {
            self tmp = *this;
            operator--();
            return tmp;
        }

        node_ptr node;

    private:
        bool is_nil(node_ptr ptr) { return ptr->left == ptr; }

    };

    template<typename Value, typename Reference, typename Pointer>
    inline bool operator==(const rb_tree_iterator<Value, Reference, Pointer> &x,
                           const rb_tree_iterator<Value, Reference, Pointer> &y) {
        return x.node == y.node;
    }

    template<typename Value, typename Reference, typename Pointer>
    inline bool operator!=(const rb_tree_iterator<Value, Reference, Pointer> &x,
                           const rb_tree_iterator<Value, Reference, Pointer> &y) {
        return x.node != y.node;
    }


    template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc=mini::allocator<rb_tree_node<Value>>>
    class rb_tree {

    public:
        typedef rb_tree_node<Value> *node_ptr;
        typedef const rb_tree_node<Value> *const_node_ptr;
        typedef std::size_t size_type;
        typedef Value &reference;
        typedef const Value &const_reference;
        typedef Value *pointer;
        typedef const Value *const_pointer;
        typedef rb_tree_iterator<Value, reference, pointer> iterator;
        typedef rb_tree_iterator<Value, const_reference, const_pointer> const_iterator;

        friend class rb_tree_iterator<Value, reference, pointer>;

    private:

        Alloc rb_tree_node_allocator;
        //allocator<rb_tree_node<Value>> rb_tree_node_allocator;
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
        node_ptr create_node() {
            node_ptr ptr = get_node();
            construct(&(ptr->value), Value());
            return ptr;
        }

        node_ptr create_node(const_reference v) {
            node_ptr ptr = get_node();
            construct(&(ptr->value), v);
            return ptr;
        }

        node_ptr clone_node(node_ptr node) {
            node_ptr clone = create_node(node->value);
            clone->color = node->color;
            clone->left = nullptr;
            clone->right = nullptr;
            clone->parent = nullptr;
            return clone;
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

        node_ptr left_most() const { return rb_minimum(root()); }

        node_ptr right_most() const { return rb_maximum(root()); }

        static node_ptr &left(node_ptr node) { return node->left; }

        static node_ptr &right(node_ptr node) { return node->right; }

        static node_ptr &parent(node_ptr node) { return node->parent; }

        static reference value(node_ptr node) { return node->value; }

        static const Key &key(node_ptr node) { return KeyOfValue()(value(node)); }

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

        node_ptr rb_minimum(node_ptr root) const {
            while (root->left != nil)
                root = root->left;
            return root;
        }

        node_ptr rb_maximum(node_ptr root) const {
            while (root->right != nil)
                root = root->right;
            return root;
        }

        node_ptr create_tmp_nil() {
            node_ptr tmp_nil = create_node();
            tmp_nil->color = rb_tree_black;
            tmp_nil->left = tmp_nil;
            tmp_nil->right = tmp_nil;
            return tmp_nil;
        }

        void copy_tree(const rb_tree &other);

        node_ptr copy_tree_core(node_ptr other_node, node_ptr other_nil);

    public:
        rb_tree(Compare comp = Compare()) : node_count(0), key_compare(comp) { init(); }

        rb_tree(const rb_tree &other) {
            init();
            if (other.root() == other.nil) // then "other" is an empty tree
                return;
            node_count = other.node_count;
            copy_tree(other);
        }

        rb_tree& operator=(const rb_tree &other){
            clear();
            if(other.root()==other.nil)
                return *this;
            node_count=other.node_count;
            copy_tree(other);
            return *this;
        }

        ~rb_tree() {
            clear();
            destroy_node(nil);
        }

        void clear() {
            erase(begin(), end());
        }

        size_type size() const { return node_count; }

        bool empty() { return node_count == 0; }

        void swap(rb_tree &other) {
            if (root() == nil && other.root() == other.nil)
                return;
            else if (root() == nil) {
                nil->parent = other.root();
                other.root()->parent = nil;
                other.nil->parent = other.nil;
            } else if (other.root() == other.nil) {
                other.nil->parent = root();
                root()->parent = other.nil;
                nil->parent = nil;
            } else {
                node_ptr tmp = other.root();
                other.nil->parent = root();
                root()->parent = other.nil;
                nil->parent = tmp;
                tmp->parent = nil;
            }
        }

        pair<iterator, bool> insert_unique(const_reference v);

        void insert_unique(const_iterator i, const_iterator j);

        iterator insert_equal(const_reference v);

        void insert_equal(const_iterator i, const_iterator j);

        iterator insert(const_iterator hint, const_reference v);

        iterator _insert(node_ptr insert_position, node_ptr parent, const_reference v);

        void erase(node_ptr x);

        void erase(const_iterator i);

        void erase(const_iterator i, const_iterator j);

        size_type erase(const Key &key);

        iterator find(const Key &key) const;

        iterator begin() { return iterator(left_most()); }

        iterator end() { return iterator(nil); }

        const_iterator cbegin() const { return const_iterator(left_most()); }

        const_iterator cend() const { return const_iterator(nil); }

        iterator lower_bound(const Key &k) const;

        iterator upper_bound(const Key &k) const;

        pair<iterator, iterator> equal_range(const Key &k) const;

        size_type count(const Key &k) const {
            iterator start = lower_bound(k);
            iterator end = upper_bound(k);
            return distance(start, end);
        }

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

        void PrintTree() {
            PrintSubTree(root(), 0);
        }

        void PrintSubTree(node_ptr node, int indent) {
            if (node != nil) {
                if (node->right != nil)
                    PrintSubTree(node->right, indent + 4);
                if (indent)
                    std::cout << std::setw(indent) << ' ';
                if (node->right != nil)
                    std::cout << " /\n" << std::setw(indent) << ' ';

                if (node->color == rb_tree_black)
                    std::cout << "B";
                else
                    std::cout << "R";
                std::cout << node->value << "\n";

                if (node->left != nil) {
                    std::cout << std::setw(indent) << ' ' << " \\\n";
                    PrintSubTree(node->left, indent + 4);
                }
            }
        }

        void Debug_erase(iterator first, iterator last) {
            while (first != last) {
                erase(first);
                std::cout << "erase " << *first << "-----------------------------" << std::endl;
                ++first;
                PrintTree();
            }
        }
        //--------------------------------------------------------------------

    };

    template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
    find(const Key &k) const {
        node_ptr y = nil; //last node which is not less than key
        node_ptr x = root();

        while (x != nil) {
            if (!key_compare(key(x), k)) {
                y = x;
                x = x->left;
            } else
                x = x->right;
        }

        if (key_compare(k, key(y)))
            y = nil;
        return y;
    }

    template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
    copy_tree(const rb_tree &other) {
        node_ptr my_top = copy_tree_core(other.root(), other.nil);
        my_top->parent = nil;
        nil->parent = my_top;
    }

    template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::node_ptr rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
    copy_tree_core(node_ptr other_node, node_ptr other_nil) {
        if (other_node == other_nil)
            return nil;
        node_ptr copy = clone_node(other_node);
        node_ptr left = copy_tree_core(other_node->left, other_nil);
        node_ptr right = copy_tree_core(other_node->right, other_nil);
        copy->left = left;
        copy->right = right;
        if (left != nil)
            left->parent = copy;
        if (right != nil)
            right->parent = copy;
        return copy;
    }

    template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
    lower_bound(const Key &k) const {
        node_ptr bound = nil;
        node_ptr node = root();

        // if node with value k exists,bound point to first k node
        // if node with value k doesn't exists,bound point to first node bigger than k
        // Above all , bound point to first node which is not less than k
        while (node != nil) {
            if (!key_compare(key(node), k)) {
                bound = node;
                node = node->left;
            } else
                node = node->right;
        }

        return iterator(bound);
    }

    template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
    upper_bound(const Key &k) const {
        node_ptr bound = nil;
        node_ptr node = root();

        while (node != nil) {
            if (key_compare(k, key(node))) {
                bound = node;
                node = node->left;
            } else
                node = node->right;
        }
        return iterator(bound);
    }

    template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator, typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator>
    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
    equal_range(const Key &k) const {
        return pair<iterator, iterator>(lower_bound(k), upper_bound(k));
    }


    template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
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

    template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
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


    template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
    insert_equal(const_reference v) { //for map, Value is a pair
        /*
        node_ptr new_node = create_node(v);
        color(new_node) = rb_tree_red;

        node_ptr y = nil;
        node_ptr x = root();
        while (x != nil) {
            y = x;
            //if (v < x->value)
            if (key_compare(KeyOfValue()(v), key(x)))
                x = x->left;
            else
                x = x->right;
        }
        new_node->parent = y;
        if (y == nil)
            nil->parent = new_node;
        else if (key_compare(KeyOfValue()(v), key(y)))
            y->left = new_node;
        else
            y->right = new_node;
        new_node->left = nil;
        new_node->right = nil;

        rb_insert_fixup(new_node);
        */
        node_ptr y = nil;
        node_ptr x = root();

        while (x != nil) {
            y = x;
            x = key_compare(KeyOfValue()(v), key(x)) ? x->left : x->right;
        }
        return _insert(x, y, v);
    }

    template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator, bool>
    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
    insert_unique(const_reference v) {
        node_ptr y = nil;
        node_ptr x = root();
        bool comp = true;

        while (x != nil) {
            y = x;
            comp = key_compare(KeyOfValue()(v), key(x));
            x = comp ? x->left : x->right;
        }
        iterator j = iterator(y);
        if (comp) {
            if (j == begin()) {
                return pair<iterator, bool>(_insert(x, y, v), true);
            } else {
                --j;
            }
        }
        if (key_compare(key(j.node), KeyOfValue()(v)))
            return pair<iterator, bool>(_insert(x, y, v), true);
        return pair<iterator, bool>(j, false);

    }

    template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
    _insert(node_ptr insert_position, node_ptr parent, const_reference v) {
        node_ptr new_node = create_node(v);
        new_node->color = rb_tree_red;
        new_node->left = nil;
        new_node->right = nil;

        new_node->parent = parent;
        if (parent == nil)
            nil->parent = new_node;
        else if (key_compare(KeyOfValue()(v), key(parent)))
            parent->left = new_node;
        else
            parent->right = new_node;

        rb_insert_fixup(new_node);

        ++node_count;

        return iterator(new_node);
    };

    template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
    insert_unique(const_iterator i, const_iterator j) {
        while (i != j)
            insert_unique(*i++);
    }

    template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
    insert_equal(const_iterator i, const_iterator j) {
        for (; i != j; ++i)
            insert_equal(*i);
    }

    template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
    insert(const_iterator hint, const_reference v) {
        return _insert(hint.node->left, hint.node, v);
    }

    template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
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


    template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
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

    template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
    rb_delete_fixup(node_ptr x) {
        node_ptr old_x = x;
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

                    x = root();
                }
            }
        }
        if (old_x != nil && old_x->left == old_x) {//if old_x is a tmp_nil
            rb_transplant(old_x, nil);
            //put_node(old_x);
            destroy_node(old_x);
        }

        x->color = rb_tree_black;
    }

    template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
    erase(node_ptr z) {
        if (z == nil)
            return;

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
        --node_count;
        destroy_node(z);
    }

    template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
    erase(const_iterator i) {
        erase(i.node);
    }

    template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
    erase(const_iterator i, const_iterator j) {
        while (i != j)
            erase(i++.node);
    }

    template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    typename rb_tree<Key,Value,KeyOfValue,Compare,Alloc>::size_type rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
    erase(const Key &key){
        size_type count = 0;
        iterator iter = find(key);
        while (iter != end()) {
            ++count;
            erase(iter);
            iter = find(key);
        }
        return count;
    }

            template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    bool operator==(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc> &tree1,
                    const rb_tree<Key, Value, KeyOfValue, Compare, Alloc> &tree2) {
        return (tree1.size() == tree2.size() && equal(tree1.cbegin(), tree1.cend(), tree1.cbegin()));
    }

    template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    bool operator<(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc> &tree1,
                   const rb_tree<Key, Value, KeyOfValue, Compare, Alloc> &tree2) {
        return lexicographical_compare(tree1.cbegin(), tree1.cend(), tree2.cbegin(), tree2.cend());
    }

}


#endif //MINISTL_RB_TREE_H
