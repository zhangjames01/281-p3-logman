// 01BD41C3BF016AD7E8B6F837DF18926EC3E83350

/*

    A utility file containing the declaration of the class archive.

*/

#include <vector>
#include <deque>
#include <unordered_map>
#include <strings.h>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

class archive {
public:
    
    // ----------------------------------------------------------------------------
    //                             DRIVER FUNCTIONS
    // ----------------------------------------------------------------------------
    
    // Reads a series of log entries from the master log file.
    void readMasterLog(const string& fileName);
    
    // Accepts commands from the user and handles them accordingly.
    void getUserCommands();
    
    // Processes incoming command and selects which action to take.
    void processCommands(char command);
    
    
private:
    // Switch to see if a previous search was completed for certain command functions.
    bool previouslySearched = 0;
    
    bool previousTimestampSearch = 0;
    
    bool timestampAppended = 0;
    
    
    // Container for a single log entry.
    struct entry {
        uint32_t entryID;
        int64_t timestampNum;
        string timestamp;
        string category;
        string message;
    };
    
    // Comparator for sorting Master Log List. Sort by timestamp -> category -> entryID.
    struct logComparator {
        bool operator()(const entry& e1, const entry& e2) const {
            // If timestamp is a tie.
            if (e1.timestampNum == e2.timestampNum) {
                // If category is a tie.
                if (strcasecmp(e1.category.c_str(), e2.category.c_str()) == 0) {
                    return e1.entryID < e2.entryID;
                }
                else {
                    return (strcasecmp(e1.category.c_str(), e2.category.c_str()) < 0);
                }
            }
            else {
                return e1.timestampNum < e2.timestampNum;
            }
        }
    };
    
    // Comparator for finding lower and upper bounds for timestamps search.
    struct timestampComparator {
        bool operator() (const int64_t timestamp, const entry& e1) const { // Used by upper_bound
            return timestamp < e1.timestampNum;
        }
        bool operator() (const entry& e1, const int64_t timestamp) const { // Used by lower_bound
            return e1.timestampNum < timestamp;
        }
    };
    
    vector<entry> masterLog; // Master list of all log entries read in.
    
    vector<entry>::iterator start;
        // FOR TIMESTAMP SEARCHES
    vector<entry>::iterator end;
    
    // Helper data vector that helps with quick retrieval of log entries by original position.
    vector<uint32_t> masterLogIndices;
    
    deque<uint32_t> excerptList; // Excerpt list of log entries using indices.
    
    vector<uint32_t> recentSearches; // Storage of entries from recent searches.
    
    // Contains every category from the master log file.
    unordered_map<string, vector<uint32_t>> categoryLog;
    
    // Contains every keyword possible in the master log file.
    unordered_map<string, vector<uint32_t>> keywordLog;
    
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
    
    // Delete log entry by given position. (d)
    void deleteLogEntry();
    
    // Move log entry at given position to the beginning of excerpt list. (b)
    void moveToBeginning();
    
    // Move log entry at given position to the end of excerpt list. (e)
    void moveToEnd();
    
    // Sort excerpt list by timestamp -> category -> entryID. (s)
    void sortExcerptList();
    
    // Removes all entries from the excerpt list. (l)
    void clearExcerptList();
    
    // ----------------------------------------------------------------------------
    //                              OUTPUT COMMANDS
    // ----------------------------------------------------------------------------
    
    // Print most recent search results by timestamp -> category -> entryID. (g)
    void printRecentSearch();
    
    // Print all excerpt list entries. (p)
    void printExcerptlist();
    
    // ----------------------------------------------------------------------------
    //                              HELPER FUNCTIONS
    // ----------------------------------------------------------------------------
    
    // Helper function to populate the unordered map with all the keywords.
    void populateKeywordLog(string& phrase, uint32_t entryID) {
        uint32_t startofWord = 0;
        uint32_t endofWord = 0;
        
        // If a char of the string is not alphanumerical, add the current read word in.
        for (auto ptr = phrase.c_str(); *ptr != '\0'; ++ ptr) {
            if (!isalnum(*ptr)) {
                if (startofWord != endofWord) {
                    string tempPhrase = phrase.substr(startofWord, endofWord - startofWord);
                    // Subtract endofWord and startofWord to get the length.
                    keywordLog[tempPhrase].push_back(entryID);
                    // Duplicates
                    if (keywordLog[tempPhrase].size() == 1) {
                        
                    }
                    else if (keywordLog[tempPhrase][keywordLog[tempPhrase].size() - 2] == entryID) {
                        keywordLog[tempPhrase].pop_back();
                    }
                    else {}
                }
                startofWord = endofWord + 1;
            }
            ++ endofWord;
        }
        // If the last word of the string has not been read in, do that here.
        if (startofWord != phrase.size()) {
            string tempPhrase = phrase.substr(startofWord, endofWord - startofWord);
            keywordLog[tempPhrase].push_back(entryID);
            if (keywordLog[tempPhrase].size() == 1) {
                
            }
            else if (keywordLog[tempPhrase][keywordLog[tempPhrase].size() - 2] == entryID) {
                keywordLog[tempPhrase].pop_back();
            }
            else {}
        }
    }
};
