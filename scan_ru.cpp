#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <print>
#include <sys/uio.h>

using namespace std;

struct Region
{
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
    vector<Region> writableRegions;

    print("Введите PID игры: "); cin >> pid;
    print("Введите нужное значение: "); cin >> targetValue;

    ifstream maps("/proc/" + to_string(pid) + "/maps");
    string line;
    while (getline(maps, line)) {
        if (line.find("rw-p") != string::npos) {
            uintptr_t start = stoull(line.substr(0, line.find("-")), nullptr, 16);
            uintptr_t end = stoull(line.substr(line.find("-") + 1, line.find(" ")), nullptr, 16);
            writableRegions.push_back({start, end});
        }
    }
    print("Сканирую {} регионов памяти...\n", writableRegions.size());

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

    if (!foundAddresses.empty()) {
        print("Первый адрес из списка: {}\n", (void*)foundAddresses[0]);
    }

    while (foundAddresses.size() > 1) {
        print("\nНайдено адресов: {}. Измени значение в игре!\n", foundAddresses.size());
        print("1. Ввести точное значение\n2. Значение Увеличилось\n3. Значение уменьшилось\n0. Стоп и вывести список\nВыбор: ");
        int choice = 0; cin >> choice;
        if (choice == 0) break;
        
        float newValue;
        if (choice == 1) {
            print("Введите новое значение: "); cin >> newValue;
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
    print("\nФинальный список адресов:\n");
    for (uintptr_t addr : foundAddresses) {
        print("{}\n", (void*)addr);
    }

    return 0;
}
