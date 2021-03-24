// B99292359FFD910ED13A7E6C7F9705B8742F0D79

/*

    A utility file containing the member function definitions of the class archive.

*/


#include "archive.h"

// Reads a series of log entries from the master log file.
void archive::readMasterLog(string fileName) {
    ifstream fin(fileName);
    entry temp;
    uint32_t numEntries = 0;
    
    // Checks if file did not open. REMOVE LATER when submitting.
    if (!fin.is_open()) {
        cout << "File did not open.\n";
        exit(1);
    }
    // Reads in timestamp, category, and message from one log entry
    while (getline(fin, temp.timestamp, '|')) {
        getline(fin, temp.category, '|');
        getline(fin, temp.message);
        // Sets entryID for each log entry.
        temp.entryID = numEntries;
        // Converts timestamp into a number for quicker comparison
        temp.timestampNum = stoll(temp.timestamp);
        
        // Pushes that log entry into the master list vector.
        masterLog.push_back(temp);
        
        // Increment number of entries just read in.
        ++ numEntries;
    }
    
    // Close the file for safety.
    fin.close();
    
    // Sort Master log File.
    logComparator logSort;
    sort(masterLog.begin(), masterLog.end(), logSort);
    
    // Print total number of entries read.
    cout << numEntries << " entries read\n";
}


// Accepts commands from the user and handles them accordingly.
void archive::getUserCommands() {
    char command;
    
    // Process commands until 'quit' is accepted.
    do {
        cout << "% ";
        cin >> command;
        
        // process command
        processCommands(command);
        
    } while (command != 'q');
    
    
}

// Processes incoming command and selects which action to take.
void archive::processCommands(char command) {
    string junk = "";
    switch (command) {
    // ----------------------------------------------------------------------------
    //                             SEARCHING COMMANDS
    // ----------------------------------------------------------------------------
        case 't': // Timestamps Search
            timestampsSearch();
            break;
            
        case 'm': // Single Timestamp Search
            matchingSearch();
            break;
            
        case 'c': // Category Search
            categorySearch();
            break;
            
        case 'k': // Keyword Search
            keywordSearch();
            break;
    // ----------------------------------------------------------------------------
    //                         EXCERPT LIST EDITING COMMANDS
    // ----------------------------------------------------------------------------
        case 'a': // Append log entry
            appendLogEntry();
            break;
            
        case 'r': // Append search results
            appendSearchResults();
            break;
            
        case 'd': // Delete log entry
            deleteLogEntry();
            break;
            
        case 'b': // Move to beginning
            moveToBeginning();
            break;
            
        case 'e': // Move to end
            moveToEnd();
            break;
            
        case 's': // Sort excerpt list
            sortExcerptList();
            break;
        
        case 'l': // Clear excerpt list
            clearExcerptList();
            break;
    // ----------------------------------------------------------------------------
    //                              OUTPUT COMMANDS
    // ----------------------------------------------------------------------------
        case 'g': // Print most recent search results
            printRecentSearch();
            break;
            
        case 'p': // Print excerpt list
            printExcerptlist();
            break;
            
        case 'q': // Quit
            return;
            
        case '#': // No operation
            getline(cin, junk);
            break;
    // ----------------------------------------------------------------------------
    //                         ERROR HANDLING: COMMANDS
    // ----------------------------------------------------------------------------
        default:
            cerr << "Command does not exist. Please enter proper command.\n";
            break;
    }
}

// ----------------------------------------------------------------------------
//                             SEARCHING COMMANDS
// ----------------------------------------------------------------------------

// Searches all log entries with timestamps within a specified range. (t)
void archive::timestampsSearch() {
    
}

// Searches all log entries with timestamps matching the given timestamp. (m)
void archive::matchingSearch() {
    
}

// Searches all log entries with the matching category. (c)
void archive::categorySearch() {
    
}

// Searches all log entries that contain every keyword given. (k)
void archive::keywordSearch() {

}

// ----------------------------------------------------------------------------
//                         EXCERPT LIST EDITING COMMANDS
// ----------------------------------------------------------------------------

// Append log entry by given position(entryID) to the end of excerpt list. (a)
void archive::appendLogEntry() {
    // Sorts a helper data structure the first time for constant access later on.
    if (!isAppendSort) {
        appendLogEntryHelper();
    }
    
    // Read in rest of input to retrieve position for master log.
    uint32_t position;
    cin >> position;
    
    // Checks if input is valid.
    if (position < 0 || position >= masterLog.size()) {
        cerr << "Invalid log entry position.\n";
        return;
    }
    
    // Append log at end of excerpt list.
    excerptList.push_back(masterLog[masterLogIndices[position]]);
    
    cout << "log entry " << position << " appended\n";
    
}

void archive::appendLogEntryHelper() {
    masterLogIndices.resize(masterLog.size()); // Reserve size
    
    // Sets the entryID index appropriately.
    for (uint32_t i = 0; i < masterLogIndices.size(); ++ i) {
        for (uint32_t j = 0; j < masterLog.size(); ++ j) {
            if (i == masterLog[j].entryID) {
                masterLogIndices[i] = j;
            }
        }
    }
    
    // Switches so this action is only done the first time 'a' is received.
    isAppendSort = 1;
}

// Append all log entries returned by most recent previous search. (r)
void archive::appendSearchResults() {
    
}

// Delete log entry by given position(entryID). (d)
void archive::deleteLogEntry() {
    
}

// Move log entry at given position(entryID) to the beginning of excerpt list. (b)
void archive::moveToBeginning() {
    
}

// Move log entry at given position(entryID) to the end of excerpt list. (e)
void archive::moveToEnd() {
    
}

// Sort excerpt list by timestamp -> category -> entryID. (s)
void archive::sortExcerptList() {
    
}

// Removes all entries from the excerpt list. (l)
void archive::clearExcerptList() {
    
}

// ----------------------------------------------------------------------------
//                              OUTPUT COMMANDS
// ----------------------------------------------------------------------------

// Print most recent serch results by timestamp -> category -> entryID. (g)
void archive::printRecentSearch() {
    
}

// Print all excerpt list entries. (p)
void archive::printExcerptlist() {
    // (Position)|(EntryID)|(Timestamp)|(Category)|(Message)|Newline
    for (uint32_t i = 0; i < excerptList.size(); ++ i) {
        cout << i << "|" << excerptList[i].entryID << "|" << excerptList[i].timestamp
        << "|" << excerptList[i].category << "|" << excerptList[i].message << "\n";
    }
}
