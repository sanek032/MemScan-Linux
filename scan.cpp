#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <print>
#include <sys/uio.h>

using namespace std;

struct MemoryRegion {
    uintptr_t start;
    uintptr_t end;
};

bool readMemory(int pid, uintptr_t addr, float& value) {
    struct iovec local[1];
    local[0].iov_base = &value;
    local[0].iov_len = sizeof(float);
    struct iovec remote[1];
    remote[0].iov_base = (void*)addr;
    remote[0].iov_len = sizeof(float);
    return process_vm_readv(pid, local, 1, remote, 1, 0) != -1;
}

int main() {
    int pid;
    float targetValue;
    vector<uintptr_t> foundAddresses;
    vector<MemoryRegion> writableRegions;

    print("Enter Target PID: "); 
    cin >> pid;
    print("Enter Initial Value: "); 
    cin >> targetValue;

    ifstream maps("/proc/" + to_string(pid) + "/maps");
    string line;
    while (getline(maps, line)) {
        if (line.find("rw-p") != string::npos) {
            size_t dashPos = line.find("-");
            size_t spacePos = line.find(" ", dashPos);
            uintptr_t start = stoull(line.substr(0, dashPos), nullptr, 16);
            uintptr_t end = stoull(line.substr(dashPos + 1, spacePos - dashPos - 1), nullptr, 16);
            writableRegions.push_back({start, end});
        }
    }

    print("Scanning {} memory regions...\n", writableRegions.size());

    for (auto& region : writableRegions) {
        for (uintptr_t addr = region.start; addr < region.end - sizeof(float); addr += 4) {
            float val;
            if (readMemory(pid, addr, val)) {
                if (val == targetValue) {
                    foundAddresses.push_back(addr);
                }
            }
        }
    }

    while (foundAddresses.size() > 1) {
        print("\nAddresses found: {}. Change the value in-game!\n", foundAddresses.size());
        print("1. Enter exact value\n2. Value increased\n3. Value decreased\n0. Stop and show list\nChoice: ");
        int choice; 
        cin >> choice;
        
        if (choice == 0) break;
        
        float newValue;
        if (choice == 1) {
            print("Enter new value: "); 
            cin >> newValue;
        }

        vector<uintptr_t> nextList;
        for (uintptr_t addr : foundAddresses) {
            float currentVal;
            if (readMemory(pid, addr, currentVal)) {
                bool keep = false;
                if (choice == 1 && currentVal == newValue) keep = true;
                if (choice == 2 && currentVal > targetValue) keep = true;
                if (choice == 3 && currentVal < targetValue) keep = true;

                if (keep) nextList.push_back(addr);
            }
        }

        foundAddresses = nextList;
        if (choice == 1) targetValue = newValue;
    }

    print("\nFinal Address List:\n");
    for (uintptr_t addr : foundAddresses) {
        print("{}\n", (void*)addr);
    }

    return 0;
}
