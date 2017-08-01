//
// Created by Sabre Sever on 2017/7/27.
//

#ifndef MINISTL_DEQUE_H
#define MINISTL_DEQUE_H

#include "allocator.h"
#include "iterator.h"

namespace mini {

    std::size_t deque_buffer_size(std::size_t n, std::size_t size) {
        return n != 0 ? n : (size < 512 ? std::size_t(512 / size) : std::size_t(1));
    }

    template<class Value, class Reference, class Pointer>
    class deque_iterator : public iterator_base<random_access_iterator_tag, Value, ptrdiff_t, Pointer, Reference> {
    public:

        typedef Value value_type;
        typedef std::size_t size_type;
        typedef deque_iterator<Value, Value &, Value *> iterator;
        typedef deque_iterator<Value, const Value &, const Value *> const_iterator;
        typedef deque_iterator<Value, Reference, Pointer> self;
        typedef typename iterator_base<random_access_iterator_tag, Value, ptrdiff_t, Pointer, Reference>::pointer base_ptr;
        typedef typename iterator_base<random_access_iterator_tag, Value, ptrdiff_t, Pointer, Reference>::reference base_ref;
        typedef typename iterator_base<random_access_iterator_tag, Value, ptrdiff_t, Pointer, Reference>::difference_type base_diff;
        typedef base_ptr *map_pointer;

        template<class Val, class Alloc> friend
        class deque;

        template<class V, class R, class P> friend
        class deque_iterator;

        deque_iterator() : begin(nullptr), end(nullptr), cur(nullptr), node(nullptr) {}

        deque_iterator(base_ptr begin_ptr,
                       base_ptr end_ptr,
                       base_ptr cur_ptr,
                       map_pointer node_ptr) :
                begin(begin_ptr), end(end_ptr), cur(cur_ptr), node(node_ptr) {}

        /* libcpp constructor
         * template <class _Pp, class _Rp, class _MP>
            _LIBCPP_INLINE_VISIBILITY
            __deque_iterator(const __deque_iterator<value_type, _Pp, _Rp, _MP, difference_type, __block_size>& __it,
                typename enable_if<is_convertible<_Pp, pointer>::value>::type* = 0) _NOEXCEPT
        : __m_iter_(__it.__m_iter_), __ptr_(__it.__ptr_) {}
         *
         */


        //allows conversion from iterator to const_iterator
        deque_iterator(const iterator &other) :
                begin(other.begin), end(other.end), cur(other.cur), node(const_cast<map_pointer>(other.node)) {}

        //TODO:const problem
        deque_iterator& operator=(const iterator &other){
            begin=other.begin;
            end=other.end;
            cur=other.cur;
            node=other.node;
            return *this;
        }

        /*
        //allows conversion from const_iterator to iterator
        deque_iterator(const const_iterator &other) :
                begin(other.begin), end(other.end), cur(other.cur), node(other.node) {}
        */

        //reference is a dependent name
        base_ref operator*() { return *cur; }

        base_ptr operator->() { return cur; }

        void set_node(base_ptr *new_node) {
            node = new_node;
            begin = *node;
            end = begin + deque_buffer_size(0, sizeof(value_type));
        }

        base_diff operator-(const self &other) const {
            return base_diff(deque_buffer_size(0, sizeof(value_type))) * (node - other.node - 1) +
                   (other.end - other.cur) + (cur - begin);
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
            if (n >= 0) {
                node_steps = (n + (cur - begin)) / buffer_size;
                node_offset = (n + (cur - begin)) % buffer_size;

            } else {
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

        self operator+(base_diff n) const {
            self tmp = *this;
            return tmp += n;
        }

        self operator-(base_diff n) const {
            self tmp = *this;
            return tmp -= n;
        }

        bool operator==(const self &other) const { return cur == other.cur; }

        bool operator!=(const self &other) const { return !(*this == other); }

        bool operator<(const self &other) const {
            if (node == other.node)
                return cur < other.cur;
            else
                return node < other.node;
        }

        bool operator>(const self &other) const { return other < *this; }

        bool operator<=(const self &other) const { return !(other < *this); }

        bool operator>=(const self &other) const { return !(*this < other); }

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

        explicit deque(size_type count, const_reference value = value_type()) {
            init();
            for (size_type i = 0; i < count; ++i)
                push_back(value);
        }

        template<class InputIterator>
        deque(InputIterator first, InputIterator last) {
            init();
            while (first != last)
                push_back(*first++);
        }

        deque(const deque &other) {
            init();
            for (const_iterator iter = other.cbegin(); iter != other.cend(); ++iter)
                push_back(*iter);
        }

        deque(std::initializer_list<value_type> ilist) {
            init();
            for (auto &item:ilist)
                push_back(item);
        }

        ~deque() { clear(); }

        reference front() { return *start; }

        const_reference front() const { return *start; }

        reference back() { return *finish; }

        const_reference back() const { return *finish; }

        iterator begin() const { return start; }

        const_iterator cbegin() const { return start; }

        iterator end() const { return finish; }

        const_iterator cend() const { return finish; }

        void assign(size_type count, const_reference value) {
            clear();
            for (int i = 0; i < count; ++i)
                push_back(value);
        }

        template<class InputIterator>
        void assign(InputIterator first, InputIterator last) {
            clear();
            while (first != last)
                push_back(*first++);
        }

        void assign(std::initializer_list<value_type> ilist) {
            clear();
            for (auto &item:ilist)
                push_back(item);
        }

        void push_back(const_reference value);

        void push_front(const_reference value);

        iterator insert(iterator pos, const_reference value);

        iterator insert(const_iterator pos, const_reference value);

        void insert(iterator pos, size_type count, const_reference value);

        void insert(const_iterator pos, size_type count, const_reference value);

        template<class InputIterator>
        void insert(iterator pos, InputIterator first, InputIterator last);

        template<class InputIterator>
        iterator insert(const_iterator pos, InputIterator first, InputIterator last);

        iterator insert(const_iterator pos, std::initializer_list<value_type> ilist);

        iterator erase(iterator pos);

        iterator erase(const_iterator pos);

        iterator erase(iterator first, iterator last);

        iterator erase(const_iterator first, const_iterator last);

        reference operator[](size_type pos) { return *(start + pos); }

        const_reference operator[](size_type pos) const { return *(start + pos); }

        reference at(size_type pos) { return *(start + pos); }

        const_reference at(size_type pos) const { return *(start + pos); }


        void pop_back();

        void pop_front();

        void clear();

        size_type size() { return size_type(finish - start); }

        bool empty() { return size() == 0; }

    private:

        typedef pointer *map_pointer;

        map_pointer map;
        size_type map_size;

        iterator start;
        iterator finish;

        Allocator value_allocator;
        allocator <pointer> map_allocator;

        //std::allocator<value_type> value_allocator;
        //std::allocator<pointer> map_allocator;


        void init() {
            map = map_allocator.allocate(8);
            map_size = 8;
            map_pointer node = map + map_size / 2;
            size_type buffer_size = deque_buffer_size(0, sizeof(value_type));
            *node = value_allocator.allocate(buffer_size);

            start = iterator(*node, *node + buffer_size, *node + buffer_size / 2, node);
            finish = iterator(*node, *node + buffer_size, *node + buffer_size / 2, node);
        }

        void reallocate_map(bool front);

    };

    template<class Value, class Allocator>
    void deque<Value, Allocator>::push_back(const_reference value) {
        if (finish.cur < finish.end - 1) {
            construct(finish.cur, value);
            ++finish.cur;
        } else {
            if (finish.node == map + map_size - 1)
                reallocate_map(false);
            *(finish.node + 1) = value_allocator.allocate(deque_buffer_size(0, sizeof(value_type)));
            construct(finish.cur, value);
            ++finish;
        }
    }

    template<class Value, class Allocator>
    void deque<Value, Allocator>::push_front(const_reference value) {
        if (start.cur > start.begin) {
            --start.cur;
            construct(start.cur, value);
        } else {
            if (start.node == map)
                reallocate_map(true);
            *(start.node - 1) = value_allocator.allocate(deque_buffer_size(0, sizeof(value_type)));
            --start;
            construct(start.cur, value);
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
                std::copy(start.node, finish.node + 1, new_start_node);
        } else {
            map_size *= 2;
            map_pointer new_map = map_allocator.allocate(map_size);
            free_space = map_size - nodes_size;
            new_start_node = new_map + free_space / 2;
            std::copy(start.node, finish.node + 1, new_start_node);
            map_allocator.deallocate(map, old_map_size);
            map = new_map;
        }
        start.node = new_start_node;
        finish.node = new_start_node + nodes_size - 1;
    }


    template<class Value, class Allocator>
    typename deque<Value, Allocator>::iterator
    deque<Value, Allocator>::insert(deque::iterator pos, const_reference value) {
        const_iterator position = pos;
        return insert(position,value);
        /*
        if (pos < start || pos > finish)
            return finish;
        if (pos == finish) {
            push_back(value);
            iterator tmp = finish;
            --tmp;
            return tmp;
        } else if (pos == start) {
            push_front(value);
            return start;
        } else {
            if (pos - start < finish - pos) {
                push_front(*start);
                iterator copy_start = start;
                copy_start += 2;
                std::copy(copy_start, pos, start + 1);
                --pos;
            } else {
                iterator end1 = finish;
                iterator end2 = end1;
                push_back(*(--end1));
                std::copy_backward(pos, end1, end2);
            }
            construct(pos.cur, value);
            return pos;
        }
         */
    }


    template<class Value, class Allocator>
    typename deque<Value, Allocator>::iterator
    deque<Value, Allocator>::insert(const_iterator position, const_reference value) {
        iterator pos = start+(position-start);
        if (pos < start || pos > finish)
            return finish;
        if (pos == finish) {
            push_back(value);
            iterator tmp = finish;
            --tmp;
            return tmp;
        } else if (pos == start) {
            push_front(value);
            return start;
        } else {
            if (pos - start < finish - pos) {
                push_front(*start);
                iterator copy_start = start;
                copy_start += 2;
                std::copy(copy_start, pos, start + 1);
                --pos;
            } else {
                iterator end1 = finish;
                iterator end2 = end1;
                push_back(*(--end1));
                std::copy_backward(pos, end1, end2);
            }
            destroy(pos.cur);
            construct(pos.cur, value);
            return pos;
        }
    }

    /*
    template<class Value, class Allocator>
    void deque<Value, Allocator>::insert(iterator pos, size_type count, const_reference value) {
        for (size_type i = 0; i < count; ++i)
            pos = insert(pos, value);
    }
     */

    template<class Value, class Allocator>
    void deque<Value, Allocator>::insert(const_iterator position, size_type count, const_reference value) {
        iterator pos=start+(position-start);
        for (size_type i = 0; i < count; ++i)
            pos = insert(pos, value);
    }

    template<class Value, class Allocator>
    template<class InputIterator>
    void deque<Value, Allocator>::insert(iterator pos, InputIterator first, InputIterator last) {
        while (first != last) {
            pos = insert(pos, *first);
            ++first;
        }
    }

    template<class Value, class Allocator>
    template<class InputIterator>
    typename deque<Value, Allocator>::iterator
    deque<Value, Allocator>::insert(const_iterator position, InputIterator first, InputIterator last) {
        iterator pos = start+(position-start);
        while (first != last) {
            pos = insert(pos, *first);
            ++first;
        }
    }

    template<class Value, class Allocator>
    typename deque<Value, Allocator>::iterator
    deque<Value, Allocator>::insert(const_iterator position, std::initializer_list<value_type> ilist) {
        iterator pos = start+(position-start);
        for (auto &item:ilist)
            pos = insert(pos, item);
        return pos;
    }

    template<class Value, class Allocator>
    typename deque<Value, Allocator>::iterator
    deque<Value, Allocator>::erase(const_iterator position) {
        iterator pos = start+(position-start);
        if (pos == start) {
            pop_front();
            return start;
        } else if (pos == finish - 1) {
            pop_back();
            return finish;
        } else {
            if (pos - start < finish - pos) {
                std::copy_backward(start, pos, pos + 1);
                pop_front();
                ++pos;
            } else {
                std::copy(pos + 1, finish, pos);
                pop_back();
            }
            return pos;
        }
    }

    template<class Value, class Allocator>
    typename deque<Value, Allocator>::iterator
    deque<Value, Allocator>::erase(const_iterator first_iter, const_iterator last_iter) {
        iterator first = start+(first_iter-start);
        iterator last = start+(last_iter-start);
        if (first == start && last == finish) {
            clear();
            return finish;
        } else {
            size_type buffer_size = deque_buffer_size(0, sizeof(value_type));
            size_type erase_size = size_type(last - first);
            iterator result;
            if (first - start < finish - last) {
                std::copy_backward(start, first, last);
                iterator new_start = start + erase_size;
                destroy(start, new_start);
                for (map_pointer node = start.node; node < new_start.node; ++node)
                    value_allocator.deallocate(*node, buffer_size);
                start = new_start;
                result = last;
            } else {
                std::copy(last, finish, first);
                iterator new_finish = finish - erase_size;
                destroy(new_finish, finish);
                for (map_pointer node = new_finish.node; node < finish.node; ++node)
                    value_allocator.deallocate(*node, buffer_size);
                finish = new_finish;
                result = first;
            }
            return result;
        }

    }


    template<class Value, class Allocator>
    void deque<Value, Allocator>::pop_front() {
        if (start != finish) {
            destroy(start.cur);
            iterator old_start = start;
            ++start;
            if (old_start.cur == old_start.end - 1)
                map_allocator.deallocate(old_start.begin, deque_buffer_size(0, sizeof(value_type)));
        }
    }

    template<class Value, class Allocator>
    void deque<Value, Allocator>::pop_back() {
        if (start != finish) {
            iterator old_finish = finish;
            --finish;
            destroy(finish.cur);
            if (old_finish.cur == old_finish.begin)
                map_allocator.deallocate(old_finish.begin, deque_buffer_size(0, sizeof(value_type)));
        }
    }

    template<class Value, class Allocator>
    void deque<Value, Allocator>::clear() {
        size_type buffer_size = deque_buffer_size(0, sizeof(value_type));
        for (map_pointer node = start.node + 1; node != finish.node + 1; ++node) {
            destroy(*node, *node + buffer_size);
            value_allocator.deallocate(*node, buffer_size);
        }
        destroy(*(start.node), *(start.node) + buffer_size);
        start.cur = start.begin + buffer_size / 2;
        finish = start;
    }


}

#endif //MINISTL_DEQUE_H
