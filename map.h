//
// Created by Sabre Sever on 2017/7/25.
//

#ifndef MINISTL_MAP_H
#define MINISTL_MAP_H

#include "rb_tree.h"
#include "util.h"
#include "function.h"

namespace mini {

    template<typename Key, class Mapped, class Compare = std::less<Key>, class Allocator=allocator<pair<const Key, Mapped>>>
    class map {

    public:
        typedef Key key_type;
        typedef Mapped mapped_type;
        typedef pair<const Key, Mapped> value_type;
        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;
        typedef Compare key_compare;
        typedef Allocator allocator_type;
        typedef value_type &reference;
        typedef const value_type &const_reference;
        typedef value_type *pointer;
        typedef const value_type *const_pointer;

        class value_compare {
            friend class map<Key, Mapped, Compare, Allocator>;

        private:
            Compare comp;

            value_compare(Compare c) : comp(c) {}

        public:
            bool operator()(const value_type &x, const value_type &y) const {
                return comp(x.first, y.first);
            }
        };

    private:
        typedef rb_tree<key_type, value_type, Select1st<value_type>, key_compare, Allocator> tree_type;
        tree_type tree;

    public:
        typedef typename tree_type::const_iterator iterator;
        typedef typename tree_type::const_iterator const_iterator;

        map() : tree(Compare()) {}

        template<class InputIterator>
        map(InputIterator first, InputIterator last,
            const Compare &comp = Compare(), const Allocator &alloc = Allocator()):tree(comp) {
            tree.insert_unique(first, last);
        }

        map(const map &other) : tree(other.tree) {}

        map(std::initializer_list<value_type> init) {
            for (auto &item:init)
                tree.insert_unique(item);
        }

        map &operator=(const map &other) {
            tree = other.tree;
            return *this;
        }

        Mapped &operator[](const Key &key);

        iterator begin() { return tree.begin(); }

        const_iterator cbegin() const { return tree.cbegin(); }

        iterator end() { return tree.end(); }

        const_iterator cend() const { return tree.cend(); }

        bool empty() const { return size() == 0; }

        size_type size() const { return tree.size(); }

        void clear() { tree.clear(); }

        pair<iterator, bool> insert(const value_type &value) { return tree.insert_unique(value); }

        iterator insert(iterator hint, const value_type &value) { return tree.insert(hint, value); }

        template<class InputIterator>
        void insert(InputIterator first, InputIterator last) { tree.insert_unique(first, last); }

        void insert(std::initializer_list<value_type> ilist) {
            for (auto &item:ilist)
                tree.insert_unique(item);
        }

        void erase(iterator pos) { tree.erase(pos); }

        void erase(iterator first, iterator last) { tree.erase(first, last); }

        size_type erase(const key_type &key) { return tree.erase(key); }

        void swap(map &other) { tree.swap(other.tree); }

        size_type count(const key_type &key) { return tree.count(key); }

        const_iterator find(const Key &key) const {return tree.find(key);}

        const_iterator lower_bound(const Key &key) const

    };

}

#endif //MINISTL_MAP_H
