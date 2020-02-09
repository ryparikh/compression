#ifndef RESULT_H 
#define RESULT_H

#pragma once

#include <cstdint>

enum class Result : std::uint64_t
{
	Fail,
	Ok,
	OutOfMemory,
};

#endif /* RESULT_H */
