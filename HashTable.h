#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "Models.h"

// Hash table with linear probing (open addressing), keyed by tracking ID.
// The table stores pointers to parcels; it does not own them.
class HashTable {
private:
    Parcel** slots;   // nullptr means the slot is empty
    int capacity;
    int count;

    int hash(int key) const;
    void place(Parcel* parcel);
    void rehash();

public:
    HashTable(int initialCapacity = 11);
    ~HashTable();

    HashTable(const HashTable&) = delete;
    HashTable& operator=(const HashTable&) = delete;

    void insert(Parcel* parcel);                         // tracking IDs must be unique
    Parcel* search(int trackingId, int& probes) const;   // probes = slots examined
    Parcel* search(int trackingId) const;

    int size() const { return count; }
    int getCapacity() const { return capacity; }
    double loadFactor() const { return static_cast<double>(count) / capacity; }
};

#endif
