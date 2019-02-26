#pragma once

#include "Utils.h"


#include "spdlog/spdlog.h"

#include <type_traits>
#include <string_view>
#include <chrono>

#define CCALL(funcname) #funcname, funcname



template<typename T, typename... Args>
void WrapStatCostTimeVoid(std::string_view funcname, T& func, Args... args)
{
	//static_cast(std::is_function<std::remove_pointer<T>>::value);
	auto starttimestamp = std::chrono::high_resolution_clock::now();
	func(args...);
	auto endtimestamp = std::chrono::high_resolution_clock::now();
	auto costtime = endtimestamp - starttimestamp;
	LOG_ALQAZ->info("function {} cost {} nanoseconds", funcname, costtime.count());
}

template<typename Ret, typename T, typename... Args>
Ret WrapStatCostTime(std::string_view funcname, T& func, Args... args)
{
	Ret ret;
	//static_cast(std::is_function<T>::value);
	auto starttimestamp = std::chrono::high_resolution_clock::now();
	ret = func(args...);
	auto endtimestamp = std::chrono::high_resolution_clock::now();
	auto costtime = endtimestamp - starttimestamp;
	LOG_ALQAZ->info("function {} cost {} nanoseconds", funcname, costtime.count());
	return ret;
}

template<typename T, class U, typename... Args>
T WrapStatCostTime(std::string_view funcname, T (U::*func)(Args...), U* px, Args... args)
{
	T ret;
	auto starttimestamp = std::chrono::high_resolution_clock::now();
	ret = (px->*func)(args...);
	auto endtimestamp = std::chrono::high_resolution_clock::now();
	auto costtime = endtimestamp - starttimestamp;
	LOG_ALQAZ->info("function {} cost {} nanoseconds", funcname, costtime.count());
	return ret;
}

template<typename U, typename... Args>
void WrapStatCostTimeVoid(std::string_view funcname, void (U::*func)(Args...), U* px, Args... args)
{
	auto starttimestamp = std::chrono::high_resolution_clock::now();
	(px->*func)(args...);
	auto endtimestamp = std::chrono::high_resolution_clock::now();
	auto costtime = endtimestamp - starttimestamp;
	LOG_ALQAZ->info("function {} cost {} nanoseconds", funcname, costtime.count());
}