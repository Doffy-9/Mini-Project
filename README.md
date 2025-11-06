# 🚗 Parking Management System (C++ Mini Project)

### 📘 Overview
This project is a **console-based Parking Management System** written in **C++**, designed to manage vehicles across multiple floors efficiently.  
It supports vehicle parking, removal, waiting queue management, real-time slot tracking, and data persistence using files.

---

### 🧩 Key Features
- Multi-floor parking support using **graph-based floor connectivity**.
- **Automatic slot assignment** (lowest available slot first).
- **Waiting queue system** when parking is full.
- **Single-lane (LIFO)** record for parked vehicle order.
- **Regex validation** for vehicle number format (e.g., MH12AB1234).
- **Flat-rate billing system** (time-based).
- **Persistent data storage** via `parking_data.txt` and `parking_log.txt`.
- **Graph connectivity visualization** for floors.

---

### ⚙️ System Design

#### 🧱 Classes and Structures
1. **ParkingGraph**
   - Represents inter-floor connections using an adjacency list.
   - Each floor can be connected to others (e.g., ramps between floors).
   - Methods:
     - `addConnection(u, v)` → Connect two floors.
     - `getConnections(floor)` → Get directly connected floors.
     - `showConnections()` → Display floor connectivity.

2. **Vehicle (struct)**
   - Stores:
     - `number` — Vehicle number (validated format).
     - `type` — Vehicle type (Car/Bike/Bus label).
     - `entryTime` — Time of parking.
     - `floor`, `slot` — Location details.

3. **ParkingLot**
   - Manages parking operations.
   - Data structures used:
     - `priority_queue` → For available slots (ascending order).
     - `unordered_map` → To store parked vehicles.
     - `queue` → For waiting cars.
     - `stack` → For LIFO parking lane visualization.
   - Handles:
     - Vehicle parking/removal.
     - Slot allocation.
     - Waiting queue management.
     - Billing calculation.
     - Data saving/loading.

---

### 💰 Billing System
- **Flat rate:** ₹20/hour (applied uniformly for all vehicles).  
- Billing is calculated from **entry time to exit time** using system time.  
- Minimum charge = ₹20 (for less than 1 hour).

---

### 🧾 File Handling
The system uses two files for persistence:

| File Name | Description |
|------------|-------------|
| `parking_data.txt` | Stores active parked vehicles and their slot/floor info. |
| `parking_log.txt` | Logs all entry, exit, and waiting events with timestamps. |

These files are automatically **loaded at program start** and **updated on every change**.

---

### 🚀 How to Run

### Compile:
```bash
g++ mini.cpp -o mini
./mini
Enter number of floors: 3
Enter number of slots per floor: 5
[INFO] No previous data found. Creating new file.

===== Parking Management Menu =====
1. Park Vehicle
2. Remove Vehicle
3. Show Status
4. Show Floor Connections
0. Exit
Enter choice:
``` 
### 🧭 Example Workflow
1. The user enters the total number of floors and slots per floor.
2. Selects the **“Park Vehicle”** option and provides:
   - Vehicle number (validated by regex)
   - Vehicle type (Car/Bike/Bus)
   - Preferred floor
3. The system automatically assigns the **lowest available slot** on the chosen floor.
4. If the floor is full, it checks **connected floors** (via `ParkingGraph`) or adds the vehicle to the **waiting queue**.
5. When a vehicle leaves, the system:
   - Calculates the parking bill (flat rate)
   - Frees the slot
   - Parks the next waiting vehicle (if any)
6. Parking details and logs are automatically saved to:
   - `parking_data.txt`
   - `parking_log.txt`
7. On program restart, all data is **reloaded** from the saved files.

---

### ⚙️ Constraints and Rules
- Vehicle numbers must follow the Indian format: [A-Z]{2}[0-9]{1,2}[A-Z]{1,2}[0-9]{4}
Example: MH12AB1234  
- Duplicate vehicle numbers are **not allowed** (each must be unique).  
- The chosen floor must be valid (`0 ≤ floor < totalFloors`).  
- The system assigns the **lowest available slot number** first.  
- If all slots are full, the vehicle enters a **waiting queue**.  
- Upon removal of a vehicle, the first vehicle in the queue is automatically parked.  
- Data is **auto-saved** and **auto-loaded** between sessions.  

---

### 🧑‍💻 Technologies Used
| Component | Description |
|------------|-------------|
| **Language** | C++ |
| **Paradigm** | Object-Oriented Programming (OOP) |
| **Data Structures** | Queue, Stack, Priority Queue, Map, Graph |
| **Core Concepts** | File Handling, Regex Validation, Time Library |
| **Storage** | Persistent data in `parking_data.txt` and `parking_log.txt` |
| **Algorithms** | Graph traversal and floor connectivity management |

---

### 🏁 Future Enhancements
- Introduce **vehicle-specific billing rates** (different for car, bike, bus).  
- Add a **GUI or web interface** for better user interaction.  
- Implement **BFS-based nearest available floor finder**.  
- Generate **daily reports** — total earnings, vehicles parked, floor usage.  
- Add **admin authentication** and remote monitoring features.  
- Integrate **RFID or license plate recognition** for automatic entry/exit tracking.  

