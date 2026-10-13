#include <iostream>
#include <vector>
#include <iomanip>
#include <limits>
#include <string>

// Enum representing the status of each hash slot
enum SlotStatus {
    EMPTY,     // Slot has never been used
    OCCUPIED,  // Slot currently holds an account number
    DELETED    // Slot previously held an account (tombstone for proper probing)
};

// Structure representing an individual slot in the hash table
struct HashSlot {
    long long accountNumber;
    SlotStatus status;

    HashSlot() : accountNumber(-1), status(EMPTY) {}
};

// Hash Table class using Linear Probing for open addressing
class BankHashTable {
private:
    int tableSize;
    int currentCount;
    std::vector<HashSlot> table;

    // Primary Hash Function: index = x % table_size
    int hashFunction(long long x) const {
        return static_cast<int>(((x % tableSize) + tableSize) % tableSize);
    }

public:
    explicit BankHashTable(int size = 10) : tableSize(size), currentCount(0), table(size) {}

    bool isFull() const {
        return currentCount >= tableSize;
    }

    bool isEmpty() const {
        return currentCount == 0;
    }

    int getTableSize() const {
        return tableSize;
    }

    int getItemCount() const {
        return currentCount;
    }

    // 1. INSERT x:
    // Calculates index = x % table_size.
    // If occupied, uses linear probing: (index + 1) % table_size.
    bool insert(long long x) {
        if (x < 0) {
            std::cout << "\n[Error] Account number must be a non-negative integer.\n";
            return false;
        }

        // Check if account already exists
        int dummyProbes = 0;
        if (search(x, dummyProbes) != -1) {
            std::cout << "\n[Duplicate] Account number " << x << " is already present in the hash table.\n";
            return false;
        }

        if (isFull()) {
            std::cout << "\n[Overflow Error] Hash table is FULL! Cannot insert account " << x << ".\n";
            return false;
        }

        int initialIndex = hashFunction(x);
        int index = initialIndex;
        int probes = 0;
        int firstTombstone = -1;

        // Linear probing to locate an available slot
        while (probes < tableSize) {
            // Track the first deleted slot as a candidate for insertion
            if (table[index].status == DELETED && firstTombstone == -1) {
                firstTombstone = index;
            }

            // If an empty slot is encountered, we can safely stop probing
            if (table[index].status == EMPTY) {
                break;
            }

            probes++;
            index = (index + 1) % tableSize; // Linear probing step
        }

        // Use the first tombstone (deleted slot) if found, else use the empty slot
        int targetSlot = (firstTombstone != -1) ? firstTombstone : index;

        table[targetSlot].accountNumber = x;
        table[targetSlot].status = OCCUPIED;
        currentCount++;

        std::cout << "\n[Success] INSERT: Account " << x << "\n";
        std::cout << "  -> Primary Hash Key (x % " << tableSize << ") : Index " << initialIndex << "\n";
        std::cout << "  -> Placed at Final Position       : Slot " << targetSlot << "\n";
        std::cout << "  -> Probes / Collisions Resolved   : " << probes << "\n";
        return true;
    }

    // 2. SEARCH x:
    // Finds the slot of account number x using linear probing.
    // Returns index if found, or -1 if not found.
    int search(long long x, int& probeCount) const {
        probeCount = 0;
        if (isEmpty()) {
            return -1;
        }

        int initialIndex = hashFunction(x);
        int index = initialIndex;

        while (probeCount < tableSize) {
            probeCount++;

            // If we hit an EMPTY slot, the key does not exist in the table
            if (table[index].status == EMPTY) {
                return -1;
            }

            // If slot is OCCUPIED and matches the account number
            if (table[index].status == OCCUPIED && table[index].accountNumber == x) {
                return index;
            }

            // Otherwise, continue linear probing: (index + 1) % table_size
            index = (index + 1) % tableSize;
        }

        return -1; // Scanned entire table, key not present
    }

    // Helper for interactive user search
    void searchAndPrint(long long x) const {
        int probes = 0;
        int foundIndex = search(x, probes);

        std::cout << "\n------------------------------------------------------\n";
        std::cout << "               SEARCH ACCOUNT: " << x << "\n";
        std::cout << "------------------------------------------------------\n";
        int initialIndex = hashFunction(x);
        std::cout << "Primary Hash (x % " << tableSize << "): Index " << initialIndex << "\n";

        if (foundIndex != -1) {
            std::cout << "[FOUND] Account " << x << " is located at Slot " << foundIndex << "\n";
            std::cout << "Number of Probes needed: " << probes << "\n";
        } else {
            std::cout << "[NOT FOUND] Account " << x << " does not exist in the table.\n";
            std::cout << "Probes inspected: " << probes << "\n";
        }
        std::cout << "------------------------------------------------------\n";
    }

    // 3. DISPLAY:
    // Displays the entire hash table status and slots
    void display() const {
        std::cout << "\n========================================================================\n";
        std::cout << "                BANK HASH TABLE (Size = " << tableSize 
                  << ", Occupied = " << currentCount << ")\n";
        std::cout << "========================================================================\n";
        std::cout << std::left << std::setw(8)  << "Slot"
                  << std::left << std::setw(14) << "Status"
                  << std::left << std::setw(22) << "Account Number"
                  << std::left << std::setw(16) << "Primary Index"
                  << std::left << std::setw(12) << "Probing Offset" << "\n";
        std::cout << "------------------------------------------------------------------------\n";

        for (int i = 0; i < tableSize; ++i) {
            std::cout << std::left << std::setw(8) << i;

            if (table[i].status == OCCUPIED) {
                long long acc = table[i].accountNumber;
                int primary = hashFunction(acc);
                int offset = (i - primary + tableSize) % tableSize;

                std::cout << std::left << std::setw(14) << "OCCUPIED"
                          << std::left << std::setw(22) << acc
                          << std::left << std::setw(16) << primary
                          << std::left << std::setw(12) << offset << "\n";
            } else if (table[i].status == DELETED) {
                std::cout << std::left << std::setw(14) << "DELETED"
                          << std::left << std::setw(22) << "(tombstone)"
                          << std::left << std::setw(16) << "-"
                          << std::left << std::setw(12) << "-" << "\n";
            } else {
                std::cout << std::left << std::setw(14) << "EMPTY"
                          << std::left << std::setw(22) << "-"
                          << std::left << std::setw(16) << "-"
                          << std::left << std::setw(12) << "-" << "\n";
            }
        }
        std::cout << "========================================================================\n";
        std::cout << "Load Factor: " << std::fixed << std::setprecision(2) 
                  << (static_cast<double>(currentCount) / tableSize) << "\n";
        std::cout << "Collision Formula: (index + 1) % table_size\n";
        std::cout << "========================================================================\n";
    }

    // Optional / Bonus: DELETE x (marks slot as DELETED / tombstone)
    bool remove(long long x) {
        int probes = 0;
        int foundIndex = search(x, probes);

        if (foundIndex == -1) {
            std::cout << "\n[Error] Cannot delete: Account " << x << " not found in hash table.\n";
            return false;
        }

        table[foundIndex].status = DELETED;
        table[foundIndex].accountNumber = -1;
        currentCount--;

        std::cout << "\n[Success] Account " << x << " deleted from Slot " << foundIndex << ".\n";
        return true;
    }
};

// Safe input helper functions
void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int getValidInt(const std::string& prompt, int minVal = 1) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value && value >= minVal) {
            clearInputBuffer();
            return value;
        }
        std::cout << "[Invalid Input] Please enter a valid number (>= " << minVal << ").\n";
        clearInputBuffer();
    }
}

long long getValidLongLong(const std::string& prompt) {
    long long value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value && value >= 0) {
            clearInputBuffer();
            return value;
        }
        std::cout << "[Invalid Input] Please enter a valid positive account number.\n";
        clearInputBuffer();
    }
}

void printMenu() {
    std::cout << "\n=========================================\n";
    std::cout << "         BANK HASH TABLE MENU            \n";
    std::cout << "=========================================\n";
    std::cout << " 1. INSERT Account Number (x)\n";
    std::cout << " 2. SEARCH Account Number (x)\n";
    std::cout << " 3. DISPLAY Hash Table\n";
    std::cout << " 4. DELETE Account Number (x)\n";
    std::cout << " 5. Exit\n";
    std::cout << "=========================================\n";
}

int main() {
    std::cout << "=========================================================\n";
    std::cout << "      BANK ACCOUNT HASH TABLE WITH LINEAR PROBING        \n";
    std::cout << "=========================================================\n\n";

    // Set table size (default 10, or user-customizable)
    int size = getValidInt("Enter Hash Table Size (e.g., 10): ", 3);
    BankHashTable bankTable(size);

    // Pre-populate with typical sample account numbers to demonstrate linear probing & collisions
    std::cout << "\n--- Inserting Sample Bank Accounts to Demonstrate Linear Probing ---\n";
    std::vector<long long> sampleAccounts = {1023, 2033, 3043, 4015, 5026};

    for (long long acc : sampleAccounts) {
        bankTable.insert(acc);
    }

    bankTable.display();

    int choice = 0;
    while (choice != 5) {
        printMenu();
        choice = getValidInt("Enter your choice (1-5): ", 1);

        switch (choice) {
            case 1: {
                long long acc = getValidLongLong("Enter Account Number to INSERT: ");
                bankTable.insert(acc);
                break;
            }
            case 2: {
                long long acc = getValidLongLong("Enter Account Number to SEARCH: ");
                bankTable.searchAndPrint(acc);
                break;
            }
            case 3: {
                bankTable.display();
                break;
            }
            case 4: {
                long long acc = getValidLongLong("Enter Account Number to DELETE: ");
                bankTable.remove(acc);
                break;
            }
            case 5: {
                std::cout << "\nExiting Bank Hash Table System. Goodbye!\n";
                break;
            }
            default: {
                std::cout << "\n[Error] Invalid choice! Please select 1 to 5.\n";
                break;
            }
        }
    }

    return 0;
}
