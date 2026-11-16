#ifndef MODELS_H
#define MODELS_H

#include <string>

enum Status { REGISTERED, PENDING, DISPATCHED, DELIVERED };

inline std::string statusName(Status status) {
    switch (status) {
        case REGISTERED: return "Registered";
        case PENDING:    return "Pending";
        case DISPATCHED: return "Dispatched";
        case DELIVERED:  return "Delivered";
    }
    return "Unknown";
}

struct Customer {
    int id;
    std::string name;
    std::string phone;
    std::string address;

    Customer(int id, const std::string& name, const std::string& phone, const std::string& address)
        : id(id), name(name), phone(phone), address(address) {}
};

struct Parcel {
    int trackingId;
    int customerId;
    std::string source;
    std::string destination;
    double weight;
    int priority;    // 1 (lowest) to 5 (highest)
    Status status;
    bool urgent;     // true if the parcel was requested through the urgent heap
    int arrivalNo;   // order in which the delivery request arrived

    Parcel(int trackingId, int customerId, const std::string& source,
           const std::string& destination, double weight, int priority)
        : trackingId(trackingId), customerId(customerId), source(source),
          destination(destination), weight(weight), priority(priority),
          status(REGISTERED), urgent(false), arrivalNo(0) {}
};

// An entry of the urgent-delivery heap.
struct Request {
    int trackingId;
    int priority;
    int arrivalNo;

    // "a > b" means "a is dispatched before b":
    // higher priority first, and for equal priority the earlier arrival first.
    bool operator>(const Request& other) const {
        if (priority != other.priority) return priority > other.priority;
        return arrivalNo < other.arrivalNo;
    }
};

// One line of the delivery history.
struct HistoryEntry {
    int trackingId;
    Status status;   // DISPATCHED or DELIVERED
};

// Remembers what a parcel looked like before a status change, so undo can restore it.
struct UndoRecord {
    int trackingId;
    Status previousStatus;
};

#endif
