// 01BD41C3BF016AD7E8B6F837DF18926EC3E83350

/*

    A utility file containing the declaration of the class archive.

*/

#include <vector>
#include <deque>
#include <unordered_map>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

class archive {
public:
    // Reads a series of log entries from the master log file.
    void readMasterLog(string fileName);
    
    // Accepts commands from the user and handles them accordingly.
    void getUserCommands();
    
    // Processes incoming command and selects which action to take.
    void processCommands(char command);
    
    // ----------------------------------------------------------------------------
    //                             SEARCHING COMMANDS
    // ----------------------------------------------------------------------------
    
    // Searches all log entries with timestamps within a specified range. (t)
    void timestampsSearch();
    
    // Searches all log entries with timestamps matching the given timestamp. (m)
    void matchingSearch();
    
    // Searches all log entries with the matching category. (c)
    void categorySearch();
    
    // Searches all log entries that contain every keyword given. (k)
    void keywordSearch();
    
    // ----------------------------------------------------------------------------
    //                         EXCERPT LIST EDITING COMMANDS
    // ----------------------------------------------------------------------------
    
    // Append log entry by given position(entryID) to the end of excerpt list. (a)
    void appendLogEntry();
    
    // Append all log entries returned by most recent previous search. (r)
    void appendSearchResults();
    
    // Delete log entry by given position(entryID). (d)
    void deleteLogEntry();
    
    // Move log entry at given position(entryID) to the beginning of excerpt list. (b)
    void moveToBeginning();
    
    // Move log entry at given position(entryID) to the end of excerpt list. (e)
    void moveToEnd();
    
    // Sort excerpt list by timestamp -> category -> entryID. (s)
    void sortExcerptList();
    
    // Removes all entries from the excerpt list. (l)
    void clearExcerptList();
    
    // ----------------------------------------------------------------------------
    //                              OUTPUT COMMANDS
    // ----------------------------------------------------------------------------
    
    // Print most recent serch results by timestamp -> category -> entryID. (g)
    void printRecentSearch();
    
    // Print all excerpt list entries. (p)
    void printExcerptlist();
    
    
    
    
    
    //HELPER FUNCTION
    void appendLogEntryHelper();
    
    
    
private:
    struct entry { // Container for a single log entry.
        uint32_t entryID;
        int64_t timestampNum;
        string timestamp; // make this uint 64 later for comparing???
        string category;
        string message;
    };
    
    // Comparator for sorting Master Log List. Sort by timestamp -> category -> entryID.
    struct logComparator {
        bool operator()(const entry& e1, const entry& e2) const {
            if (e1.timestampNum == e2.timestampNum) {
                if (strcasecmp(e1.category.c_str(), e2.category.c_str()) == 0) {
                    return e1.entryID < e2.entryID;
                }
                else { // CHECK THIS LATER
                    return (strcasecmp(e1.category.c_str(), e2.category.c_str()) < 0);
                }
            }
            else {
                return e1.timestampNum < e2.timestampNum;
            }
        }
    };
    
    
    vector<entry> masterLog; // Master list of all log entries read in.
    
    // Helper data vector that helps with quick retrieval of log entries by original position.
    vector<uint32_t> masterLogIndices;
    
    deque<entry> excerptList; // Excerpt list of log entries.

    bool isAppendSort = 0;
};
