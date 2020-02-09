#ifndef ICOMPRESSOR_H
#define ICOMPRESSOR_H

#pragma once

#include <cstddef>
#include <vector>

#include <Core/Result.h>

struct ICompressor
{
	virtual ~ICompressor() noexcept = default;
	virtual Result compress_byte_stream(const std::vector<std::byte>& uncompressedStream, /* out */ std::vector<std::byte>& compressedStream) noexcept = 0;
};

#endif /* ICOMPRESSOR_H */

