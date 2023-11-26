#include <iostream>
#include <map>

class StringLookup {
public:
    // Constructor to initialize the lookup table
    StringLookup() : lookupTable({
        {1, "String1"},
        {2, "String2"},
        {3, "String3"}
        // Add more entries as needed
    }) {}

    // Function to perform the string lookup
    std::string lookupString(int id) const {
        // Check if the ID exists in the lookup table
        auto it = lookupTable.find(id);
        if (it != lookupTable.end()) {
            // Return the corresponding string
            return it->second;
        } else {
            // If ID is not found, return an empty string or an error message
            return "Not Found";
        }
    }

private:
    const std::map<int, std::string> lookupTable;
};
