#include "warn.hpp"

void Warnings::analyze(std::string &code, std::deque<std::string> &lines) {
	// check for missing fastio
	if (code.find("fastio();") == std::string::npos && (code.find("sync_with_stdio") == std::string::npos
		|| code.find("cin.tie") == std::string::npos)) {
		std::cout << "\033[1;33mWarning: \033[0m" << "Fast input/output is not being used. "
			<< "Or, you are running only one of `sync_with_stdio` or `cin.tie`. "
			<< "Ignore this warning if you are using `scanf` and `printf`. " << std::endl;
	}
	// check for endls
	if (code.find("endl") != std::string::npos) {
		std::cout << "\033[1;33mWarning: \033[0m" << "`endl` is slow, and should be avoided in favor of `\\n`. "
			<< "Ignore this warning if you are solving an interactive problem." << std::endl;
	}
	// check for mixing and matching of cin/cout and scanf/printf
	if (code.find("cin") != std::string::npos && code.find("scanf") != std::string::npos) {
		std::cout << "\033[1;33mWarning: \033[0m" << "Mixing C++-style IO (e.g. `cin`) and C-style IO (e.g. `scanf`) "
			<< "is not recommended. Ignore this warning if you are solving an interactive problem." << std::endl;
	}
	// check for use of `std::`
	if (code.find("std::") != std::string::npos) {
		std::cout << "\033[1;33mWarning: \033[0m" << "Using `std::` is not recommended, as `using namespace std;` "
			<< "is already included." << std::endl;
	}
	// check for use of unordered set/map
	if (code.find("unordered_set") != std::string::npos || code.find("unordered_map") != std::string::npos) {
		std::cout << "\033[1;33mWarning: \033[0m" << "Using `unordered_set` or `unordered_map` is not recommended, "
			<< "as the worst case time complexity can be O(n). Instead, use `set` or `map`." << std::endl;
	}

	int depth = 0;
	std::vector<std::vector<std::string>> functions;
	std::vector<std::string> tmp;
	for (int i = 0; i < lines.size(); i++) {
		if (lines[i].size() == 0) continue;
		// check if we are in a function
		bool flag = false;
		if (lines[i].find('{') != std::string::npos) {
			depth++;
			flag = true;
		}
		if (lines[i].find('}') != std::string::npos) { // mega scuffed	
			depth--;
			flag = true;
		}
		if (depth) tmp.push_back(lines[i]);
		if (flag) {
			if (!depth && !tmp.empty()) {
				functions.push_back(tmp);
				tmp.clear();
			}
		}
	}

	// for (const auto &v : functions) {
	// 	for (const auto &l : v) std::cout << l << '\n';
	// }

	std::cout << "Time complexity analysis (BETA): " << std::endl;

	for (int i = 0; i < functions.size(); i++) {
		std::string name = functions[i][0];
		int exp_n = 0;
		int depth = 0;
		std::set<int> fors;
		for (int j = 0; j < functions[i].size(); j++) {
			if (functions[i][j].size() == 0) continue;
			if (functions[i][j].find('{') != std::string::npos &&
				(functions[i][j].find("for") != std::string::npos || functions[i][j].find("while") != std::string::npos)) {
				depth++;
				fors.insert(depth);

				continue;
			}
			if (functions[i][j].find('}') != std::string::npos) {
				depth--;
				if (fors.count(depth + 1)) {
					exp_n = std::max(exp_n, (int)fors.size());
					fors.erase(depth + 1);
				}
			}
		}
		if (exp_n == 0) std::cout << name << " O(1)" << std::endl;
		else if (exp_n == 1) std::cout << name << " O(n)" << std::endl;
		else std::cout << name << " O(n^" << exp_n  << ")" << std::endl;
	}
}
