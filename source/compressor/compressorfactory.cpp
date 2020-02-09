#include "compressorfactory.h"
#include "huffmancompressor.h"

/* static */ std::unique_ptr<ICompressorFactory> ICompressorFactory::create_unique() noexcept
{
	try
	{
		std::unique_ptr<ICompressorFactory> spFactory(std::make_unique<CompressorFactory>());
		return spFactory;
	}
	catch(...)
	{
		return nullptr;
	}
}

std::unique_ptr<ICompressor> CompressorFactory::make_unique_compressor(CompressionType compressionType) noexcept
{
	std::unique_ptr<ICompressor> spCompressor;
	switch (compressionType)
	{
		case Huffman:
			{
				spCompressor = std::make_unique<HuffmanCompressor>();
			}
			break;
		default:
			break;
	}

	return spCompressor;
}

