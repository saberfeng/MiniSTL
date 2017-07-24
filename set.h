//
// Created by Sabre Sever on 2017/7/23.
//

#ifndef MINISTL_SET_H
#define MINISTL_SET_H

#include "allocator.h"
#include "rb_tree.h"

namespace mini {

    template<typename T>
    struct identity {
        const T &operator()(const T &x) { return x; }
    };

    template<class Key, class Compare = std::less<Key>, class Allocator =allocator <rb_tree_node<Key>>>
    class set {

    public:
        typedef Key key_type;
        typedef Key value_type;
        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;
        typedef Compare key_compare;
        typedef Compare value_compare;
        typedef Allocator allocator_type;
        typedef value_type &reference;
        typedef const value_type &const_reference;
        typedef value_type *pointer;
        typedef const value_type *const_pointer;
        typedef rb_tree <key_type, value_type, identity<value_type>, key_compare, allocator_type> tree_type;
        typedef typename tree_type::const_iterator iterator;
        typedef typename tree_type::const_iterator const_iterator;

        //iterator

    public:

        explicit set(const Compare &comp = Compare(), const Allocator &alloc = Allocator()) : tree(comp) {}

        template<class InputIterator>
        set(InputIterator first, InputIterator last, const Compare &comp = Compare(),
            const Allocator &alloc = Allocator()):tree(comp) {
            tree.insert_unique(first, last);
        }

        set(const set &other) : tree(other.tree) {}

        set(std::initializer_list<value_type> init, const Compare &comp = Compare()) : tree(comp) {
            for (auto &item :init)
                tree.insert_unique(item);
        }

        iterator begin() { return tree.begin(); }

        const_iterator cbegin() { return tree.begin(); }

        iterator end() { return tree.end(); }

        const_iterator cend() { return tree.end(); }

        void clear() { tree.clear(); }

        std::pair<iterator, bool> insert(const_reference value) { return tree.insert_unique(value); };

        iterator insert(iterator hint, const_reference value) { return tree.insert(hint, value); }

        template<class InputIterator>
        void insert(InputIterator first, InputIterator last) { tree.insert(first, last); }

        void insert(std::initializer_list<value_type> ilist) {
            for (auto &item :ilist)
                tree.insert_unique(item);
        }

        //TODO: emplace

        void erase(iterator position) { tree.erase(position); }

        void erase(iterator first, iterator last) { tree.erase(first, last); }

        size_type erase(const_reference key) {
            size_type count = 0;
            iterator iter = tree.find(key);
            while (iter != tree.end()) {
                ++count;
                tree.erase(iter);
                iter = tree.find(key);
            }
            return count;
        }

        void swap(set &other) {
            tree.swap(other.tree);
        }

        size_type count(const_reference k) const { return tree.count(k); }

        const_iterator find(const_reference k) const { return tree.find(k); }

        std::pair<iterator, iterator> equal_range(const_reference k) const { return tree.equal_range(k); }

        key_compare key_comp() const { return key_compare(); }

        value_compare value_comp() const { return value_compare(); }

    private:

        tree_type tree;


    };


    //TODO: equal lexicographical_compare
    template<class Key, class Compare, class Allocator>
    bool operator==(set<Key, Compare, Allocator> set1, set<Key, Compare, Allocator> set2) {

    }

    template<class Key, class Compare, class Allocator>
    bool operator!=(set<Key, Compare, Allocator> set1, set<Key, Compare, Allocator> set2) {
        return !(set1 == set2);
    }

    template<class Key, class Compare, class Allocator>
    bool operator<(set<Key, Compare, Allocator> set1, set<Key, Compare, Allocator> set2) {

    }

    template<class Key, class Compare, class Allocator>
    bool operator<=(set<Key, Compare, Allocator> set1, set<Key, Compare, Allocator> set2) {
        return !(set2 < set1);
    }

    template<class Key, class Compare, class Allocator>
    bool operator>(set<Key, Compare, Allocator> set1, set<Key, Compare, Allocator> set2) {
        return !(set1 <= set2);
    }

    template<class Key, class Compare, class Allocator>
    bool operator>=(set<Key, Compare, Allocator> set1, set<Key, Compare, Allocator> set2) {
        return !(set1 < set2);
    }

}


#endif //MINISTL_SET_H
