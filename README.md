# Operating Systems Labs

A collection of Operating Systems assignments implemented in C, covering Linux Kernel Modules, process management, and multi-threaded synchronization.

## 📂 Project Structure

| Directory / File | Description |
|------------------|-------------|
| **lab1/simple.c** | **Linux Kernel Module:** Loads into the kernel to print the `init_task` PCB information (PID, state, flags, priority) to the kernel log. |
| **lab2part1.c** | **Basic Synchronization:** Uses `pthread_mutex` to safely handle concurrent deposits and withdrawals on a shared bank balance. |
| **lab2part2.c** | **Conditional Synchronization:** Extends the bank model with constraints (Min Balance: 0, Max: 400), ensuring threads only proceed when valid. |
| **fork_and_print.c**| **Process Management:** Demonstrations of the `fork()` system call. |

## 🛠 Prerequisites

* **OS:** Linux (Ubuntu/WSL recommended)
* **Compiler:** GCC
* **Kernel Headers:** Required for Lab 1 (`sudo apt-get install linux-headers-$(uname -r)`)

## 🚀 Build & Run Instructions

### 1. Kernel Module (Lab 1)
*Requires root privileges.*

```bash
cd lab1
make                     # Compile using the Makefile
sudo insmod simple.ko    # Insert module into kernel
dmesg | tail             # View kernel log output (PCB Info)
sudo rmmod simple        # Remove module
```

## 2. Thread Synchronization (Lab 2)

This lab demonstrates thread synchronization using POSIX threads (`pthread`) and mutex locks in C to manage shared resources safely.

### Part 1: Basic Mutex
Implements a safe deposit and withdraw mechanism using a mutex to protect the shared balance variable from race conditions.

**Compile & Run:**
```bash
gcc lab2part1.c -o part1 -lpthread
./part1 100 50
# Usage: ./part1 <deposit_amount> <withdraw_amount>
```

### Part 2: Conditional Logic
Extends the bank model with balance constraints (Min: 0, Max: 400). Threads check the balance inside the critical section and only proceed if the transaction is valid.

**Compile & Run:**
```bash
gcc lab2part2.c -o part2 -lpthread
./part2 100
# Usage: ./part2 <amount> (Note: Logic requires amount to be 100)
```
