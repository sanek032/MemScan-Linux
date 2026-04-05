# MemScan-Linux

A lightweight, high-performance memory scanner for Linux processes, written in C++23. It functions as a CLI-based alternative to Cheat Engine, allowing you to find and filter memory addresses in real-time.



## Features
- **Memory Region Parsing**: Automatically scans `/proc/[pid]/maps` to find writable memory segments (`rw-p`).
- **Real-time Filtering**: Supports initial scan and subsequent filtering (Exact value, Increased, Decreased).
- **Process VM Access**: Utilizes `process_vm_readv` for fast inter-process memory reading.
- **Modern C++**: Built using C++23 features like `std::print`.

## Requirements
- Linux OS
- G++ compiler with C++23 support
- Root privileges (required for `process_vm_readv` access)

## Installation & Usage

1. **Clone the repository:**
   ```bash
   git clone https://github.com/sanek032/MemScan-Linux
   cd MemScan-Linux
```
    Compile the scanner:
    ```bash

    g++ -std=c++23 main.cpp -o memscan
```
    Run with sudo:
    ```bash

    sudo ./memscan
```
How it works

    Target PID: Enter the PID of the running game or application.

    Initial Scan: Enter the current value you are looking for (e.g., player health).

    Filtering: Change the value in the target application, then use the filter options (Increased/Decreased/Exact) to narrow down the results until the exact memory address is found.

Disclaimer

This tool is for educational and research purposes only. Using it on protected software or online games may violate their Terms of Service and lead to bans.
