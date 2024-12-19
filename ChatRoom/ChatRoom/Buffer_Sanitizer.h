#pragma once
#pragma once
#include<iostream>
#include <string>
#include <cctype>
#include <vector>
#include <boost/asio.hpp>
#include <boost/container/vector.hpp>

constexpr size_t BUFFER_SIZE = 1024;
//buffer sanitizer

class Buffer_Sanitizer 
	: public boost::asio::mutable_buffer,  
	  public boost::container::vector<char> {
public:

	Buffer_Sanitizer(void* data, size_t size) 
		: boost::asio::mutable_buffer(data, size), 
		  boost::container::vector<char>(static_cast<char*>(data), static_cast<char*>(data) + size) {}

	std::string operator() (boost::asio::mutable_buffer& mtBuffer) const {
		std::string result;
		char* buff = static_cast<char*>(mtBuffer.data());
		size_t length = mtBuffer.size();
		for (size_t i = 0; i < length; i ++){
			char ch = buff[i]; 
			if (std::isprint(static_cast<unsigned char>(ch))) {
				result += ch;
			}
		}	
		return result;
	}
};