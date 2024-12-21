#pragma once
#include <iostream>
#include <string>
#include <cctype>
#include <span>
#include <vector>
#include <boost/asio.hpp>

constexpr size_t BUFFER_SIZE = 1024;

class Buffer_Sanitizer {
public:
    template <typename T>
    std::string operator()(const T& container) const {
        // Special handling for boost::asio::mutable_buffer
        if constexpr (std::is_same_v<T, boost::asio::mutable_buffer>) {
            return sanitize_buffer(container);
        }
        // Handle general container types with 'data()' and 'size()' methods
        else {
            static_assert(
                has_data_method<T>::value,
                "Type must have a 'data()' method returning a pointer");

            static_assert(
                has_size_method<T>::value,
                "Type must have a 'size()' method returning a size");

            const char* buffer = reinterpret_cast<const char*>(container.data());
            size_t length = container.size();

            return sanitize_buffer(buffer, length);
        }
    }

private:
    // Function to sanitize buffers for general containers like std::vector, std::span, etc.
    std::string sanitize_buffer(const char* buffer, size_t length) const {
        std::string result;
        for (size_t i = 0; i < length; i++) {
            char ch = buffer[i];
            if (std::isprint(static_cast<unsigned char>(ch))) {
                result += ch;
            }
        }
        return result;
    }

    // Function to sanitize boost::asio::mutable_buffer specifically
    std::string sanitize_buffer(const boost::asio::mutable_buffer& buffer) const {
        const char* data = static_cast<const char*>(buffer.data());
        size_t length = buffer.size();

        return sanitize_buffer(data, length);
    }

    // SFINAE technique to check if the type has 'data()' and 'size()' methods.
    // std::true_type is used if the method exists, std::false_type if not.
    template <typename T, typename = void>
    struct has_data_method : std::false_type {};

    template <typename T>
    struct has_data_method<T, std::void_t<decltype(std::declval<T>().data())>> : std::true_type {};

    template <typename T, typename = void>
    struct has_size_method : std::false_type {};

    template <typename T>
    struct has_size_method<T, std::void_t<decltype(std::declval<T>().size())>> : std::true_type {};
};
