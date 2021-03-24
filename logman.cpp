//  01BD41C3BF016AD7E8B6F837DF18926EC3E83350
//  logman.cpp
//  p3-logman
//
//  Created by James Zhang on 3/21/21.
//

#include "xcode_redirect.hpp"
#include "archive.h"

// ----------------------------------------------------------------------------
//                               Driver
// ----------------------------------------------------------------------------

int main(int argc, char** argv) {
    ios_base::sync_with_stdio(false); // Speeds up I/O.
    xcode_redirect(argc, argv); // Xcode redirect.
    
    string commandArg = argv[1];
    // Deals with command line options in the beginning.
    if (commandArg == "-h" || commandArg == "--help") {
        cout << "Log Manager. Please ask someone for help.\n";
        exit(0);
    }
    else {
        archive Archive; // Don't need initializer
        Archive.readMasterLog(commandArg);
        Archive.storeOrigMasterLog();
        Archive.getUserCommands();
        cout << "oh";
    }
    
    return 0;
}
