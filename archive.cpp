// 01BD41C3BF016AD7E8B6F837DF18926EC3E83350

/*

    A utility file containing the member function definitions of the class archive.

*/


#include "archive.h"

 // ----------------------------------------------------------------------------
 //                             DRIVER FUNCTIONS
 // ----------------------------------------------------------------------------

// Reads a series of log entries from the master log file.
void archive::readMasterLog(const string& fileName) {
    ifstream fin(fileName);
    entry temp;
    string timestampNum;
    uint32_t numEntries = 0;
    
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
        
        // Increment number of entries just read in.
        ++ numEntries;
    }
    
    // Close the file for safety.
    fin.close();
    
    // Sort Master log File.
    logComparator logSort;
    sort(masterLog.begin(), masterLog.end(), logSort);
    
    // Reserve size of helper container.
    masterLogIndices.resize(masterLog.size());
    
    for (uint32_t i = 0; i < masterLog.size(); ++ i) {
        string tempCategory = masterLog[i].category;
        string tempMessage = masterLog[i].message;
        // Makes category lowercase so categories are case insensitive when being worked with.
        transform(tempCategory.begin(), tempCategory.end(), tempCategory.begin(), ::tolower);
        // Makes message lowercase so messages are case insensitive when being worked with.
        transform(tempMessage.begin(), tempMessage.end(), tempMessage.begin(), ::tolower);
        
        // Pushes each new category and stores the respective entry ID for that log.
        categoryLog[tempCategory].push_back(i);
        
        // Pushes each keyword in the entry and stores the respective entry ID for that log.
        populateKeywordLog(tempCategory, i);
        populateKeywordLog(tempMessage, i);
        
        // Sorts a helper data structure the first time for constant access later on.
        masterLogIndices[masterLog[i].entryID] = i;
    }
    
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
            if (!previouslySearched) {
                cerr << "No previous search has been done. Please try again.\n";
                break;
            }
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
            if (!previouslySearched) {
                cerr << "No previous search has been done. Please try again.\n";
                break;
            }
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
    if (previouslySearched) {
        // Clear search.
        recentSearches.clear();
    }
    
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
        previouslySearched = 0;
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
    
    // Switch to previously searched.
    previouslySearched = 1;
}

// Searches all log entries with timestamps matching the given timestamp. (m)
void archive::matchingSearch() {
    if (previouslySearched) {
        // Clear search.
        recentSearches.clear();
    }
    uint32_t totalSearches = 0;
    // Read in rest of input to retrieve matching timestamp.
    string timestamp;
    cin >> timestamp;
    
    // Checks if timestamp is valid.
    if (timestamp.size() != 14) {
        cerr << "Invalid timestamp size. Please enter proper timestamp.\n";
        previouslySearched = 0;
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
    
    previouslySearched = 1;
}

// Searches all log entries with the matching category. (c)
void archive::categorySearch() {
    if (previouslySearched) {
        // Clear search.
        recentSearches.clear();
    }
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
        recentSearches = categoryLog[category];
        totalSearches = categoryLog[category].size();
    }
    
    cout << "Category search: " << totalSearches << " entries found\n";
    
    // Switch to previously searched.
    previouslySearched = 1;
}

// Searches all log entries that contain every keyword given. (k)
void archive::keywordSearch() {
    if (previouslySearched) {
        // Clear search.
        recentSearches.clear();
    }
    
    string keywordsGiven;
    getline(cin, keywordsGiven);
    // Remove extra space read in using getline.
    keywordsGiven.erase(keywordsGiven.begin(), keywordsGiven.begin() + 1);
    // Makes keywords lowercase so keywords are case insensitive when being worked with.
    transform(keywordsGiven.begin(), keywordsGiven.end(), keywordsGiven.begin(), ::tolower);
    
    string keyword = "";
    vector<uint32_t> searchResult;
    vector<uint32_t>::iterator it;
    size_t totalSearches = 0;
    
    uint32_t startofWord = 0;
    uint32_t endofWord = 0;
    bool firstSet = 1;
    
    // If a char of the string is not alphanumerical, add the current read word in.
    for (auto ptr = keywordsGiven.c_str(); *ptr != '\0'; ++ ptr) {
        if (!isalnum(*ptr)) {
            if (startofWord != endofWord) {
                // Subtract endofWord and startofWord to get the length.
                // If keyword is not in master log, there will be no search entries.
                if (keywordLog.find(keywordsGiven.substr(startofWord, endofWord - startofWord)) == keywordLog.end()) {
                    //SET total search to 0
                    cout << "Keyword search: " << 0 << " entries found\n";
                    return;
                }
                // Find the first vector of entries for set intersection.
                if (firstSet == 1) {
                    searchResult = keywordLog[keywordsGiven.substr(startofWord, endofWord - startofWord)];
                    firstSet = 0;
                }
                else {
                    // Search up log entries if there is a matching keyword.
                    // Add log entried by entry ID to the resulting vector.
                    it = set_intersection(searchResult.begin(), searchResult.end(), keywordLog[keywordsGiven.substr(startofWord, endofWord - startofWord)].begin(), keywordLog[keywordsGiven.substr(startofWord, endofWord - startofWord)].end(), searchResult.begin());
                    searchResult.resize(static_cast<size_t>(it - searchResult.begin()));
                }
            }
            startofWord = endofWord + 1;
        }
        ++ endofWord;
    }
    // If the last word of the string has not been read in, do that here.
    if (startofWord != keywordsGiven.size()) {
        // If keyword is not in master log, there will be no search entries.
        if (keywordLog.find(keywordsGiven.substr(startofWord, endofWord - startofWord)) == keywordLog.end()) {
            //SET total search to 0
            cout << "Keyword search: " << 0 << " entries found\n";
            return;
        }
        // Find the first vector of entries for set intersection.
        if (firstSet == 1) {
            searchResult = keywordLog[keywordsGiven.substr(startofWord, endofWord - startofWord)];
            firstSet = 0;
        }
        else {
        // Search up log entries if there is a matching keyword.
        // Add log entried by entry ID to the resulting vector.
        it = set_intersection(searchResult.begin(), searchResult.end(), keywordLog[keywordsGiven.substr(startofWord, endofWord - startofWord)].begin(), keywordLog[keywordsGiven.substr(startofWord, endofWord - startofWord)].end(), searchResult.begin());
            searchResult.resize(static_cast<size_t>(it - searchResult.begin()));
        }
    }
    
    totalSearches = searchResult.size();
    for (uint32_t i = 0; i < searchResult.size(); ++ i) {
        // Add log entries by the entry ID.
        recentSearches.push_back(searchResult[i]);
    }
    cout << "Keyword search: " << totalSearches << " entries found\n";
    
    // Switch to previously searched.
    previouslySearched = 1;
}

// ----------------------------------------------------------------------------
//                         EXCERPT LIST EDITING COMMANDS
// ----------------------------------------------------------------------------

// Append log entry by given position(entryID) to the end of excerpt list. (a)
void archive::appendLogEntry() {
    // Read in rest of input to retrieve position for master log.
    int position;
    cin >> position;
    
    // Checks if position is valid.
    if (position < 0 || static_cast<size_t>(position) >= masterLog.size()) {
        cerr << "Invalid log entry position. Please enter valid position.\n";
        return;
    }
    
    // Append log at end of excerpt list.
    excerptList.push_back(masterLogIndices[static_cast<size_t>(position)]);
    
    cout << "log entry " << position << " appended\n";
    
}

// Append all log entries returned by most recent previous search. (r)
void archive::appendSearchResults() {
    size_t entriesAppended = 0;
    
    // Add all log entries from recent search to excerpt list. Should be sorted already.
    excerptList.insert(excerptList.end(), recentSearches.begin(), recentSearches.end());
    entriesAppended = recentSearches.size();
    
    cout << entriesAppended << " log entries appended\n";
}

// Delete log entry by given position. (d)
void archive::deleteLogEntry() {
    // Read in rest of input to retrieve position for excerpt list.
    int position;
    cin >> position;
    
    // Checks if position is valid.
    if (position < 0 || static_cast<size_t>(position) >= excerptList.size()) {
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
    int position;
    cin >> position;
    
    // Checks if position is valid.
    if (position < 0 || static_cast<size_t>(position) >= excerptList.size()) {
        cerr << "Invalid log entry position. Please enter valid position.\n";
        return;
    }
    
    // Store log entry in a temporary.
    uint32_t entryMoved = excerptList[static_cast<size_t>(position)];
    // Delete log entry at given position.
    excerptList.erase(excerptList.begin() + position);
    // Push the log entry to the beginning of the excerpt list.
    excerptList.push_front(entryMoved);
    
    cout << "Moved excerpt list entry " << position << "\n";
}

// Move log entry at given position to the end of excerpt list. (e)
void archive::moveToEnd() {
    // Read in rest of input to retrieve position excerpt list.
    int position;
    cin >> position;
    
    // Checks if position is valid.
    if (position < 0 || static_cast<size_t>(position) >= excerptList.size()) {
        cerr << "Invalid log entry position. Please enter valid position.\n";
        return;
    }
    
    // Store log entry in a temporary.
    uint32_t entryMoved = excerptList[static_cast<size_t>(position)];
    // Delete log entry at given position.
    excerptList.erase(excerptList.begin() + position);
    // Push the log entry to the end of the excerpt list.
    excerptList.push_back(entryMoved);
    
    cout << "Moved excerpt list entry " << position << "\n";
}

// Sort excerpt list by timestamp -> category -> entryID. (s)
void archive::sortExcerptList() {
    cout << "excerpt list sorted\n";
    
    // Print if excerpt list was previously empty.
    if (excerptList.empty()) {
        cout << "(previously empty)\n";
        return;
    }
    
    size_t lastElement = excerptList.size() - 1;
    
    // Print previous contents, with the first and last element.
    cout << "previous ordering:\n";
    
    cout << 0 << "|" << masterLog[excerptList[0]].entryID << "|" << masterLog[excerptList[0]].timestamp
    << "|" << masterLog[excerptList[0]].category << "|" << masterLog[excerptList[0]].message << "\n" << "...\n";
    
    cout << lastElement << "|" << masterLog[excerptList[lastElement]].entryID << "|" << masterLog[excerptList[lastElement]].timestamp
    << "|" << masterLog[excerptList[lastElement]].category << "|" << masterLog[excerptList[lastElement]].message << "\n";
    
    // Sort excerpt list.
    sort(excerptList.begin(), excerptList.end());
    
    // Print sorted contents, with the first and last element.
    cout << "new ordering:\n";
    
    cout << 0 << "|" << masterLog[excerptList[0]].entryID << "|" << masterLog[excerptList[0]].timestamp
    << "|" << masterLog[excerptList[0]].category << "|" << masterLog[excerptList[0]].message << "\n" << "...\n";
    
    cout << lastElement << "|" << masterLog[excerptList[lastElement]].entryID << "|" << masterLog[excerptList[lastElement]].timestamp
    << "|" << masterLog[excerptList[lastElement]].category << "|" << masterLog[excerptList[lastElement]].message << "\n";
}

// Removes all entries from the excerpt list. (l)
void archive::clearExcerptList() {
    cout << "excerpt list cleared\n";
    
    // Print if excerpt list was already empty.
    if (excerptList.empty()) {
        cout << "(previously empty)\n";
        return;
    }
    
    size_t lastElement = excerptList.size() - 1;
    
    // Print previous contents, with the first and last element.
    cout << "previous contents:\n";
    
    cout << 0 << "|" << masterLog[excerptList[0]].entryID << "|" << masterLog[excerptList[0]].timestamp
    << "|" << masterLog[excerptList[0]].category << "|" << masterLog[excerptList[0]].message << "\n" << "...\n";
    
    cout << lastElement << "|" << masterLog[excerptList[lastElement]].entryID << "|" << masterLog[excerptList[lastElement]].timestamp
    << "|" << masterLog[excerptList[lastElement]].category << "|" << masterLog[excerptList[lastElement]].message << "\n";
    
    
    excerptList.clear();
}

// ----------------------------------------------------------------------------
//                              OUTPUT COMMANDS
// ----------------------------------------------------------------------------

// Print most recent search results by timestamp -> category -> entryID. (g)
void archive::printRecentSearch() {
    // (EntryID)|(Timestamp)|(Category)|(Message)|Newline
    for (uint32_t i = 0; i < recentSearches.size(); ++ i) {
        cout << masterLog[recentSearches[i]].entryID << "|" << masterLog[recentSearches[i]].timestamp
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
