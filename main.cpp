#include <iostream>
#include <stdexcept>
#include <string>
#include "DeliverySystem.h"
#include "DeliverySystem.cpp"
#include "BST.h"
#include "BST.cpp"
#include "AVL.cpp"
#include "AVL.h"
#include "Graph.h"
#include "Graph.cpp"
#include "HashTable.cpp"
#include "HashTable.h"
#include "Heap.h"
#include "LinkedList.h"
#include "Models.h"
#include "Queue.h"
#include "Stack.h"
#include "Input.h"

void printMenu() {
    std::cout << "\n=========================================\n"
              << " ROUTEWISE DELIVERY SYSTEM\n"
              << "=========================================\n"
              << "1. Register Customer\n"
              << "2. Register Parcel\n"
              << "3. Search Parcel\n"
              << "4. Add Normal Delivery Request\n"
              << "5. Add Urgent Delivery Request\n"
              << "6. Process Next Delivery\n"
              << "7. Mark Parcel as Delivered\n"
              << "8. Undo Last Operation\n"
              << "9. Display Pending Deliveries\n"
              << "10. Display Parcel Records\n"
              << "11. City Network (display / add location / add road)\n"
              << "12. BFS Network Traversal\n"
              << "13. DFS Network Traversal\n"
              << "14. Find Shortest Delivery Route\n"
              << "15. Generate Minimum Delivery Network\n"
              << "16. Compare BST and AVL Performance\n"
              << "17. Display System Statistics\n"
              << "18. Exit\n";
}

int main(int argc, char* argv[]) {
    std::string dataDirectory = (argc > 1) ? argv[1] : "data";

    DeliverySystem app;
    app.loadData(dataDirectory);

    try {
        int choice;
        do {
            printMenu();
            choice = readInt("Enter your choice: ", 1, 18);

            switch (choice) {
                case 1:  app.registerCustomer();         break;
                case 2:  app.registerParcel();           break;
                case 3:  app.searchParcel();             break;
                case 4:  app.addNormalRequest();         break;
                case 5:  app.addUrgentRequest();         break;
                case 6:  app.processNextDelivery();      break;
                case 7:  app.markDelivered();            break;
                case 8:  app.undoLastOperation();        break;
                case 9:  app.displayPending();           break;
                case 10: app.displayRecords();           break;
                case 11: app.manageNetwork();            break;
                case 12: app.runBfs();                   break;
                case 13: app.runDfs();                   break;
                case 14: app.findShortestRoute();        break;
                case 15: app.generateMinimumNetwork();   break;
                case 16: app.compareTrees();             break;
                case 17: app.displayStatistics();        break;
                case 18: std::cout << "Goodbye!\n";      break;
            }
        } while (choice != 18);
    } catch (const std::runtime_error&) {
        std::cout << "\nInput ended. Exiting.\n";
    }
    return 0;
}
