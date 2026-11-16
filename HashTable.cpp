#include "HashTable.h"

HashTable::HashTable(int initialCapacity)
    : slots(new Parcel*[initialCapacity]()), capacity(initialCapacity), count(0) {}

HashTable::~HashTable() {
    delete[] slots;
}

int HashTable::hash(int key) const {
    return key % capacity;
}

// Puts a parcel in the first free slot, starting at its hash position.
void HashTable::place(Parcel* parcel) {
    int index = hash(parcel->trackingId);
    while (slots[index] != nullptr) {
        index = (index + 1) % capacity;
    }
    slots[index] = parcel;
}

// Grows the table and re-inserts every parcel (positions depend on the capacity).
void HashTable::rehash() {
    Parcel** oldSlots = slots;
    int oldCapacity = capacity;

    capacity = capacity * 2 + 1;
    slots = new Parcel*[capacity]();

    for (int i = 0; i < oldCapacity; i++) {
        if (oldSlots[i] != nullptr) place(oldSlots[i]);
    }
    delete[] oldSlots;
}

void HashTable::insert(Parcel* parcel) {
    if ((count + 1) * 10 > capacity * 6) rehash();   // keep the load factor under 0.6
    place(parcel);
    count++;
}

Parcel* HashTable::search(int trackingId, int& probes) const {
    probes = 0;
    int index = hash(trackingId);
    while (slots[index] != nullptr) {
        probes++;
        if (slots[index]->trackingId == trackingId) return slots[index];
        index = (index + 1) % capacity;
    }
    return nullptr;
}

Parcel* HashTable::search(int trackingId) const {
    int probes;
    return search(trackingId, probes);
}
