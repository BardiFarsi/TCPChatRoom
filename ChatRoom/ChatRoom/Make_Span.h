#pragma once
#include <span>
#include <concepts>
#include <vector>
#include <array>
#include <type_traits>

template<typename T>
concept valid_type = std::same_as<T, char> ||
					  std::same_as<T, unsigned char> ||
					  std::same_as<T, std::byte>;

template <typename Container>
class Make_Span
{
	// Handles SFINAE to determine the element type for both containers and raw arrays.
	using contType = typename std::remove_reference_t<Container>::value_type;

public:
	explicit Make_Span(Container& container) requires valid_type<contType>
		: span_(reinterpret_cast<std::byte*>(container.data()), container.size()) {}

	template<size_t N>
	explicit Make_Span(contType (&buffer)[N]) requires valid_type<contType>
		: span_(reinterpret_cast<std::byte*>(buffer), N) {}

	~Make_Span() = default;

	operator std::span<std::byte>() const {
		return span_; 
	}

private:
	std::span<std::byte> span_; 
};

