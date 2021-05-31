#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>

#include "../config.cpp"

int main(int argc, char* argv[]) {
    std::string config_path;

    if (argc == 1) {
        std::cout << "Default config path.";
        config_path = "../config.cfg";
    } else if (argc == 2) {
        std::cout << "Custom config path.";
        config_path = std::string(argv[1]);
    } else {
        std::cout << "Invalid amount of arguments.";
        return -1;
    }
    std::cout << std::endl
              << std::endl;

    Config cfg(config_path);

    

	return 0;
}