#include <iostream>
#include <queue>
#include <stack>
#include <unordered_map>
#include <vector>
#include <string>
#include <regex>
#include <ctime>
#include <iomanip>
using namespace std;

// Class for floor connectivity
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
        cout << "\nFloor Connectivity Map:\n";
        for (int i = 0; i < floors; ++i) {
            cout << "Floor " << i << " -> ";
            for (int j : adj[i]) cout << j << " ";
            cout << endl;
        }
    }
};

// Vehicle information
struct VehicleInfo {
    string number;
    string type;
    int floor;
    int slot;
    time_t entryTime;
};

// Parking management class
class ParkingLot {
    int totalFloors;
    int slotsPerFloor;
    double totalEarnings = 0;
    vector<priority_queue<int, vector<int>, greater<int>>> availableSlotsPerFloor;
    unordered_map<string, VehicleInfo> parkedVehicles;
    queue<pair<string, int>> waitingCars;
    stack<string> singleLane;

public:
    ParkingLot(int floors, int slots) {
        totalFloors = floors;
        slotsPerFloor = slots;
        availableSlotsPerFloor.resize(floors);
        for (int f = 0; f < floors; ++f)
            for (int s = 1; s <= slots; ++s)
                availableSlotsPerFloor[f].push(s);
    }

    bool hasSpaceOnFloor(int floor) {
        return !availableSlotsPerFloor[floor].empty();
    }

    bool isValidVehicleNumber(const string& num) {
        regex pattern("^[A-Z]{2}[0-9]{1,2}[A-Z]{1,2}[0-9]{1,4}$");
        return regex_match(num, pattern);
    }

    double getHourlyRate(const string& type) {
        if (type == "car" || type == "CAR") return 25.0;
        if (type == "bike" || type == "BIKE") return 10.0;
        if (type == "bus" || type == "BUS") return 50.0;
        return 20.0;
    }

    void parkVehicle(const string& vehicleNo, const string& type, int floor) {
        if (!isValidVehicleNumber(vehicleNo)) {
            cout << "Invalid vehicle number format. Use format like MH12AB1234.\n";
            return;
        }

        if (parkedVehicles.find(vehicleNo) != parkedVehicles.end()) {
            cout << "Vehicle already parked.\n";
            return;
        }

        if (floor < 0 || floor >= totalFloors) {
            cout << "Invalid floor number.\n";
            return;
        }

        if (!availableSlotsPerFloor[floor].empty()) {
            int slot = availableSlotsPerFloor[floor].top();
            availableSlotsPerFloor[floor].pop();

            VehicleInfo info = { vehicleNo, type, floor, slot, time(0) };
            parkedVehicles[vehicleNo] = info;
            singleLane.push(vehicleNo);

            cout << "Vehicle " << vehicleNo << " parked at Floor " << floor << ", Slot " << slot << ".\n";
        } else {
            cout << "Preferred floor " << floor << " is full. Vehicle added to waiting queue.\n";
            waitingCars.push({ vehicleNo, floor });
        }
    }

    void removeVehicle(const string& vehicleNo) {
        if (parkedVehicles.find(vehicleNo) == parkedVehicles.end()) {
            cout << "Vehicle not found.\n";
            return;
        }

        VehicleInfo info = parkedVehicles[vehicleNo];
        parkedVehicles.erase(vehicleNo);
        availableSlotsPerFloor[info.floor].push(info.slot);

        time_t exitTime = time(0);
        double hours = difftime(exitTime, info.entryTime) / 3600.0;
        if (hours < 1) hours = 1;

        double charge = getHourlyRate(info.type) * hours;
        totalEarnings += charge;

        cout << "Vehicle " << vehicleNo << " removed from Floor " << info.floor << ", Slot " << info.slot << ".\n";
        cout << fixed << setprecision(2);
        cout << "Parking Duration: " << hours << " hour(s), Charge: ₹" << charge << endl;

        if (!waitingCars.empty()) {
            string nextCar = waitingCars.front().first;
            int preferredFloor = waitingCars.front().second;
            waitingCars.pop();

            if (!availableSlotsPerFloor[preferredFloor].empty()) {
                int slot = availableSlotsPerFloor[preferredFloor].top();
                availableSlotsPerFloor[preferredFloor].pop();

                VehicleInfo nextInfo = { nextCar, "CAR", preferredFloor, slot, time(0) };
                parkedVehicles[nextCar] = nextInfo;
                cout << "Waiting vehicle " << nextCar << " parked at Floor " << preferredFloor << ", Slot " << slot << ".\n";
            } else {
                waitingCars.push({ nextCar, preferredFloor });
            }
        }
    }

    void showStatus() {
        cout << "\n===== Parking Lot Status =====\n";

        if (parkedVehicles.empty())
            cout << "No vehicles parked.\n";
        else
            for (auto& entry : parkedVehicles)
                cout << "Vehicle " << entry.second.number << " -> Floor " << entry.second.floor
                     << ", Slot " << entry.second.slot << " (" << entry.second.type << ")\n";

        cout << fixed << setprecision(2);
        cout << "\nTotal Earnings: ₹" << totalEarnings << endl;

        cout << "\nWaiting Queue: ";
        if (waitingCars.empty()) cout << "None\n";
        else {
            queue<pair<string, int>> temp = waitingCars;
            while (!temp.empty()) {
                cout << temp.front().first << " ";
                temp.pop();
            }
            cout << endl;
        }

        cout << "\nSingle-lane Order (LIFO): ";
        if (singleLane.empty()) cout << "None\n";
        else {
            stack<string> temp = singleLane;
            while (!temp.empty()) {
                cout << temp.top() << " ";
                temp.pop();
            }
            cout << endl;
        }

        cout << "\nFloor-wise Slot Map:\n";
        for (int f = 0; f < totalFloors; ++f) {
            cout << "Floor " << f << ": ";
            vector<bool> occupied(slotsPerFloor + 1, false);
            for (auto& p : parkedVehicles)
                if (p.second.floor == f)
                    occupied[p.second.slot] = true;

            for (int s = 1; s <= slotsPerFloor; ++s)
                cout << "[" << s << ":" << (occupied[s] ? "X" : "_") << "] ";
            cout << endl;
        }
    }
};

// Main function
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

    int choice;
    string vehicleNo, type;
    do {
        cout << "\n===== Parking Management Menu =====\n";
        cout << "1. Park Vehicle\n";
        cout << "2. Remove Vehicle\n";
        cout << "3. Show Parking Status\n";
        cout << "4. Show Floor Connections\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1: {
            cout << "Enter vehicle number (e.g., MH12AB1234): ";
            cin >> vehicleNo;
            cout << "Enter vehicle type (car/bike/bus): ";
            cin >> type;
            int floor;
            cout << "Enter preferred floor (0-" << totalFloors - 1 << "): ";
            cin >> floor;
            parking.parkVehicle(vehicleNo, type, floor);
            break;
        }
        case 2:
            cout << "Enter vehicle number to remove: ";
            cin >> vehicleNo;
            parking.removeVehicle(vehicleNo);
            break;
        case 3:
            parking.showStatus();
            break;
        case 4:
            graph.showConnections();
            break;
        case 0:
            cout << "Exiting program.\n";
            break;
        default:
            cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 0);

    return 0;
}

