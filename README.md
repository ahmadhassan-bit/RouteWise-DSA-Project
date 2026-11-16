# RouteWise – Smart Delivery and Route Optimization System

A console-based C++ application for a courier company. It manages customers and parcels,
dispatches normal and urgent deliveries, supports undo, indexes parcels in BST/AVL trees,
and plans routes over a city road network.

Built for **CL2001 – Data Structures Lab**. All data structures are implemented from scratch;
no STL containers (`vector`, `map`, `queue`, `priority_queue`, ...) are used.

## Features

- Register customers and parcels (dynamic memory, fast lookup by tracking ID)
- Normal requests (FIFO queue) and urgent requests (priority heap, ties broken by arrival order)
- Dispatch the next delivery, mark parcels delivered, undo the last status change
- Delivery history, pending-delivery view, system statistics
- BST and AVL parcel index with a height comparison
- City network with adjacency list and adjacency matrix
- BFS, DFS and reachability check
- Shortest route (Dijkstra) and minimum delivery network (Prim)

## Data structures used

| Structure | File | Used for |
|---|---|---|
| Doubly linked list | `LinkedList.h` | Customers, parcels, delivery history, queue storage, adjacency lists |
| Stack (array) | `Stack.h` | Undo of status changes, iterative DFS |
| Queue (linked) | `Queue.h` | Normal delivery requests, BFS |
| Max-heap (array) | `Heap.h` | Urgent deliveries (highest priority first, then earliest arrival) |
| Hash table, linear probing | `HashTable.h/.cpp` | Parcel lookup by tracking ID |
| BST | `BST.h/.cpp` | Ordered parcel index |
| AVL tree | `AVL.h/.cpp` | Balanced parcel index (extends the BST with rotations) |
| Graph (list + matrix) | `Graph.h/.cpp` | Road network, BFS, DFS, Dijkstra, Prim |

`DeliverySystem` (`DeliverySystem.h/.cpp`) connects everything; `main.cpp` is just the menu.

## Build and run

Requires a C++11 compiler (g++ or clang++).

```
make
./routewise            # loads the sample data from ./data
```

Without `make`:

```
g++ -std=c++11 -o routewise src/main.cpp src/DeliverySystem.cpp src/Graph.cpp src/HashTable.cpp src/BST.cpp src/AVL.cpp
```

Run it from the project folder. To use another data folder: `./routewise path/to/folder`.
A folder without data files starts the system empty.

A scripted demo that covers every required capability is included:

```
./routewise < data/demo_input.txt
```

## Data files (`data/`)

Lines starting with `#` and blank lines are ignored.

- `network.txt` – a line with just a name is a location; `name,name,distance` is a road
- `customers.txt` – `id,name,phone,address`
- `parcels.txt` – `trackingId,customerId,source,destination,weight,priority`

The program is not tied to this data. Everything can also be entered through the menu.

## Notes on behaviour

- Priority is 1 (lowest) to 5 (highest). Urgent requests use the parcel's priority.
  Urgent deliveries are always dispatched before normal ones.
- Undo reverses the most recent **dispatch** or **delivery**. Undoing a dispatch puts the
  parcel back in the queue or heap in its original position.
- The sample parcels have ascending tracking IDs, so the plain BST becomes a chain while
  the AVL tree stays balanced. Option 16 also tests sorted keys 1..n.
- Option 11 opens a small sub-menu to display the network, add a location and add a road.

## Time complexity

n = parcels, V = locations, E = roads, h = tree height.

| Operation | Complexity |
|---|---|
| Linked list push/pop at either end | O(1) |
| Stack push/pop | O(1) amortized |
| Queue enqueue/dequeue | O(1) |
| Heap push/pop | O(log n) |
| Hash table insert/search | O(1) average, O(n) worst case |
| BST insert/search | O(h): O(log n) average, O(n) worst case |
| AVL insert/search | O(log n) |
| Tree in-order traversal | O(n) |
| Add location | O(V) name check, plus O(V²) when the arrays grow |
| Add road | O(1) |
| BFS / DFS | O(V + E) |
| Shortest path (Dijkstra, array-based) | O(V²) |
| Minimum spanning tree (Prim, matrix) | O(V²) |
| Find a customer | O(customers) |

## Screenshots

Add screenshots of parcel registration, priority dispatching, tracking lookup and route
optimization to a `screenshots/` folder and link them here.
