#pragma once
#include <span>
#include <concepts>
#include <vector>
#include <array>
#include <type_traits>
#include <mutex>

// Concept to validate types
template <typename T>
concept valid_type = std::same_as<T, char> ||
std::same_as<T, unsigned char> ||
std::same_as<T, std::byte>;

template <typename T>
concept valid_container = requires(T container) {
    { container.data() } -> std::convertible_to<void*>;
    { container.size() } -> std::convertible_to<std::size_t>;
}&& valid_type<typename T::value_type>;

class Span_Factory {
public:
    // Operator() for containers like std::vector, std::array
    template <typename T>
    std::span<std::byte> operator()(T& container) requires valid_container<T> {
        std::lock_guard<std::mutex> lock(containerMtx_);
        return std::span<std::byte>(
            reinterpret_cast<std::byte*>(container.data()),
            container.size()
        );
    }

    // Operator() for raw arrays
    template <typename T, std::size_t N>
    std::span<std::byte> operator()(T(&array)[N]) requires valid_type<T> {
        std::lock_guard<std::mutex> lock(rawMtx_);
        return std::span<std::byte>(
            reinterpret_cast<std::byte*>(array),
            N
        );
    }
private:
    std::mutex containerMtx_;
    std::mutex rawMtx_;
};
