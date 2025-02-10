#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <algorithm>

void find_match(std::string content, std::regex pattern, std::string identifier_name) 
{
    auto matches_begin = std::sregex_iterator(content.begin(), content.end(), pattern);
    auto matches_end = std::sregex_iterator();    

    for(std::sregex_iterator i = matches_begin; i != matches_end; ++i) {
        std::smatch match = *i;
        size_t pos = match.position();
        
        // Count newlines before the match to get line number
        size_t line_number = 1 + std::count(content.begin(), content.begin() + pos, '\n');
        
        // Find start of the line
        size_t line_start = content.rfind('\n', pos);
        if (line_start == std::string::npos) line_start = 0;
        else line_start++;
        
        // Calculate column (pos - start of line + 1 for 1-based indexing)
        size_t column = pos - line_start + 1;

        std::cout << match.str() 
                  << " line " << line_number 
                  << " cols " << column << "-" << column + match.str().length()
                  << " is " << identifier_name
                  << std::endl;
    }
}


int main() {
    std::ifstream file("/home/jetskipenguin/C++/decaf-22-compiler/samples/badbool.frag");
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

    std::regex bool_pattern("(TRUE|FALSE|True|False)");

    find_match(content, bool_pattern, "T_Identifier");

    return 0;
}
