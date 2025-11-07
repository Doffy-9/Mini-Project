#include <iostream>
#include <queue>
#include <stack>
#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>
#include <regex>
#include <ctime>
#include <iomanip>
#include <cmath> // for ceil()
using namespace std;

// ---------- Graph class for floor connections ----------
class ParkingGraph {
    int floors;
    vector<vector<int>> adj;
public:
    ParkingGraph(int f) : floors(f) {
        adj.resize(f);
    }

    void addConnection(int u, int v) {
        if (u < floors && v < floors) {
            adj[u].push_back(v);
            adj[v].push_back(u);
        }
    }

    vector<int> getConnections(int floor) {
        if (floor < adj.size()) return adj[floor];
        return {};
    }

    void showConnections() {
        cout << "\nFloor Connectivity:\n";
        for (int i = 0; i < floors; ++i) {
            cout << "Floor " << i << " -> ";
            for (int j : adj[i]) cout << j << " ";
            cout << endl;
        }
    }
};

// ---------- Vehicle structure ----------
struct Vehicle {
    string number;
    string type;
    time_t entryTime;
    int floor;
    int slot;
};

// ---------- ParkingLot class ----------
class ParkingLot {
    int totalFloors;
    vector<priority_queue<int, vector<int>, greater<int>>> availableSlotsPerFloor;
    unordered_map<string, Vehicle> parked;
    queue<string> waiting;
    stack<string> lane;
    double totalEarnings = 0.0;

public:
    ParkingLot(int floors, int slotsPerFloor) {
        totalFloors = floors;
        availableSlotsPerFloor.resize(floors);
        for (int f = 0; f < floors; ++f)
            for (int s = 1; s <= slotsPerFloor; ++s)
                availableSlotsPerFloor[f].push(s);
    }

    bool isValidVehicleNumber(const string& v) {
        regex pattern("^[A-Z]{2}[0-9]{1,2}[A-Z]{1,2}[0-9]{4}$");
        return regex_match(v, pattern);
    }

    bool hasSpace(int floor) {
        return !availableSlotsPerFloor[floor].empty();
    }

    string formatTime(time_t t) {
        char buf[80];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&t));
        return string(buf);
    }

    // ---------- Updated Billing Logic ----------
    double calculateBill(const string& type, time_t inTime) {
    time_t outTime = time(0);
    double hours = difftime(outTime, inTime) / 3600.0;

    // Round up to the next full hour and apply minimum 1-hour charge
    hours = ceil(hours);
    if (hours < 1.0) hours = 1.0;

    double rate = 30.0; // fixed rate for all vehicle types
    double bill = rate * hours;

    return bill;
}


    void logEvent(const string& text) {
        ofstream log("parking_log.txt", ios::app);
        if (!log) {
            cerr << "Error: Unable to write to parking_log.txt\n";
            return;
        }
        log << "[" << formatTime(time(0)) << "] " << text << "\n";
        log.close();
    }

    // ---------- Park a vehicle ----------
    void parkVehicle(const string& num, const string& type, int floor, ParkingGraph& graph) {
        if (!isValidVehicleNumber(num)) {
            cout << "Invalid vehicle number format. Use like MH12AB1234.\n";
            return;
        }
        if (parked.find(num) != parked.end()) {
            cout << "Vehicle already parked.\n";
            return;
        }

        if (floor < 0 || floor >= totalFloors) {
            cout << "Invalid floor.\n";
            return;
        }

        if (hasSpace(floor)) {
            int slot = availableSlotsPerFloor[floor].top();
            availableSlotsPerFloor[floor].pop();

            Vehicle v = {num, type, time(0), floor, slot};
            parked[num] = v;
            lane.push(num);
            saveData();

            cout << "Vehicle " << num << " parked at Floor " << floor << ", Slot " << slot << endl;
            logEvent("[IN]  " + num + " | Type: " + type + " | Floor: " + to_string(floor) +
                     " | Slot: " + to_string(slot) + " | Time: " + formatTime(v.entryTime));
        } else {
            cout << "Preferred floor " << floor << " is full. Added to waiting queue.\n";
            waiting.push(num);
            logEvent("[WAIT] " + num + " added to waiting queue (preferred floor " + to_string(floor) + ")");
        }
    }

    // ---------- Remove a vehicle ----------
    void removeVehicle(const string& num) {
        auto it = parked.find(num);
        if (it == parked.end()) {
            cout << "Vehicle not found.\n";
            return;
        }

        Vehicle v = it->second;
        double bill = calculateBill(v.type, v.entryTime);
        totalEarnings += bill;

        parked.erase(it);
        availableSlotsPerFloor[v.floor].push(v.slot);
        saveData();

        cout << "Vehicle " << num << " removed from Floor " << v.floor
             << ", Slot " << v.slot << ". Bill: ₹" << fixed << setprecision(2) << bill << endl;

        logEvent("[OUT] " + num + " | Type: " + v.type + " | Floor: " + to_string(v.floor) +
                 " | Slot: " + to_string(v.slot) + " | Bill: ₹" + to_string(bill));

        if (!waiting.empty()) {
            string next = waiting.front();
            waiting.pop();
            cout << "Parking waiting vehicle " << next << "...\n";
            for (int f = 0; f < totalFloors; ++f) {
                if (hasSpace(f)) {
                    int slot = availableSlotsPerFloor[f].top();
                    availableSlotsPerFloor[f].pop();

                    Vehicle wv = {next, "CAR", time(0), f, slot};
                    parked[next] = wv;
                    saveData();
                    cout << "Vehicle " << next << " parked at Floor " << f << ", Slot " << slot << endl;
                    logEvent("[IN]  " + next + " (from waiting queue) | Floor: " + to_string(f) +
                             " | Slot: " + to_string(slot) + " | Time: " + formatTime(wv.entryTime));
                    return;
                }
            }
            cout << "No free slots available for waiting vehicles.\n";
            logEvent("[WAIT] " + next + " could not be parked (no free slots)");
        }
    }

    // ---------- Show status ----------
    void showStatus(int slotsPerFloor) {
        cout << "\n===== Parking Lot Status =====\n";
        if (parked.empty())
            cout << "No vehicles parked.\n";
        else
            for (auto& p : parked)
                cout << p.first << " -> Floor " << p.second.floor
                     << ", Slot " << p.second.slot << " (" << p.second.type << ")\n";

        cout << "\nTotal Earnings: ₹" << fixed << setprecision(2) << totalEarnings << endl;

        cout << "\nWaiting Queue: ";
        if (waiting.empty()) cout << "None\n";
        else {
            queue<string> temp = waiting;
            while (!temp.empty()) {
                cout << temp.front() << " ";
                temp.pop();
            }
            cout << endl;
        }

        cout << "\nSingle-lane Order: ";
        if (lane.empty()) cout << "None\n";
        else {
            stack<string> temp = lane;
            while (!temp.empty()) {
                cout << temp.top() << " ";
                temp.pop();
            }
            cout << endl;
        }

        cout << "\nFloor-wise Slots:\n";
        for (int f = 0; f < totalFloors; ++f) {
            cout << "Floor " << f << ": ";
            for (int s = 1; s <= slotsPerFloor; ++s) {
                bool occupied = false;
                for (auto& p : parked)
                    if (p.second.floor == f && p.second.slot == s)
                        occupied = true;
                cout << "[" << s << ":" << (occupied ? "X" : "_") << "] ";
            }
            cout << endl;
        }
    }

    // ---------- Save & Load ----------
    void saveData() {
        ofstream file("parking_data.txt", ios::trunc);
        if (!file) {
            cerr << "Error: Unable to open parking_data.txt for writing.\n";
            return;
        }
        for (auto& p : parked)
            file << p.second.number << " " << p.second.type
                 << " Floor:" << p.second.floor
                 << " Slot:" << p.second.slot
                 << " Time:" << formatTime(p.second.entryTime) << "\n";
        file.close();
        cout << "[DEBUG] Data saved successfully to parking_data.txt\n";
    }

    void loadData() {
        ifstream file("parking_data.txt");
        if (!file.is_open()) {
            cout << "[INFO] No previous data found. Creating new file.\n";
            ofstream create("parking_data.txt");
            create.close();
            return;
        }

        parked.clear();
        string num, type, floorLabel, slotLabel, timeLabel;
        int floor, slot;
        string timeStr;

        while (file >> num >> type >> floorLabel >> slotLabel >> timeLabel) {
            floor = stoi(floorLabel.substr(6));
            slot = stoi(slotLabel.substr(5));
            getline(file, timeStr);

            Vehicle v = {num, type, time(0), floor, slot};
            parked[num] = v;
            if (!availableSlotsPerFloor[floor].empty())
                availableSlotsPerFloor[floor].pop();
        }

        cout << "[DEBUG] Data loaded from parking_data.txt (" << parked.size() << " vehicles)\n";
        file.close();
    }
};

// ---------- MAIN ----------
int main() {
    int totalFloors, slotsPerFloor;

    cout << "Enter number of floors: ";
    cin >> totalFloors;
    cout << "Enter number of slots per floor: ";
    cin >> slotsPerFloor;

    ParkingGraph graph(totalFloors);
    for (int i = 0; i < totalFloors - 1; ++i)
        graph.addConnection(i, i + 1);

    ParkingLot parking(totalFloors, slotsPerFloor);
    parking.loadData();

    int choice;
    string num, type;
    do {
        cout << "\n===== Parking Management Menu =====\n";
        cout << "1. Park Vehicle\n";
        cout << "2. Remove Vehicle\n";
        cout << "3. Show Status\n";
        cout << "4. Show Floor Connections\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            cout << "Enter vehicle number (e.g., MH12AB1234): ";
            cin >> num;
            cout << "Enter vehicle type (car/bike/bus): ";
            cin >> type;
            int floor;
            cout << "Enter preferred floor (0-" << totalFloors - 1 << "): ";
            cin >> floor;
            parking.parkVehicle(num, type, floor, graph);
            break;
        case 2:
            cout << "Enter vehicle number to remove: ";
            cin >> num;
            parking.removeVehicle(num);
            break;
        case 3:
            parking.showStatus(slotsPerFloor);
            break;
        case 4:
            graph.showConnections();
            break;
        case 0:
            cout << "Exiting...\n";
            break;
        default:
            cout << "Invalid choice.\n";
        }
    } while (choice != 0);

    return 0;
}


