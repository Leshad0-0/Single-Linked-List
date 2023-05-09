# SingleLinkedList

## Introduction
SingleLinkedList is a templated class that provides an implementation of a singly linked list data structure. It is designed to be simple and efficient, and provides all the necessary functionality you would expect from a linked list, including insertion, deletion, and traversal.



## Usage
### Creating a SingleLinkedList
You can create an empty SingleLinkedList object like this:
```cpp
SingleLinkedList<int> list;
```
You can also initialize a SingleLinkedList with values using an initializer list:
```cpp
SingleLinkedList<int> list = {1, 2, 3};
```


### Iterating over a SingleLinkedList
You can iterate over a SingleLinkedList using range-based for loops:
```cpp
SingleLinkedList<int> list = {1, 2, 3};
for (int value : list) {
    std::cout << value << std::endl;
}
```
You can also use iterators to iterate over a SingleLinkedList:
```cpp
SingleLinkedList<int> list = {1, 2, 3};
for (auto it = list.begin(); it != list.end(); ++it) {
    std::cout << *it << std::endl;
}
```


### Modifying a SingleLinkedList
You can add elements to the beginning of a SingleLinkedList using the push_front method:
```cpp
list.push_front(1);
```
You can insert elements after a specific position using the insert_after method:
```cpp
list.insert_after(list.begin(), 2);
```
You can remove the first element of a SingleLinkedList using the pop_front method:
```cpp
list.pop_front();
```
You can remove an element after a specific position using the erase_after method:
```cpp
list.erase_after(list.begin());
```


### Getting information about a SingleLinkedList
You can get the size of a SingleLinkedList using the size method:
```cpp
std::cout << list.size();l;
```
You can check if a SingleLinkedList is empty using the empty method:
```cpp
std::cout << list.empty();
```


### Clearing a SingleLinkedList
You can remove all elements from a SingleLinkedList using the clear method:
```cpp
list.clear();
```


## Testing
The SingleLinkedList implementation includes a set of tests in the tests directory. The tests cover basic functionality of the linked list, including adding and removing nodes, traversing the list, finding nodes by value or index, and manipulating the list structure.