# Cyclic cache-friendly doubly-linked list

## Description & Features
This project includes the description of the list itself and a bunch of useful methods for working with it. These methods include:
- Constructor and destructor
- Insertion and deletion of head and tail
- Insertion before/after a node and deletion of a node with his *real* position (array index)
- Insertion before/after a node and deletion of a node with his *logical* number
- Getting node's *real* position with his *logical* number
- In-place O(n) list sort
- Verification
- Graphic dump

***IMPORTANT THING ABOUT NODES' LOGICAL NUMBERS:*** head node has *logical* number 1, tail node's *logical* number equals list's size, fictional node has *logical* number 0 and *real* position (array index) 0 as well.

Sorting function can be used to match nodes' logical numbers with their positions in the array. If this happens, list automatically switches to the quick mode - all the functions taking nodes' *logical* numbers as arguments start working with algorithmic complexity O(1) instead of O(n). This continues until the accordance between array indexes and logical numbers isn't broken.

Unlike traditional doubly-linked list's implementations, this one is using indexes instead of pointers. This fact makes it possible to store all the nodes in a single array, making it much more memory-local and easy to cache. Free nodes are stored at the same array and joined into an another single-linked index-based list, which makes the process of "finding a place" for a new node just as fast as if it was about allocating memory with *malloc()*. If the array becomes overflowed, resizing functions are applied.

Besides, this list is cyclic with a fictional node (that has list's head as a next node and tail as a previous one), which makes most of the operations with it a bit quicker due to some regular in-code validations being unnecessary.

Finally, the list has an autoverification system ("manual" verification can pe performed by using the relevant function). To turn it off, comment out the `#define AUTO_VERIFICATION_ON` line and recompile your project. Without this before every function's execution the whole list's state will be fully diagnosted - if any flaws are detected, the "verification failed" message will appear in the console. This can be quite useful for debugging, but this makes most of the functions **much** slower.

## Latest version
The latest version of the cyclic list can be found here: <https://github.com/quaiion/cyclic-list>.

## Dependencies
As a part of a some *main* project, cyclic list requires only the **dot** utility from [Graphviz](https://graphviz.org) software package. Without it you will not be able to use graphic dumps.

## Documentation
In-code documentation (compatible with Doxygen) will be added soon.

## License
This is an open-source educational project. You can freely use, edit and distribute any versions of this code in personal and commercial purposes.

## Contacts
#### Korney Ivanishin, author of the project
email: <korney059@gmail.com>,
GitHub: [quaiion](https://github.com/quaiion)
