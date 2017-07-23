//
// Created by Sabre Sever on 2017/7/23.
//

#ifndef MINISTL_SET_H
#define MINISTL_SET_H

#include "allocator.h"
#include "rb_tree.h"

namespace mini{

    template<typename T>
    struct identity {
        const T &operator()(const T &x) { return x; }
    };

    template <class Key,class Compare = std::less<Key>,class Allocator =allocator<Key>>
    class set{

    public:
        typedef Key key_type;
        typedef Key value_type;
        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;
        typedef Compare key_compare;
        typedef Compare value_compare;
        typedef Allocator allocator_type;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        typedef value_type* pointer;
        typedef const value_type* const_pointer;
        typedef rb_tree<key_type,value_type,identity<value_type>,key_compare,allocator_type> tree_type;
        typedef tree_type::const_iterator iterator;
        typedef tree_type::const_iterator const_iterator;

        //iterator

    public:

        set():tree(Compare()){}

        explicit set(const Compare& comp=Compare(),const Allocator& alloc=Allocator()):tree(comp){}

        template <class InputIterator>
        set(InputIterator first,InputIterator last,const Compare& comp=Compare(),const Allocator &alloc=Allocator()):tree(comp){
            tree.insert_unique(first,last);
        }

        set(const set& other):tree()

        ~set(){}

    private:

        tree_type tree;


    };


}



#endif //MINISTL_SET_H
