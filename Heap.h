#ifndef HEAP_H
#define HEAP_H

// Array-based max-heap.
// T must provide operator>, where "a > b" means "a comes out of the heap before b".

template <typename T>
class Heap {
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

    void swapItems(int a, int b) {
        T temp = items[a];
        items[a] = items[b];
        items[b] = temp;
    }

    void siftUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (!(items[index] > items[parent])) break;
            swapItems(index, parent);
            index = parent;
        }
    }

    void siftDown(int index) {
        while (true) {
            int left = 2 * index + 1;
            int right = left + 1;
            int best = index;
            if (left < count && items[left] > items[best]) best = left;
            if (right < count && items[right] > items[best]) best = right;
            if (best == index) break;
            swapItems(index, best);
            index = best;
        }
    }

public:
    Heap(int initialCapacity = 16)
        : items(new T[initialCapacity]), capacity(initialCapacity), count(0) {}

    // Copy constructor lets us print the heap in order without disturbing it.
    Heap(const Heap& other)
        : items(new T[other.capacity]), capacity(other.capacity), count(other.count) {
        for (int i = 0; i < count; i++) items[i] = other.items[i];
    }

    ~Heap() { delete[] items; }

    Heap& operator=(const Heap&) = delete;

    void push(const T& value) {
        if (count == capacity) grow();
        items[count++] = value;
        siftUp(count - 1);
    }

    // pop requires a non-empty heap (check isEmpty() first).
    T pop() {
        T top = items[0];
        items[0] = items[--count];
        siftDown(0);
        return top;
    }

    bool isEmpty() const { return count == 0; }
    int size() const { return count; }
};

#endif
