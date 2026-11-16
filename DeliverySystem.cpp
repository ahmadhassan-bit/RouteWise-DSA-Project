#include "DeliverySystem.h"
#include "Input.h"
#include <fstream>
#include <iomanip>
#include <iostream>
using namespace std;

namespace {

const int MAX_ID = 999999999;

void printParcelRow(const Parcel& p) {
    cout << "  #" << p.trackingId << " | Customer " << p.customerId << " | "
         << p.source << " -> " << p.destination << " | " << p.weight << " kg | Priority "
         << p.priority << " | " << statusName(p.status) << "\n";
}

}  // namespace

DeliverySystem::DeliverySystem() : nextArrivalNo(1) {}

DeliverySystem::~DeliverySystem() {
    for (ListNode<Customer*>* n = customers.getHead(); n != nullptr; n = n->next) delete n->data;
    for (ListNode<Parcel*>* n = parcels.getHead(); n != nullptr; n = n->next) delete n->data;
}

// ---------------------------------------------------------------------------
// Adding records (shared by the menu options and the data-file loaders)
// ---------------------------------------------------------------------------

Customer* DeliverySystem::findCustomer(int id) const {
    for (ListNode<Customer*>* n = customers.getHead(); n != nullptr; n = n->next) {
        if (n->data->id == id) return n->data;
    }
    return nullptr;
}

bool DeliverySystem::addCustomer(int id, const string& name, const string& phone,
                                 const string& address, string& error) {
    if (id <= 0) {
        error = "customer ID must be positive";
        return false;
    }
    if (findCustomer(id) != nullptr) {
        error = "a customer with this ID already exists";
        return false;
    }
    customers.pushBack(new Customer(id, name, phone, address));
    return true;
}

bool DeliverySystem::addParcel(int trackingId, int customerId, const string& source,
                               const string& destination, double weight, int priority,
                               string& error) {
    if (trackingId <= 0) {
        error = "tracking ID must be positive";
    } else if (parcelTable.search(trackingId) != nullptr) {
        error = "a parcel with this tracking ID already exists";
    } else if (findCustomer(customerId) == nullptr) {
        error = "customer not found";
    } else if (network.findLocation(source) == -1 || network.findLocation(destination) == -1) {
        error = "source and destination must be locations in the city network";
    } else if (weight <= 0) {
        error = "weight must be greater than 0";
    } else if (priority < 1 || priority > 5) {
        error = "priority must be between 1 and 5";
    } else {
        Parcel* parcel = new Parcel(trackingId, customerId, source, destination, weight, priority);
        parcels.pushBack(parcel);
        parcelTable.insert(parcel);
        bstIndex.insert(trackingId, parcel);
        avlIndex.insert(trackingId, parcel);
        return true;
    }
    return false;
}

// ---------------------------------------------------------------------------
// Loading the sample data files
// ---------------------------------------------------------------------------

bool DeliverySystem::loadNetwork(const string& path) {
    ifstream file(path.c_str());
    if (!file) return false;

    int locations = 0, roads = 0;
    string line;
    while (getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        string fields[3];
        int count = splitFields(line, fields, 3);
        int weight;
        if (count == 1) {                       // a line with only a name is a location
            if (network.addLocation(fields[0]) != -1) locations++;
        } else if (count == 3 && parseInt(fields[2], weight)
                   && network.addRoad(network.findLocation(fields[0]),
                                      network.findLocation(fields[1]), weight)) {
            roads++;                            // name,name,distance is a road
        } else {
            cout << "  Skipped invalid line in " << path << ": " << line << "\n";
        }
    }
    cout << "Loaded " << locations << " locations and " << roads << " roads.\n";
    return true;
}

bool DeliverySystem::loadCustomers(const string& path) {
    ifstream file(path.c_str());
    if (!file) return false;

    int loaded = 0;
    string line;
    while (getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        string fields[4], error = "invalid line";
        int id;
        if (splitFields(line, fields, 4) == 4 && parseInt(fields[0], id)
            && addCustomer(id, fields[1], fields[2], fields[3], error)) {
            loaded++;
        } else {
            cout << "  Skipped customer (" << error << "): " << line << "\n";
        }
    }
    cout << "Loaded " << loaded << " customers.\n";
    return true;
}

bool DeliverySystem::loadParcels(const string& path) {
    ifstream file(path.c_str());
    if (!file) return false;

    int loaded = 0;
    string line;
    while (getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        string fields[6], error = "invalid line";
        int trackingId, customerId, priority;
        double weight;
        if (splitFields(line, fields, 6) == 6 && parseInt(fields[0], trackingId)
            && parseInt(fields[1], customerId) && parseDouble(fields[4], weight)
            && parseInt(fields[5], priority)
            && addParcel(trackingId, customerId, fields[2], fields[3], weight, priority, error)) {
            loaded++;
        } else {
            cout << "  Skipped parcel (" << error << "): " << line << "\n";
        }
    }
    cout << "Loaded " << loaded << " parcels.\n";
    return true;
}

// Order matters: parcels need customers and locations to exist first.
void DeliverySystem::loadData(const string& directory) {
    cout << "Loading data from '" << directory << "'...\n";
    if (!loadNetwork(directory + "/network.txt")) cout << "No network.txt found.\n";
    if (!loadCustomers(directory + "/customers.txt")) cout << "No customers.txt found.\n";
    if (!loadParcels(directory + "/parcels.txt")) cout << "No parcels.txt found.\n";
}

// ---------------------------------------------------------------------------
// Modules 1-3: customers, parcels, requests, dispatch, undo
// ---------------------------------------------------------------------------

void DeliverySystem::registerCustomer() {
    cout << "\n--- Register Customer ---\n";
    int id = readInt("Customer ID: ", 1, MAX_ID);
    string name = readText("Name: ");
    string phone = readText("Phone: ");
    string address = readText("Address: ");

    string error;
    if (addCustomer(id, name, phone, address, error)) cout << "Customer registered.\n";
    else cout << "Could not register customer: " << error << ".\n";
}

void DeliverySystem::registerParcel() {
    cout << "\n--- Register Parcel ---\n";
    if (network.getLocationCount() == 0) {
        cout << "Add locations to the city network first.\n";
        return;
    }

    int trackingId = readInt("Tracking ID: ", 1, MAX_ID);
    int customerId = readInt("Customer ID: ", 1, MAX_ID);
    network.displayLocations();
    int source = promptLocation("Source location number: ");
    int destination = promptLocation("Destination location number: ");
    double weight = readPositiveDouble("Weight (kg): ");
    int priority = readInt("Priority (1 = lowest, 5 = highest): ", 1, 5);

    string error;
    if (addParcel(trackingId, customerId, network.getName(source), network.getName(destination),
                  weight, priority, error)) {
        cout << "Parcel registered.\n";
    } else {
        cout << "Could not register parcel: " << error << ".\n";
    }
}

void DeliverySystem::searchParcel() {
    cout << "\n--- Search Parcel ---\n";
    int trackingId = readInt("Tracking ID: ", 1, MAX_ID);

    int probes;
    Parcel* parcel = parcelTable.search(trackingId, probes);
    if (parcel == nullptr) {
        cout << "No parcel with tracking ID " << trackingId << " (hash table: " << probes << " probe(s)).\n";
        return;
    }

    cout << "Found after " << probes << " probe(s) in the hash table:\n";
    printParcelRow(*parcel);
    Customer* customer = findCustomer(parcel->customerId);
    if (customer != nullptr) {
        cout << "  Customer: " << customer->name << ", " << customer->phone
             << ", " << customer->address << "\n";
    }
}

void DeliverySystem::addRequest(bool urgent) {
    cout << "\n--- Add " << (urgent ? "Urgent" : "Normal") << " Delivery Request ---\n";
    int trackingId = readInt("Tracking ID: ", 1, MAX_ID);

    Parcel* parcel = parcelTable.search(trackingId);
    if (parcel == nullptr) {
        cout << "Parcel not found.\n";
        return;
    }
    if (parcel->status != REGISTERED) {
        cout << "This parcel already has a request (status: " << statusName(parcel->status) << ").\n";
        return;
    }

    parcel->urgent = urgent;
    parcel->arrivalNo = nextArrivalNo++;
    parcel->status = PENDING;

    if (urgent) {
        urgentHeap.push(Request{trackingId, parcel->priority, parcel->arrivalNo});
        cout << "Urgent request added (priority " << parcel->priority << ").\n";
    } else {
        normalQueue.enqueue(trackingId);
        cout << "Normal request added to the queue.\n";
    }
}

void DeliverySystem::addNormalRequest() { addRequest(false); }
void DeliverySystem::addUrgentRequest() { addRequest(true); }

// Every status change is recorded for undo and added to the delivery history.
void DeliverySystem::changeStatus(Parcel* parcel, Status newStatus) {
    undoStack.push(UndoRecord{parcel->trackingId, parcel->status});
    parcel->status = newStatus;
    history.pushBack(HistoryEntry{parcel->trackingId, newStatus});
}

void DeliverySystem::processNextDelivery() {
    cout << "\n--- Process Next Delivery ---\n";
    Parcel* parcel = nullptr;

    // Urgent deliveries always go first; normal ones follow in arrival order.
    if (!urgentHeap.isEmpty()) {
        parcel = parcelTable.search(urgentHeap.pop().trackingId);
        cout << "Dispatching urgent delivery:\n";
    } else if (!normalQueue.isEmpty()) {
        parcel = parcelTable.search(normalQueue.dequeue());
        cout << "Dispatching normal delivery:\n";
    } else {
        cout << "There are no pending deliveries.\n";
        return;
    }

    changeStatus(parcel, DISPATCHED);
    printParcelRow(*parcel);
    network.printShortestRoute(network.findLocation(parcel->source),
                               network.findLocation(parcel->destination));
}

void DeliverySystem::markDelivered() {
    cout << "\n--- Mark Parcel as Delivered ---\n";
    int trackingId = readInt("Tracking ID: ", 1, MAX_ID);

    Parcel* parcel = parcelTable.search(trackingId);
    if (parcel == nullptr) {
        cout << "Parcel not found.\n";
    } else if (parcel->status != DISPATCHED) {
        cout << "Only dispatched parcels can be delivered (current status: "
             << statusName(parcel->status) << ").\n";
    } else {
        changeStatus(parcel, DELIVERED);
        cout << "Parcel #" << trackingId << " marked as delivered.\n";
    }
}

void DeliverySystem::undoLastOperation() {
    cout << "\n--- Undo Last Operation ---\n";
    if (undoStack.isEmpty()) {
        cout << "Nothing to undo.\n";
        return;
    }

    UndoRecord record = undoStack.pop();
    Parcel* parcel = parcelTable.search(record.trackingId);
    history.popBack();

    // Undoing a dispatch puts the parcel back where it came from.
    if (record.previousStatus == PENDING) {
        if (parcel->urgent) {
            urgentHeap.push(Request{parcel->trackingId, parcel->priority, parcel->arrivalNo});
        } else {
            normalQueue.enqueueFront(parcel->trackingId);
        }
    }
    parcel->status = record.previousStatus;

    cout << "Parcel #" << parcel->trackingId << " restored to status: "
         << statusName(parcel->status) << ".\n";
}

void DeliverySystem::displayPending() const {
    cout << "\n--- Pending Deliveries ---\n";

    cout << "Urgent (dispatch order):\n";
    if (urgentHeap.isEmpty()) cout << "  (none)\n";
    Heap<Request> copy(urgentHeap);   // pop from a copy so the real heap stays untouched
    while (!copy.isEmpty()) {
        Request request = copy.pop();
        Parcel* parcel = parcelTable.search(request.trackingId);
        cout << "  #" << request.trackingId << " | Priority " << request.priority
             << " | Arrival " << request.arrivalNo << " | " << parcel->destination << "\n";
    }

    cout << "Normal (arrival order):\n";
    if (normalQueue.isEmpty()) cout << "  (none)\n";
    for (ListNode<int>* n = normalQueue.getHead(); n != nullptr; n = n->next) {
        Parcel* parcel = parcelTable.search(n->data);
        cout << "  #" << n->data << " | Arrival " << parcel->arrivalNo
             << " | " << parcel->destination << "\n";
    }
}

void DeliverySystem::displayRecords() const {
    cout << "\n--- Customers ---\n";
    if (customers.isEmpty()) cout << "  (none)\n";
    for (ListNode<Customer*>* n = customers.getHead(); n != nullptr; n = n->next) {
        const Customer& c = *n->data;
        cout << "  " << c.id << " | " << c.name << " | " << c.phone << " | " << c.address << "\n";
    }

    cout << "\n--- Parcels (sorted by tracking ID) ---\n";
    if (parcels.isEmpty()) cout << "  (none)\n";
    avlIndex.inorder(printParcelRow);

    cout << "\n--- Delivery History ---\n";
    if (history.isEmpty()) cout << "  (none)\n";
    for (ListNode<HistoryEntry>* n = history.getHead(); n != nullptr; n = n->next) {
        cout << "  #" << n->data.trackingId << " -> " << statusName(n->data.status) << "\n";
    }
}

// ---------------------------------------------------------------------------
// Module 4: BST and AVL comparison
// ---------------------------------------------------------------------------

void DeliverySystem::compareTrees() const {
    cout << "\n--- BST vs AVL ---\n";
    cout << "Parcel index (" << parcels.size() << " parcels, inserted in registration order):\n";
    cout << "  BST height: " << bstIndex.height() << "\n";
    cout << "  AVL height: " << avlIndex.height() << "\n";

    cout << "\nTest with sorted tracking IDs 1..n (the worst case for a plain BST).\n";
    int n = readInt("n (1 to 1000): ", 1, 1000);

    BST testBst;
    AVL testAvl;
    for (int key = 1; key <= n; key++) {
        testBst.insert(key, nullptr);
        testAvl.insert(key, nullptr);
    }

    int smallestPossible = 0;
    while ((1 << smallestPossible) - 1 < n) smallestPossible++;

    cout << "  BST height: " << testBst.height() << "\n";
    cout << "  AVL height: " << testAvl.height() << "\n";
    cout << "  Smallest possible height for " << n << " nodes: " << smallestPossible << "\n";
}

// ---------------------------------------------------------------------------
// Modules 5-8: city network
// ---------------------------------------------------------------------------

int DeliverySystem::promptLocation(const string& prompt) const {
    return readInt(prompt, 1, network.getLocationCount()) - 1;
}

void DeliverySystem::promptAddRoad() {
    if (network.getLocationCount() < 2) {
        cout << "Add at least two locations first.\n";
        return;
    }
    network.displayLocations();
    int a = promptLocation("First location number: ");
    int b = promptLocation("Second location number: ");
    int distance = readInt("Distance in km: ", 1, 100000);

    if (network.addRoad(a, b, distance)) cout << "Road saved.\n";
    else cout << "A road must connect two different locations.\n";
}

void DeliverySystem::manageNetwork() {
    int choice;
    do {
        cout << "\n--- City Network ---\n"
             << "1. Display network\n"
             << "2. Add location\n"
             << "3. Add road\n"
             << "0. Back to main menu\n";
        choice = readInt("Choice: ", 0, 3);

        if (choice == 1) {
            network.display();
        } else if (choice == 2) {
            string name = readText("Location name: ");
            if (network.addLocation(name) != -1) cout << "Location added.\n";
            else cout << "That location already exists.\n";
        } else if (choice == 3) {
            promptAddRoad();
        }
    } while (choice != 0);
}

void DeliverySystem::traverseNetwork(bool useBfs) {
    cout << "\n--- " << (useBfs ? "BFS" : "DFS") << " Network Traversal ---\n";
    if (network.getLocationCount() == 0) {
        cout << "The network has no locations yet.\n";
        return;
    }

    network.displayLocations();
    int start = promptLocation("Start location number: ");

    int count;
    int* order = useBfs ? network.bfs(start, count) : network.dfs(start, count);
    cout << "Traversal order:\n  ";
    for (int i = 0; i < count; i++) {
        cout << network.getName(order[i]) << (i + 1 < count ? " -> " : "\n");
    }
    delete[] order;

    int target = promptLocation("Check if this location is reachable (number): ");
    cout << network.getName(target)
         << (network.isReachable(start, target) ? " is reachable from " : " is NOT reachable from ")
         << network.getName(start) << ".\n";
}

void DeliverySystem::runBfs() { traverseNetwork(true); }
void DeliverySystem::runDfs() { traverseNetwork(false); }

void DeliverySystem::findShortestRoute() {
    cout << "\n--- Shortest Delivery Route ---\n";
    if (network.getLocationCount() < 2) {
        cout << "The network needs at least two locations.\n";
        return;
    }
    network.displayLocations();
    int source = promptLocation("Source location number: ");
    int destination = promptLocation("Destination location number: ");
    network.printShortestRoute(source, destination);
}

void DeliverySystem::generateMinimumNetwork() const {
    cout << "\n--- Minimum Delivery Network ---";
    network.printMinimumSpanningTree();
}

// ---------------------------------------------------------------------------
// Statistics
// ---------------------------------------------------------------------------

void DeliverySystem::displayStatistics() const {
    int perStatus[4] = {0, 0, 0, 0};
    for (ListNode<Parcel*>* n = parcels.getHead(); n != nullptr; n = n->next) {
        perStatus[n->data->status]++;
    }

    cout << "\n--- System Statistics ---\n";
    cout << "Customers: " << customers.size() << "\n";
    cout << "Parcels: " << parcels.size() << " (registered " << perStatus[REGISTERED]
         << ", pending " << perStatus[PENDING] << ", dispatched " << perStatus[DISPATCHED]
         << ", delivered " << perStatus[DELIVERED] << ")\n";
    cout << "Pending urgent requests: " << urgentHeap.size() << "\n";
    cout << "Pending normal requests: " << normalQueue.size() << "\n";
    cout << "History entries: " << history.size() << ", undo steps available: "
         << undoStack.size() << "\n";
    cout << "Hash table: " << parcelTable.size() << " parcels in " << parcelTable.getCapacity()
         << " slots (load factor " << fixed << setprecision(2) << parcelTable.loadFactor() << ")\n";
    cout.unsetf(ios::fixed);
    cout << "Index height: BST " << bstIndex.height() << ", AVL " << avlIndex.height() << "\n";
    cout << "Network: " << network.getLocationCount() << " locations, "
         << network.getRoadCount() << " roads\n";
}
