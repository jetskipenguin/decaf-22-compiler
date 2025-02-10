#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <algorithm>

int main() {
    std::cout << "Hello, World!" << std::endl;

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

    auto matches_begin = std::sregex_iterator(content.begin(), content.end(), bool_pattern);
    auto matches_end = std::sregex_iterator();

    std::cout << "Found "
              << std::distance(matches_begin, matches_end)
              << " words\n";
    

    for(std::sregex_iterator i = matches_begin; i != matches_end; ++i) {
        std::smatch match = *i;
        std::string match_str = match.str();
        std::cout << "Match: " << match_str << std::endl;
    }


    return 0;
}
