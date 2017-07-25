//
// Created by Sabre Sever on 2017/7/25.
//

#ifndef MINISTL_MAP_H
#define MINISTL_MAP_H

#include "rb_tree.h"
#include "util.h"
#include "function.h"

namespace mini{

    template <typename Key,class Mapped, class Compare = std::less<Key>,class Allocator=allocator<pair<const Key,Value>>>
    class map{

    public:
        typedef Key key_type;
        typedef Mapped mapped_type;
        typedef pair<const Key,Mapped> value_type;
        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;
        typedef Compare key_compare;
        typedef Allocator allocator_type;
        typedef value_type &reference;
        typedef const value_type &const_reference;
        typedef value_type *pointer;
        typedef const value_type *const_pointer;

        class value_compare{
            friend class map<Key,Mapped,Compare,Allocator>;
        private:
            Compare comp;
            value_compare(Compare c):comp(c){}
        public:
            bool operator()(const value_type& x, const value_type &y) const {
                return comp(x.first,y.first);
            }
        };

    private:
        typedef rb_tree<key_type,value_type,Select1st<value_type>,key_compare,Allocator> tree_type;
        tree_type tree;

    public:
        typedef typename tree_type::const_iterator iterator;
        typedef typename tree_type::const_iterator const_iterator;
        

    };

}

#endif //MINISTL_MAP_H
