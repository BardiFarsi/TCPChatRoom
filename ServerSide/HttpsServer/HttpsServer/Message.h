#pragma once
#include "LOGGER.h"
#include "Span_Factory.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <array>
#include <span>
#include <iostream>
#include <ranges>
#include <type_traits>
#include <cstdint>

template <typename T>
concept validMessageType =
std::same_as<T, std::string> ||
std::same_as<T, std::string_view>;

template <typename T>
concept validContainers =
std::ranges::range<T> &&
(std::same_as<std::ranges::range_value_t<T>, char> ||
	std::same_as<std::ranges::range_value_t<T>, unsigned char> ||
	std::same_as<std::ranges::range_value_t<T>, signed char>);

template <typename T>
concept Message = validMessageType<T> || validContainers<T>;

template<typename T, typename = void>
struct has_data_method : std::false_type {};

template<typename T>
struct has_data_method<T, std::void_t<decltype(std::declval<T>().data())>> : std::true_type {};

template<typename T, typename = void>
struct has_size_method : std::false_type {};

template<typename T>
struct has_size_method<T, std::void_t<decltype(std::declval<T>().size())>> : std::true_type {};
