#ifndef QUEUE_H
#define QUEUE_H

#include "LinkedList.h"

// First-in first-out queue built on the linked list.
// Used for normal delivery requests and for BFS.

template <typename T>
class Queue {
private:
    LinkedList<T> list;

public:
    void enqueue(const T& value) { list.pushBack(value); }

    // dequeue requires a non-empty queue (check isEmpty() first).
    T dequeue() { return list.popFront(); }

    // Puts an item back at the front (used when a dispatch is undone).
    void enqueueFront(const T& value) { list.pushFront(value); }

    bool isEmpty() const { return list.isEmpty(); }
    int size() const { return list.size(); }
    ListNode<T>* getHead() const { return list.getHead(); }
};

#endif
