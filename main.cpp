#include <iostream>
#include <memory>
#include <string>
#include <functional>
#include <vector>
#include <cstdlib>
#include "allocator.h"
#include "rb_tree.h"
#include "set.h"
#include "map.h"
#include "util.h"
#include "deque.h"

using std::cout;
using std::endl;

template<typename T>
struct identity {
    const T &operator()(const T &x) { return x; }
};

void TestAllocator() {

    mini::allocator<int> int_alloc;
    int *int_ptr = int_alloc.allocate(1);
    int_alloc.construct(int_ptr, 100);
    cout << *int_ptr << endl;
    int_alloc.destroy(int_ptr);
    int_alloc.deallocate(int_ptr, 1);

    mini::allocator<std::string> string_alloc;
    std::string *string_ptr = string_alloc.allocate(2);
    string_alloc.construct(string_ptr, "first");
    string_alloc.construct(string_ptr + 1, "second");
    cout << *string_ptr << endl << *(string_ptr + 1) << endl;
    string_alloc.destroy(string_ptr);
    string_alloc.destroy(string_ptr + 1);
    string_alloc.deallocate(string_ptr, 2);
}

void TestRBTree() {

    mini::rb_tree<int, int, identity<int>, std::less<int>> tree1;
    cout << "before insert size: " << tree1.size() << endl;

    std::vector<int> input = {10, 7, 8, 15, 5, 6, 11, 11, 11, 11, 13, 12, 8, 20, 14, 16};

    for (auto &i:input)
        tree1.insert_unique(i);

    mini::rb_tree<int, int, identity<int>, std::less<int>> tree(tree1);

    cout << endl << "after insert size: " << tree.size() << endl;
    cout << "Traverse:" << endl;
    tree.DebugMidorderTraverse();
    tree.DebugPreorderTraverse();

    typename mini::rb_tree<int, int, identity<int>, std::less<int>>::node_ptr node;


    mini::rb_tree_iterator<int, int &, int *> iter = tree.begin();
    cout << "iterator iterates:" << endl;
    for (int i = 0; i < 20; ++i) {
        cout << *iter << " ";
        ++iter;
    }
    cout << endl;

    cout << "reversely iterates:" << endl;
    for (int i = 0; i < 24; ++i) {
        --iter;
        cout << *iter << " ";
    }
    cout << endl;

    std::vector<int> vec = {10, 6, 16, 14, 12, 11, 13, 5, 7, 8, 15, 20};

    /*
    for (auto &i : vec) {
        node = tree.find(i);
        tree.erase(node);
        cout << "erase " << i << ": " << endl;
        tree.DebugMidorderTraverse();
    }
    */



}

template<class T>
void PrintSet(const mini::set<T> &s) {
    typename mini::set<T>::iterator iter = s.cbegin();
    while (iter != s.cend())
        cout << *iter++ << " ";
    cout << endl;
}

void TestSet() {

    typedef mini::set<int> int_set;
    typedef typename int_set::iterator iterator;

    int_set s;
    s.insert({10, 7, 8, 15, 5, 6, 11, 11, 11, 11, 13, 12, 8, 20, 14, 16});
    PrintSet(s);

    iterator iter = s.find(12);
    s.erase(iter);
    PrintSet(s);

    iterator begin = s.find(5);
    iterator end = s.find(13);
    s.erase(begin, end);
    PrintSet(s);

    cout << s.count(16) << endl;

    int_set s2;
    s2.insert({13, 14});
    cout << "s2 < s:" << ((s2 < s) ? "true" : "false") << endl
         << "s2 == s:" << ((s2 == s) ? "true" : "false") << endl;


}

void TestPrintTree() {
    typedef mini::rb_tree<int, int, identity<int>, std::less<int>> tree;
    typedef typename tree::iterator iterator;
    tree t;

    std::vector<int> input = {10, 7, 8, 15, 5, 6, 11, 11, 11, 11, 13, 12, 8, 20, 14, 16};

    for (auto &i:input)
        t.insert_unique(i);

    cout << "before erase:" << endl;
    t.PrintTree();

    iterator first = t.find(5);
    iterator last = t.find(13);
    t.Debug_erase(first, last);

    //
}

template<class Key,class Mapped>
void PrintMap(const mini::map<Key,Mapped> &m) {
    typename mini::map<Key,Mapped>::iterator iter = m.cbegin();
    while (iter != m.cend())
        cout << (*(iter++)).second << " ";
    cout << endl;
}

void TestMap() {
    typedef mini::map<std::string, int> map_type;
    typedef typename map_type::iterator iterator;

    map_type m1;
    m1.insert(mini::make_pair(std::string("one"), 1));
    m1.insert(mini::make_pair(std::string("two"), 2));
    m1.insert(mini::make_pair(std::string("three"), 3));
    m1.insert(mini::make_pair(std::string("four"), 4));

    PrintMap(m1);

    cout<<m1[std::string("two")]<<endl;
    cout<<m1[std::string("five")]<<endl;

    cout<<m1.erase(std::string("one"))<<endl;
    PrintMap(m1);

}

int main() {

    //TestAllocator();
    //TestRBTree();
    //TestSet();
    //TestPrintTree();
    //TestMap();

    return 0;
}