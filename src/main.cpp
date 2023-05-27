#include <deque>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <string>
#include <cstring>
#include <ctime>

#include "warn.hpp"

void ffprint(std::string path, std::ofstream *out) {
	std::ifstream f(path);
	if (!f.is_open()) {
		std::cerr << "Could not find libraries!" << std::endl;
		exit(1);
	}
	std::string line;
	while (std::getline(f, line)) {
		*out << line << '\n';
	}
	f.close();
}

bool handle_args(char *arg) {
	bool credit = true;
	if (strcmp(arg, "version") == 0) {
		std::cout << "CPCC version " << CPCC_VER << std::endl;
		exit(0);
	}
	else if (strcmp(arg, "libs") == 0) {
		std::cout << "Available libraries: ";
		for (const auto &entry : std::filesystem::directory_iterator(std::string(CPCC_INCLUDE_DIR) + "utils")) {
			std::string file = entry.path().filename().string();
			if (file[0] == '.') continue;
			std::cout << file << ' ';
		}
		std::cout << std::endl;
		exit(0);
	}
	else if (strcmp(arg, "nocredit") == 0) {
		credit = false;
	}
	else {
		std::cerr << "Invalid argument " << arg << " supplied!" << std::endl;
		exit(1);
	}
	return credit;
}

int main(int argc, char *argv[]) {
	if (argc < 2 || strcmp(argv[1], "--help") == 0) {
		std::cout << "usage: " << argv[0] << " [--help] [--version] [--libs] [--nocredit] <filename>\n"
			<< "arguments:\n"
			<< "	--help		Prints this message\n"
			<< "	--version	Prints the version of CPCC you are using\n"
			<< "	--libs		Prints available CPCC libraries\n"
			<< "	--nocredit	Compiles the file without the credits at the beginning\n"
			<< std::flush;
		
		if (argc < 2)
			return 1;
		else
			return 0;
	}
	
	long long timer = clock();

	bool credit = true;

	if (strncmp(argv[1], "--", 2) == 0)
		credit = handle_args(argv[1]+2);

	std::ifstream in(argv[argc-1]);
	if (!in.is_open()) {
		std::cerr << "Invalid file " << argv[argc-1] << " supplied!" << std::endl;
		return 1;
	}

	std::ofstream out("out.cpp");
	if (!out.is_open()) {
		std::cerr << "Could not open result file!" << std::endl;
		return 1;
	}

	std::deque<std::string> lines;
	std::string _line;
	while (std::getline(in, _line)) {
		lines.push_back(_line);
	}
	in.close();

	if (credit) ffprint(std::string(CPCC_INCLUDE_DIR) + std::string("crucial/credit"), &out);
	ffprint(std::string(CPCC_INCLUDE_DIR) + std::string("crucial/base"), &out);
	
	int end_imports = -1;
	for (int i = 0; i < lines.size(); i++) {
		const std::string &line = lines[i];
		if (line.size() == 0) {
			end_imports = i;
			break;
		}
		if (line.substr(0, 4) == "use ") {
			std::string path = std::string(CPCC_INCLUDE_DIR) + "utils/" + line.substr(4);
			path.pop_back(); // remove ;
			if (!std::filesystem::exists(path)) {
				std::cerr << line.substr(4) << " is not recognized as a valid library." << std::endl;
				std::cerr << "Check to see if you are running an up-to-date CPCC version." << std::endl;
				return 1;
			}
			ffprint(path, &out);
			continue;
		}
		if (end_imports == -1) {
			end_imports = i;
			break;
		}
	}

	for (int i = 0; i < end_imports; i++) {
		lines.pop_front();
	}

	out << '\n';

	std::string main_buf;
	main_buf.reserve(65536);
	for (int i = end_imports; i < lines.size(); i++) {
		out << lines[i] << '\n';
		main_buf += lines[i] + '\n';
	}

	Warnings::analyze(main_buf, lines);

	out.close();

	long long timer2 = clock();
	std::cout << "\033[1;32mSuccess!\033[0m Compiled in " << (timer2 - timer) / (double)CLOCKS_PER_SEC * 1000 << " ms." << std::endl;

	return 0;
}
