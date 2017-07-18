#include <iostream>
#include <memory>
#include <string>
#include <functional>
#include "allocator.h"
#include "rb_tree.h"

using std::cout;
using std::endl;

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

    mini::rb_tree<int, int, std::less<int>> tree;
    cout << "before insert size: " << tree.size() << endl;

    tree.insert_unique(10);
    tree.insert_unique(7);
    tree.insert_unique(8);
    tree.insert_unique(15);
    tree.insert_unique(5);
    tree.insert_unique(6);
    tree.insert_unique(11);
    tree.insert_unique(13);
    tree.insert_unique(12);
    tree.insert_unique(20);
    tree.insert_unique(14);
    tree.insert_unique(16);

    cout << "after insert size: " << tree.size() << endl;
    cout << "Traverse:" << endl;
    tree.DebugMidorderTraverse();
    tree.DebugPreorderTraverse();

}


int main() {

    //TestAllocator();
    TestRBTree();

    return 0;
}