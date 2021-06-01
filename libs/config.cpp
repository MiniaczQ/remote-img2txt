#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <exception>

//	Post-compilation settings
namespace Config {
	//	Class for storing config properties
	class Instance {
	public:
		std::string serverIp;
		std::string clientIp;
		uint16_t cameraPort;
		uint16_t asciiPort;
		uint16_t consolePort;
		uint16_t logPort;
		uint16_t pictureW;
		uint16_t pictureH;
		uint16_t consoleW;
		uint16_t consoleH;
		uint16_t consoleP;
		uint32_t period;
		uint16_t vid;
		uint16_t pid;

		//	Load config from config_path file
		Instance(std::string config_path) {
			std::ifstream config_file(config_path);

			std::getline(config_file, serverIp);
			std::cout << "Server IP: " << serverIp << std::endl;
			std::getline(config_file, clientIp);
			std::cout << "Client IP: " << clientIp << std::endl;

			std::string buffer;

			std::getline(config_file, buffer);
			cameraPort = std::stoi(buffer);
			std::cout << "Camera port: " << cameraPort << std::endl;
			std::getline(config_file, buffer);
			asciiPort = std::stoi(buffer);
			std::cout << "ASCII port: " << asciiPort << std::endl;
			std::getline(config_file, buffer);
			consolePort = std::stoi(buffer);
			std::cout << "Console port: " << consolePort << std::endl;
			std::getline(config_file, buffer);
			logPort = std::stoi(buffer);
			std::cout << "Log port: " << logPort << std::endl;

			std::getline(config_file, buffer);
			pictureW = std::stoi(buffer);
			std::cout << "Picture width: " << pictureW << std::endl;
			std::getline(config_file, buffer);
			pictureH = std::stoi(buffer);
			std::cout << "Picture height: " << pictureH << std::endl;

			std::getline(config_file, buffer);
			consoleW = std::stoi(buffer);
			std::cout << "Console width: " << consoleW << std::endl;
			std::getline(config_file, buffer);
			consoleH = std::stoi(buffer);
			std::cout << "Console height: " << consoleH << std::endl;
			std::getline(config_file, buffer);
			consoleP = std::stoi(buffer);
			std::cout << "Console palette: " << consoleP << std::endl;

			std::getline(config_file, buffer);
			period = std::stoi(buffer);
			std::cout << "Period between frames [us]: " << period << std::endl;

			std::getline(config_file, buffer);
			vid = std::stoi(buffer);
			std::cout << "Vendor id: " << vid << std::endl;
			std::getline(config_file, buffer);
			pid = std::stoi(buffer);
			std::cout << "Product id: " << pid << std::endl;

			config_file.close();

			std::cout << "Config loaded." << std::endl << std::endl;
		}
	};

	//	Parse main() arguments and return loaded config
	Instance argsToConfig(int argc, char* argv[]) {
		std::string config_path;

		if (argc == 1) {
			std::cout << "Default config path." << std::endl;
			config_path = "../config.cfg";
		} else if (argc == 2) {
			std::cout << "Custom config path." << std::endl;
			config_path = std::string(argv[1]);
		} else {
			std::cout << "Invalid amount of arguments." << std::endl;
			throw -1;
		}
		std::cout << std::endl;

		return Instance(config_path);
	}
}