#ifndef ICOMPRESSORFACTORY_H
#define ICOMPRESSORFACTORY_H

#pragma once

#include <memory>

#include <Compressor/CompressionTypes.h>

struct ICompressor;

struct ICompressorFactory
{
	virtual ~ICompressorFactory() noexcept = default;

	static std::unique_ptr<ICompressorFactory> create_unique() noexcept;
	virtual std::unique_ptr<ICompressor> make_unique_compressor(CompressionType compressionType) noexcept = 0;
};

#endif /* ICOMPRESSORFACTORY_H */
