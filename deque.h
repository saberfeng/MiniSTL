//
// Created by Sabre Sever on 2017/7/27.
//

#ifndef MINISTL_DEQUE_H
#define MINISTL_DEQUE_H

#include "allocator.h"
#include "iterator.h"

namespace mini {

    /*
    std::size_t deque_buffer_size(std::size_t n, std::size_t size) {
        return n != 0 ? n : (size < 512 ? std::size_t(512 / size) : std::size_t(1));
    }
     */

    std::size_t deque_buffer_size(std::size_t n, std::size_t size) {
        return n != 0 ? n : (size < 512 ? std::size_t(512 / size) : std::size_t(1));
    }

    template<class Value, class Reference, class Pointer>
    class deque_iterator : public iterator_base<random_access_iterator_tag, Value, ptrdiff_t, Pointer, Reference> {
    public:

        typedef Value value_type;
        typedef std::size_t size_type;
        typedef deque_iterator<Value, Value &, Value *> iterator;
        typedef deque_iterator<Value, Reference, Pointer> self;
        typedef typename iterator_base<random_access_iterator_tag, Value, ptrdiff_t, Pointer, Reference>::pointer base_ptr;
        typedef typename iterator_base<random_access_iterator_tag, Value, ptrdiff_t, Pointer, Reference>::reference base_ref;
        typedef typename iterator_base<random_access_iterator_tag, Value, ptrdiff_t, Pointer, Reference>::difference_type base_diff;
        typedef base_ptr *map_pointer;

        template<class Val,class Alloc> friend
        class deque;

        deque_iterator() : begin(nullptr), end(nullptr), cur(nullptr), node(nullptr) {}

        deque_iterator(base_ptr begin_ptr,
                       base_ptr end_ptr,
                       base_ptr cur_ptr,
                       map_pointer node_ptr) :
                begin(begin_ptr), end(end_ptr), cur(cur_ptr), node(node_ptr) {}

        //allows conversion from iterator to const_iterator
        deque_iterator(const iterator &other) :
                begin(other.begin), end(other.end), cur(other.cur), node(other.node) {}

        //reference is a dependent name
        base_ref operator*() { return *cur; }

        base_ptr operator->() { return cur; }

        void set_node(base_ptr *new_node) {
            node = new_node;
            begin = *node;
            end = begin + deque_buffer_size(0, sizeof(value_type));
        }

        self &operator++() {
            ++cur;
            if (cur == end) {
                set_node(node + 1);
                cur = begin;
            }
            return *this;
        }

        self operator++(int) {
            self tmp = *this;
            ++*this;
            return tmp;
        }

        self &operator--() {
            if (cur == begin) {
                set_node(node - 1);
                cur = end;
            }
            --cur;
            return *this;
        }

        self operator--(int) {
            self tmp = *this;
            --*this;
            return tmp;
        }

        self &operator+=(base_diff n) {
            size_type buffer_size = deque_buffer_size(0, sizeof(value_type));
            base_diff node_steps = 0;
            base_diff node_offset = 0;
            if (n > 0) {
                node_steps = (n + (cur - begin)) / buffer_size;
                node_offset = (n + (cur - begin)) % buffer_size;

            } else if (n < 0) {
                node_steps = (n - (end - cur) + 1) / buffer_size;
                node_offset = buffer_size + (n - (end - cur)) % buffer_size;
            }
            set_node(node + node_steps);
            cur = begin + node_offset;
            return *this;
        }

        self &operator-=(base_diff n) {
            return *this += -n;
        }

        self operator+(base_diff n) {
            self tmp = *this;
            return tmp += n;
        }

        self operator-(base_diff n) {
            self tmp = *this;
            return tmp -= n;
        }

        bool operator==(const self &other) { return cur == other.cur; }

        bool operator!=(const self &other) { return !(*this == other); }

        bool operator<(const self &other) {
            if (node == other.node)
                return cur < other.cur;
            else
                return node < other.node;
        }

        bool operator>(const self &other) { return other < *this; }

        bool operator<=(const self &other) { return !(other < *this); }

        bool operator>=(const self &other) { return !(*this < other); }

    private:
        base_ptr begin;
        base_ptr end;
        base_ptr cur;
        map_pointer node;

    };

    template<class Value, class Allocator = allocator <Value>>
    class deque {

    public:
        typedef Value value_type;
        typedef Allocator allocator_type;
        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;
        typedef value_type &reference;
        typedef const value_type &const_reference;
        typedef value_type *pointer;
        typedef const value_type *const_pointer;
        typedef deque_iterator<Value, Value &, Value *> iterator;
        typedef deque_iterator<Value, const Value &, const Value *> const_iterator;

        explicit deque() { init(); }

        explicit deque(size_type count, const_reference value = value_type());

        template<class InputIterator>
        deque(InputIterator first, InputIterator last);

        deque(const deque &other);

        deque(std::initializer_list<value_type> init);

        reference front() { return *start; }

        const_reference front() const { return *start; }

        reference back() { return *finish; }

        const_reference back() const { return *finish; }

        iterator begin() { return start; }

        const_iterator cbegin() const { return start; }

        iterator end() { return finish; }

        const_iterator cend() const { return finish; }

        void push_back(const_reference value);

        void push_front(const_reference value);


        iterator insert(iterator pos, const_reference value);

        iterator insert(const_iterator pos, const_reference value);

        void insert(iterator pos, size_type count, const_reference value);

        template<class InputIterator>
        void insert(iterator pos, InputIterator first, InputIterator last);

        template<class InputIterator>
        iterator insert(const_iterator pos, InputIterator first, InputIterator last);

        iterator insert(const_iterator pos, std::initializer_list<value_type> ilist);

        iterator erase(iterator pos);

        iterator erase(const_iterator pos);

        iterator erase(iterator first, iterator last);

        iterator erase(const_iterator first, const_iterator last);


    private:

        typedef pointer *map_pointer;

        map_pointer map;
        size_type map_size;

        iterator start;
        iterator finish;

        Allocator value_allocator;
        allocator <pointer> map_allocator;


        void init() {
            map = map_allocator.allocate(1);
            map_size = 8;
            map_pointer node = map + map_size / 2;
            size_type buffer_size = deque_buffer_size(0, sizeof(value_type));
            *node = value_allocator.allocate(buffer_size);

            start = iterator(*node, *node + buffer_size, *node + buffer_size / 2, node);
            finish = iterator(*node, *node + buffer_size, *node + buffer_size / 2, node);
        }

        void reallocate_map(bool front);

        void clear();

    };

    template<class Value, class Allocator>
    void deque<Value, Allocator>::push_back(const_reference value) {
        if (finish.cur < finish.end - 1) {
            construct(finish.cur, value);
            ++finish.cur;
        } else {
            if (finish.node == map + map_size - 1)
                reallocate_map(false);
            *(finish.node+1)=value_allocator.allocate(deque_buffer_size(0, sizeof(value_type)));
            construct(finish.cur,value);
            ++finish;
        }
    }

    template<class Value, class Allocator>
    void deque<Value, Allocator>::reallocate_map(bool front) {
        size_type nodes_size = finish.node - start.node + 1;
        size_type old_map_size = map_size;
        size_type free_space = map_size - nodes_size;
        map_pointer new_start_node = nullptr;
        if (free_space >= 2) {
            new_start_node = map + free_space / 2;
            if (front)
                std::copy_backward(start.node, finish.node + 1, new_start_node + nodes_size);
            else
                std::copy(start.node, finish.node, new_start_node);
        } else {
            map_size *= 2;
            map_pointer new_map = map_allocator.allocate(map_size);
            free_space = map_size - nodes_size;
            new_start_node = new_map + free_space / 2;
            std::copy(start.node, finish.node+1, new_start_node);
            map_allocator.deallocate(map, old_map_size);
            map = new_map;
        }
        start.node = new_start_node;
        finish.node = new_start_node + nodes_size - 1;
    }

    template<class Value, class Allocator>
    typename deque<Value, Allocator>::iterator
    deque<Value, Allocator>::insert(deque::iterator pos, const_reference value) {

    }

    template <class Value,class Allocator>
    void deque<Value,Allocator>::clear(){
        size_type buffer_size=deque_buffer_size(0, sizeof(value_type));
        for(map_pointer node=start.node+1;node!=finish.node+1;++node){
            destroy(*node,*node+buffer_size);
            value_allocator.deallocate(*node,buffer_size);
        }
        destroy(*(start.node),*(start.node)+buffer_size);
        start.cur=start.begin+buffer_size/2;
        finish=start;
    }


}

#endif //MINISTL_DEQUE_H
