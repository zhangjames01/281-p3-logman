// B99292359FFD910ED13A7E6C7F9705B8742F0D79

/*

    A utility file containing the member function definitions of the class archive.

*/


#include "archive.h"

// Reads a series of log entries from the master log file.
void archive::readMasterLog(string fileName) {
    ifstream fin(fileName);
    entry temp;
    string timestampNum;
    uint32_t numEntries = 0;
    
    // Checks if file did not open. REMOVE LATER when submitting.
    if (!fin.is_open()) {
        cout << "File did not open.\n";
        exit(1);
    }
    // Reads in timestamp, category, and message from one log entry.
    while (getline(fin, temp.timestamp, '|')) {
        getline(fin, temp.category, '|');
        getline(fin, temp.message);
        
        // Sets entryID for each log entry.
        temp.entryID = numEntries;
        timestampNum = temp.timestamp;
        // Manually erase the colons from timestamp.
        timestampNum.erase(remove(timestampNum.begin(), timestampNum.end(), ':'), timestampNum.end());
        // Converts timestamp into a number for quicker comparison.
        temp.timestampNum = stoll(timestampNum);
        
        // Pushes that log entry into the master list vector.
        masterLog.push_back(temp);

        // Makes category lowercase so categories are case insensitive when being worked with.
        transform(temp.category.begin(), temp.category.end(), temp.category.begin(), ::tolower);
        // Pushes each new category and stores the respective entry ID for that log.
        categoryLog[temp.category].push_back(temp.entryID);
        
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
    uint32_t totalSearches = 0;
    // Read in rest of input to retrieve timestamp boundaries.
    string timestamp1, timestamp2;
    getline(cin, timestamp1, '|');
    // Remove extra space read in using getline delimiter.
    timestamp1.erase(timestamp1.begin(), timestamp1.begin() + 1);
    cin >> timestamp2;
    
    // Checks if timestamp is valid.
    if (timestamp1.size() != 14 || timestamp2.size() != 14) {
        cerr << "Invalid timestamp size. Please enter proper timestamp.\n";
        return;
    }
    
    // Manually erase the colons from timestamp so it can be converted into a long long int.
    timestamp1.erase(remove(timestamp1.begin(), timestamp1.end(), ':'), timestamp1.end());
    timestamp2.erase(remove(timestamp2.begin(), timestamp2.end(), ':'), timestamp2.end());
    
    // Set lower and upper iterators to search through timestamps.
    auto start = lower_bound(masterLog.begin(), masterLog.end(), stoll(timestamp1), timestampComparator());
    auto end = upper_bound(masterLog.begin(), masterLog.end(), stoll(timestamp2), timestampComparator());
    
    // Add log entry to recent search and calculate total entries matching timestamp bounds.
    while (start != end) {
        recentSearches.push_back(masterLogIndices[(*start).entryID]);
        ++ totalSearches;
        ++ start;
    }
    
    cout << "Timestamps search: " << totalSearches << " entries found\n";
}

// Searches all log entries with timestamps matching the given timestamp. (m)
void archive::matchingSearch() {
    uint32_t totalSearches = 0;
    // Read in rest of input to retrieve matching timestamp.
    string timestamp;
    cin >> timestamp;
    
    // Checks if timestamp is valid.
    if (timestamp.size() != 14) {
        cerr << "Invalid timestamp size. Please enter proper timestamp.\n";
        return;
    }
    
    // Manually erase the colons from timestamp so it can be converted into a long long int.
    timestamp.erase(remove(timestamp.begin(), timestamp.end(), ':'), timestamp.end());
    
    // Set lower and upper iterators to search through timestamps.
    auto start = lower_bound(masterLog.begin(), masterLog.end(), stoll(timestamp), timestampComparator());
    auto end = upper_bound(masterLog.begin(), masterLog.end(), stoll(timestamp), timestampComparator());
    
    // Add log entry to recent search and calculate total entries matching timestamp.
    while (start != end) {
        recentSearches.push_back(masterLogIndices[(*start).entryID]);
        ++ totalSearches;
        ++ start;
    }
    
    cout << "Timestamp search: " << totalSearches << " entries found\n";
}

// Searches all log entries with the matching category. (c)
void archive::categorySearch() {
    size_t totalSearches = 0;
    // Read in rest of input to retrieve matching category.
    string category;
    getline(cin, category);
    // Remove extra space read in using getline.
    category.erase(category.begin(), category.begin() + 1);
    // Makes category lowercase so categories are case insensitive when being worked with.
    transform(category.begin(), category.end(), category.begin(), ::tolower);
    
    // Search up log entries if there is a matching category.
    if (categoryLog.find(category) != categoryLog.end()) {
        for (uint32_t i = 0; i < categoryLog[category].size(); ++ i) {
            // Add log entries by the entry ID.
            recentSearches.push_back(masterLogIndices[categoryLog[category][i]]);
        }
        totalSearches = categoryLog[category].size();
    }
    
    cout << "Category search: " << totalSearches << " entries found\n";
}

// Searches all log entries that contain every keyword given. (k)
void archive::keywordSearch() {

}

// ----------------------------------------------------------------------------
//                         EXCERPT LIST EDITING COMMANDS
// ----------------------------------------------------------------------------

// Append log entry by given position(entryID) to the end of excerpt list. (a)
void archive::appendLogEntry() {
    // Read in rest of input to retrieve position for master log.
    uint32_t position;
    cin >> position;
    
    // Checks if position is valid.
    if (position < 0 || position >= masterLog.size()) {
        cerr << "Invalid log entry position. Please enter valid position.\n";
        return;
    }
    
    // Append log at end of excerpt list.
    excerptList.push_back(masterLogIndices[position]);
    
    cout << "log entry " << position << " appended\n";
    
}

// Sorts a helper data structure the first time for constant access later on.
void archive::storeOrigMasterLog() {
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
    size_t entriesAppended = 0;
    
    // Add all log entries from recent search to excerpt list. Should be sorted already.
    for (uint32_t i = 0; i < recentSearches.size(); ++ i) {
        excerptList.push_back(recentSearches[i]);
    }
    entriesAppended = recentSearches.size();
    
    cout << entriesAppended << " log entries appended\n";
}

// Delete log entry by given position. (d)
void archive::deleteLogEntry() {
    // Read in rest of input to retrieve position for excerpt list.
    uint32_t position;
    cin >> position;
    
    // Checks if position is valid.
    if (position < 0 || position >= excerptList.size()) {
        cerr << "Invalid log entry position. Please enter valid position.\n";
        return;
    }
    
    // Delete log entry at given position.
    excerptList.erase(excerptList.begin() + position);
    
    cout << "Deleted excerpt list entry " << position << "\n";
}

// Move log entry at given position to the beginning of excerpt list. (b)
void archive::moveToBeginning() {
    // Read in rest of input to retrieve position excerpt list.
    uint32_t position;
    cin >> position;
    
    // Checks if position is valid.
    if (position < 0 || position >= excerptList.size()) {
        cerr << "Invalid log entry position. Please enter valid position.\n";
        return;
    }
    
    // Store log entry in a temporary.
    uint32_t entryMoved = excerptList[position];
    // Delete log entry at given position.
    excerptList.erase(excerptList.begin() + position);
    // Push the log entry to the beginning of the excerpt list.
    excerptList.push_front(entryMoved);
    
    cout << "Moved excerpt list entry " << position << "\n";
}

// Move log entry at given position to the end of excerpt list. (e)
void archive::moveToEnd() {
    // Read in rest of input to retrieve position excerpt list.
    uint32_t position;
    cin >> position;
    
    // Checks if position is valid.
    if (position < 0 || position >= excerptList.size()) {
        cerr << "Invalid log entry position. Please enter valid position.\n";
        return;
    }
    
    // Store log entry in a temporary.
    uint32_t entryMoved = excerptList[position];
    // Delete log entry at given position.
    excerptList.erase(excerptList.begin() + position);
    // Push the log entry to the end of the excerpt list.
    excerptList.push_back(entryMoved);
    
    cout << "Moved excerpt list entry " << position << "\n";
}

// Sort excerpt list by timestamp -> category -> entryID. (s)
void archive::sortExcerptList() {
    sort(excerptList.begin(), excerptList.end());
}

// Removes all entries from the excerpt list. (l)
void archive::clearExcerptList() {
    excerptList.clear();
}

// ----------------------------------------------------------------------------
//                              OUTPUT COMMANDS
// ----------------------------------------------------------------------------

// Print most recent search results by timestamp -> category -> entryID. (g)
void archive::printRecentSearch() {
    // (EntryID)|(Timestamp)|(Category)|(Message)|Newline
    for (uint32_t i = 0; i < recentSearches.size(); ++ i) {
        cout << i << "|" << masterLog[recentSearches[i]].entryID << "|" << masterLog[recentSearches[i]].timestamp
        << "|" << masterLog[recentSearches[i]].category << "|" << masterLog[recentSearches[i]].message << "\n";
    }
}

// Print all excerpt list entries. (p)
void archive::printExcerptlist() {
    // (Position)|(EntryID)|(Timestamp)|(Category)|(Message)|Newline
    for (uint32_t i = 0; i < excerptList.size(); ++ i) {
        cout << i << "|" << masterLog[excerptList[i]].entryID << "|" << masterLog[excerptList[i]].timestamp
        << "|" << masterLog[excerptList[i]].category << "|" << masterLog[excerptList[i]].message << "\n";
    }
}
