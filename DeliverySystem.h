#ifndef DELIVERYSYSTEM_H
#define DELIVERYSYSTEM_H

#include <string>
#include "AVL.h"
#include "BST.h"
#include "Graph.h"
#include "HashTable.h"
#include "Heap.h"
#include "LinkedList.h"
#include "Models.h"
#include "Queue.h"
#include "Stack.h"

// Ties all the data structures together. Each public method is one menu option.
class DeliverySystem {
private:
    LinkedList<Customer*> customers;     // owns the customers
    LinkedList<Parcel*> parcels;         // owns the parcels (the other structures only point to them)
    HashTable parcelTable;               // tracking ID -> parcel
    BST bstIndex;                        // ordered parcel index
    AVL avlIndex;                        // balanced parcel index
    Queue<int> normalQueue;              // tracking IDs of normal requests, in arrival order
    Heap<Request> urgentHeap;            // urgent requests, highest priority first
    LinkedList<HistoryEntry> history;    // dispatched and delivered events
    Stack<UndoRecord> undoStack;         // status changes that can be undone
    Graph network;                       // city road network
    int nextArrivalNo;

    Customer* findCustomer(int id) const;
    bool addCustomer(int id, const std::string& name, const std::string& phone,
                     const std::string& address, std::string& error);
    bool addParcel(int trackingId, int customerId, const std::string& source,
                   const std::string& destination, double weight, int priority,
                   std::string& error);
    void addRequest(bool urgent);
    void changeStatus(Parcel* parcel, Status newStatus);
    void traverseNetwork(bool useBfs);
    void promptAddRoad();
    int promptLocation(const std::string& prompt) const;

    bool loadNetwork(const std::string& path);
    bool loadCustomers(const std::string& path);
    bool loadParcels(const std::string& path);

public:
    DeliverySystem();
    ~DeliverySystem();

    DeliverySystem(const DeliverySystem&) = delete;
    DeliverySystem& operator=(const DeliverySystem&) = delete;

    void loadData(const std::string& directory);

    void registerCustomer();
    void registerParcel();
    void searchParcel();
    void addNormalRequest();
    void addUrgentRequest();
    void processNextDelivery();
    void markDelivered();
    void undoLastOperation();
    void displayPending() const;
    void displayRecords() const;
    void manageNetwork();
    void runBfs();
    void runDfs();
    void findShortestRoute();
    void generateMinimumNetwork() const;
    void compareTrees() const;
    void displayStatistics() const;
};

#endif
