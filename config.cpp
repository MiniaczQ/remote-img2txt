#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>

class Config {
public:
	std::string server_ip;
	std::string client_ip;
	uint16_t ascii_port;
	uint16_t log_port;
	uint16_t console_port;
	uint16_t picture_w;
	uint16_t picture_h;
	uint16_t console_w;
	uint16_t console_h;
	uint16_t console_p;

	Config(std::string config_path) {
		std::ifstream config_file(config_path);

		std::getline(config_file, server_ip);
		std::cout << "Server IP: " << server_ip << std::endl;
		std::getline(config_file, client_ip);
		std::cout << "Client IP: " << client_ip << std::endl;

		std::string buffer;

		std::getline(config_file, buffer);
		ascii_port = std::stoi(buffer);
		std::cout << "ASCII port: " << ascii_port << std::endl;
		std::getline(config_file, buffer);
		log_port = std::stoi(buffer);
		std::cout << "Log port: " << log_port << std::endl;
		std::getline(config_file, buffer);
		console_port = std::stoi(buffer);
		std::cout << "Console port: " << console_port << std::endl;

		std::getline(config_file, buffer);
		picture_w = std::stoi(buffer);
		std::cout << "Picture width: " << picture_w << std::endl;
		std::getline(config_file, buffer);
		picture_h = std::stoi(buffer);
		std::cout << "Picture height: " << picture_h << std::endl;

		std::getline(config_file, buffer);
		console_w = std::stoi(buffer);
		std::cout << "Console width: " << console_w << std::endl;
		std::getline(config_file, buffer);
		console_h = std::stoi(buffer);
		std::cout << "Console height: " << console_h << std::endl;
		std::getline(config_file, buffer);
		console_p = std::stoi(buffer);
		std::cout << "Console palette: " << console_p << std::endl;

		config_file.close();

		std::cout << "Config loaded." << std::endl << std::endl;
	}
};