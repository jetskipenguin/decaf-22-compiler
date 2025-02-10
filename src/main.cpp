#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <algorithm>

int main() {
    std::cout << "Hello, World!" << std::endl;

    std::ifstream file("badbool.frag");
    if (!file.is_open()) {
        std::cerr << "Failed to open badbool.frag" << std::endl;
        return 1;
    }

    std::string content;
    std::string line;
    while (std::getline(file, line)) {
        content += line + "\n";
    }
    file.close();

    // std::regex bool_pattern("(TRUE|FALSE|True|False)");
    // std::smatch matches;
    
    // int line_num = 1;
    // int start_pos = 0;
    // std::string::const_iterator search_start(content.cbegin());
    
    // while (std::regex_search(search_start, content.cend(), matches, bool_pattern)) {
    
    //     // Calculate column by finding position from line start
    //     int col = matches.position() + 1 - start_pos;
        
    //     // Print match info
    //     std::cout << matches[0] << std::string(9 - matches[0].length(), ' ')
    //               << "line " << line_num << " cols " << col << "-" 
    //               << (col + matches[0].length() - 1) << " is T_Identifier" << std::endl;
        
    //     // Count newlines before next search position
    //     std::string prefix(content.begin(), search_start + matches.position() + matches.length());
    //     line_num += std::count(prefix.begin(), prefix.end(), '\n');
        
    //     // Update start position for next column calculation
    //     start_pos = matches.position() + matches.length();
        
    //     // Move iterator forward
    //     search_start = search_start + matches.position() + matches.length();
    // }


    return 0;
}
