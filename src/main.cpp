#include <ctime>
#include <deque>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <string>

#include <boost/program_options.hpp>

#include "warn.hpp"

#ifndef CPCC_VER // Should only happen if compiled improperly
#define CPCC_VER "ERR"
#define CPCC_INCLUDE_DIR "ERR"
#endif

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

int main(int argc, char *argv[]) {
	boost::program_options::options_description desc("CPCC usage");
	desc.add_options()
		("help", "Prints this message")
		("version", "Prints the version of CPCC you are using")
		("libs", "Prints available CPCC libraries")
		("nocredit", "Compiles the file without the credits at the beginning")
		("Wnone", "Disables all warnings")
		("o", boost::program_options::value<std::string>(), "The output file (out.cpp by default)")
		("file", boost::program_options::value<std::string>(), "The file to compile (using --file is optional)");

	boost::program_options::positional_options_description p;
	p.add("file", -1);

	boost::program_options::variables_map vm;
	boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
	boost::program_options::notify(vm);

	if (vm.count("help") || argc < 2) {
		std::cout << desc << std::endl;
		return argc < 2;
	}
	if (vm.count("version")) {
		std::cout << "CPCC version " << CPCC_VER << " with include directory " << CPCC_INCLUDE_DIR << std::endl;
		return 0;
	}
	if (vm.count("libs")) {
		std::cout << "Available libraries: ";
		for (const auto &entry : std::filesystem::directory_iterator(std::string(CPCC_INCLUDE_DIR) + "utils")) {
			std::string file = entry.path().filename().string();
			if (file[0] == '.') continue;
			std::cout << file << ' ';
		}
		std::cout << std::endl;
		return 0;
	}

	std::string outfile = "out.cpp";
	if (vm.count("o")) {
		outfile = vm["o"].as<std::string>();
	}

	bool credit = true;
	if (vm.count("nocredit")) {
		credit = false;
	}

	std::string infile = vm["file"].as<std::string>();
	
	long long timer = clock();

	std::ifstream in(infile);
	if (!in.is_open()) {
		std::cerr << "Invalid file " << infile << " supplied!" << std::endl;
		return 1;
	}

	std::ofstream out(outfile);
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
			out << '\n';
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

	while (lines.front().size() == 0) {
		lines.pop_front();
	}

	std::string main_buf;
	main_buf.reserve(65536);
	for (int i = 0; i < lines.size(); i++) {
		out << lines[i] << '\n';
		main_buf += lines[i] + '\n';
	}

	if (!vm.count("Wnone"))
		Warnings::analyze(main_buf, lines);

	out.close();

	std::cout << "\033[1;32mSuccess!\033[0m Compiled in " << (clock() - timer) / (double)CLOCKS_PER_SEC * 1000 << " ms." << std::endl;

	return 0;
}
