#ifndef STACK_H
#define STACK_H

// Array-based stack that doubles its capacity when full.
// Used for the undo feature and for iterative DFS.

template <typename T>
class Stack {
private:
    T* items;
    int capacity;
    int count;

    void grow() {
        int newCapacity = capacity * 2;
        T* bigger = new T[newCapacity];
        for (int i = 0; i < count; i++) bigger[i] = items[i];
        delete[] items;
        items = bigger;
        capacity = newCapacity;
    }

public:
    Stack(int initialCapacity = 16)
        : items(new T[initialCapacity]), capacity(initialCapacity), count(0) {}
    ~Stack() { delete[] items; }

    Stack(const Stack&) = delete;
    Stack& operator=(const Stack&) = delete;

    void push(const T& value) {
        if (count == capacity) grow();
        items[count++] = value;
    }

    // pop requires a non-empty stack (check isEmpty() first).
    T pop() { return items[--count]; }

    bool isEmpty() const { return count == 0; }
    int size() const { return count; }
};

#endif
