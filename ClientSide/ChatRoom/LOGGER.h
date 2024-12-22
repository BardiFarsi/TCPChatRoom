#ifndef LOGGER_H
#define LOGGER_H

#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include<mutex>
#include <type_traits>

using std::cout;
using std::cin;
using std::endl;

class LOGGER {
public:
    LOGGER() = default;
    ~LOGGER() = default;

    // Template structure to check if a type T is printable by checking if it can be inserted into an output stream.
    template<typename T, typename = void>
    struct Is_Printable : std::false_type {};
    // Specialization of Is_Printable for types that can be output to std::ostream.
    template<typename T>
    struct Is_Printable<T, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<T>())>> : std::true_type {};
    // For Little-Endian structure 
    template<>
    struct Is_Printable<uint8_t, void> : std::true_type {};
    // Inline constant expression to determine if a type T is printable.
    template<typename T>
    inline static constexpr bool isPrintable = Is_Printable<T>::value;
    // Variadic template function to log a series of arguments.
    // Args is a pack of types that can vary depending on the call to log().
    template<typename... Args>
    void log(const Args&... args) {
        std::lock_guard<std::mutex> lock(mtx_);
        ss_.str("");
        ss_.clear();

        auto argsHandler = [this](const auto& arg) {

            using ArgType = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<ArgType, bool>) {
                ss_ << (arg ? "true" : "false");
            }
            else if constexpr (std::is_same_v<ArgType, uint8_t>) {
                ss_ << static_cast<int>(arg);
            }
            else if constexpr (isPrintable<ArgType>) {
                ss_ << arg;
            }
            else {
                ss_ << "[Non - printable type] The memory address of the object is: " << &arg;
            }
            ss_ << ""; // In case if we need space
        };

        (argsHandler(args), ...);

        std::string logMessage = ss_.str();
        if (!logMessage.empty()) {
            cout << logMessage << endl;
        }
        ss_.clear();
    }

private:
    std::mutex mtx_;
    std::stringstream ss_;
};

extern LOGGER console;

#endif
