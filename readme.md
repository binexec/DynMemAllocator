# Dynamic Memory Allocator

This project provides a higher level, system and architectural independant implementation of malloc, calloc, free, and realloc in C. 

Before any of the allocation functions can be used, you must call the function **init_malloc** to set a start and end address for the dynamic memory heap. The memory covered by this address range will not be reserved or initialized immediately, as the heap will only grow as needed. 

### Preventing Stack Smashing
You may specify the heap end address to potentially overlap with the stack to maximize the amount of memory made available for the heap. Keep in mind that when a lot of memory is used by the user application, the stack and heap may collide into each other and cause memory corruption. In this case, consider implementing the function **check_stack_integrity** to prevent a new heap allocation from accidentally smashing into the stack. This function is called whenever the current heap allocation must be grown. The function has the following signature:

>static int check_stack_integrity(void* new_break)

The input argument **new_break** is a memory address of where the new _malloc break_ will be after the current heap expansion. The _malloc break_ is the memory address of where the current allocated heap  ends.

If the stack is deemed violated if the planned expansion is allowed, the function must **return 0**. If no problems are found, the function must **return a positive value**. 

Because there is no portable way to obtain the current stack pointer (or verify the stack's integrity), malloc will not check for stack smashing or violations in the default implementation. **If you do not wish to implement a low-level solution to prevent stack violations, you can simply make a statically allocated array of chars of desired size in your program, and use the array as the malloc heap.** In such use case, the statically allocated char array is dedicated to the heap and will never be overlapped by any other memory operations under normal circumstances. Refer to the test cases in _my_malloc_test.c_ for usage example.

### Alterantive Allocation Scheme
The default version at the root of the folder is the **dynamic heap** implementation. This is the standard version where memory grows from a lower address towards a higher address. An alternative allocation scheme is avavilable, where the second **dynamic stack** implementation is found in the folder _dyn_stack_, allocates memory from a higher starting address towards lower addresses.

Below is a diagram showing the allocation differences between the dynamic heap and dynamic stack implementation.
![alt text](https://github.com/bowen-liu/DynMemAllocator/raw/master/allocation_schemes.png)