#pragma once
#pragma once
#include<iostream>
#include <string>
#include <cctype>

constexpr size_t BUFFER_SIZE = 1024;
//buffer sanitizer

class Buffer_Sanitizer {
public:
	std::string operator() (const std::vector<char>& buff) const {
		std::string result;
		if (buff.size() > 0) {
			for (size_t i = 0; i < BUFFER_SIZE; ++i) {
				char ch = buff[i];
				if (std::isprint(static_cast<unsigned char>(ch))) {
					result += ch;
				}
			}
		}
		return result;
	}
};