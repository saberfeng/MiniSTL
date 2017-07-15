#include <iostream>
#include <memory>
#include "allocator.h"

using std::cout;
using std::endl;

int main() {

    mini::allocator<int> alloc;
    int *int_ptr=alloc.allocate(1);
    alloc.construct(int_ptr,100);
    cout<<*int_ptr<<endl;

    return 0;
}