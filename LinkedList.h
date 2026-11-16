#ifndef LINKEDLIST_H
#define LINKEDLIST_H

// Doubly linked list with head and tail pointers.
// Used for customers, parcels, delivery history, queue storage and graph adjacency lists.

template <typename T>
struct ListNode {
    T data;
    ListNode* next;
    ListNode* prev;

    ListNode(const T& value) : data(value), next(nullptr), prev(nullptr) {}
};

template <typename T>
class LinkedList {
private:
    ListNode<T>* head;
    ListNode<T>* tail;
    int count;

public:
    LinkedList() : head(nullptr), tail(nullptr), count(0) {}
    ~LinkedList() { clear(); }

    LinkedList(const LinkedList&) = delete;
    LinkedList& operator=(const LinkedList&) = delete;

    void pushBack(const T& value) {
        ListNode<T>* node = new ListNode<T>(value);
        if (tail == nullptr) {
            head = tail = node;
        } else {
            node->prev = tail;
            tail->next = node;
            tail = node;
        }
        count++;
    }

    void pushFront(const T& value) {
        ListNode<T>* node = new ListNode<T>(value);
        if (head == nullptr) {
            head = tail = node;
        } else {
            node->next = head;
            head->prev = node;
            head = node;
        }
        count++;
    }

    // popFront and popBack require a non-empty list (check isEmpty() first).
    T popFront() {
        ListNode<T>* node = head;
        T value = node->data;
        head = head->next;
        if (head == nullptr) tail = nullptr;
        else head->prev = nullptr;
        delete node;
        count--;
        return value;
    }

    T popBack() {
        ListNode<T>* node = tail;
        T value = node->data;
        tail = tail->prev;
        if (tail == nullptr) head = nullptr;
        else tail->next = nullptr;
        delete node;
        count--;
        return value;
    }

    bool isEmpty() const { return count == 0; }
    int size() const { return count; }
    ListNode<T>* getHead() const { return head; }
    ListNode<T>* getTail() const { return tail; }

    void clear() {
        while (head != nullptr) {
            ListNode<T>* next = head->next;
            delete head;
            head = next;
        }
        tail = nullptr;
        count = 0;
    }
};

#endif
