#include <dirent.h>
#include <fstream>
#include <iostream>
#include <string>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <vector>

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
	bool credit = 1;
	if (strcmp(arg, "version") == 0) {
		std::cout << "CPCC version " << CPCC_VER << std::endl;
		exit(0);
	}
	else if (strcmp(arg, "libs") == 0) {
		DIR *d = opendir((std::string(CPCC_INCLUDE_DIR) + "utils").c_str());
		struct dirent *dir;
		if (d == NULL) {
			std::cerr << "Could not find library directory!" << std::endl;
			exit(1);
		}
		std::cout << "Available libraries: ";
		while ((dir = readdir(d)) != NULL) {
			if (strncmp(dir->d_name, ".", 1)) // hide hidden files
				std::cout << dir->d_name << ' ';
		}
		closedir(d);
		std::cout << std::endl;
		exit(0);
	}
	else if (strcmp(arg, "nocredit") == 0) {
		credit = 0;
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

	bool credit = 1;

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

	std::vector<std::string> lines;
	lines.reserve(1024);
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
		if (line.size() == 0) continue;
		if (line.substr(0, 4) == "use ") {
			std::string path = std::string(CPCC_INCLUDE_DIR) + "utils/" + line.substr(4);
			path.pop_back(); // remove ;
			if (access(path.c_str(), F_OK) == -1) {
				std::cerr << line.substr(4) << " is not recognized as a valid library." << std::endl;
				std::cerr << "Check to see if you are running an up-to-date CPCC version." << std::endl;
				return 1;
			}
			ffprint(path.c_str(), &out);
			continue;
		}
		if (end_imports == -1) {
			end_imports = i;
		}
	}

	out << '\n';

	std::string main_buf;
	main_buf.reserve(65536);

	for (int i = end_imports; i < lines.size(); i++) {
		out << lines[i] << '\n';
		main_buf += lines[i] + '\n';
	}

	// check for missing fastio
	if (main_buf.find("fastio();") == std::string::npos && (main_buf.find("sync_with_stdio") == std::string::npos
		|| main_buf.find("cin.tie") == std::string::npos)) {
		std::cout << "\033[1;33mWarning: \033[0m" << "Fast input/output is not being used. "
			<< "Or, you are running only one of `sync_with_stdio` or `cin.tie`. "
			<< "Ignore this warning if you are using `scanf` and `printf`. " << std::endl;
	}
	// check for endls
	if (main_buf.find("endl") != std::string::npos) {
		std::cout << "\033[1;33mWarning: \033[0m" << "`endl` is slow, and should be avoided in favor of `\\n`. "
			<< "Ignore this warning if you are solving an interactive problem." << std::endl;
	}

	out.close();

	long long timer2 = clock();
	std::cout << "\033[1;32mSuccess!\033[0m Compiled in " << (timer2 - timer) / (double)CLOCKS_PER_SEC * 1000 << " ms." << std::endl;

	return 0;
}
