#include <iostream>
#include <Compressor/ICompressor.h>
#include <Compressor/ICompressorFactory.h>

int main()
{
    try
    {
		std::cout << "Starting to compress file..." << std::endl;

		std::unique_ptr<ICompressorFactory> spFactory(ICompressorFactory::create_unique());
		if (spFactory == nullptr)
		{
			return -1;
		}

		std::unique_ptr<ICompressor> spCompressor(spFactory->make_unique_compressor(CompressionType::Huffman));
		if (spCompressor == nullptr)
		{
			return -2;
		}
		std::vector<std::byte> uncompressedStream;
		std::string input = "I am really in love with Katie Cameron. She is the greatest person someone could ask for.  She's the best.";
		for (const char& letter : input)
		{
			uncompressedStream.push_back(static_cast<std::byte>(letter));
		}

		std::vector<std::byte> compressedStream;
		std::chrono::steady_clock::time_point fm_start = std::chrono::steady_clock::now();
		spCompressor->compress_byte_stream(uncompressedStream, /* out */ compressedStream);
		std::chrono::steady_clock::time_point fm_end = std::chrono::steady_clock::now();

		std::cout << "File compression finished." << std::endl;
		std::cout << "Uncompressed size: " << uncompressedStream.size() << std::endl;
		std::cout << "Compressed size: " << compressedStream.size() << std::endl;
		std::cout << "Compression took: "
              << std::chrono::duration_cast<std::chrono::nanoseconds>(fm_end - fm_start).count()
              << "ns.\n";

		std::cout << "Uncompressed stream: ";
		for (const std::byte& uncompressedByte : uncompressedStream)
		{
			std::cout << static_cast<char>(uncompressedByte);
		}
		std::cout << std::endl;

		for (const std::byte& compressedByte : compressedStream)
		{
			for (int i = 7; i >= 0; --i)
			{
				if ((compressedByte & static_cast<std::byte>(1 << i)) == static_cast<std::byte>(0))
				{
					std::cout << "0";
				}
				else
				{
					std::cout << "1";
				}
			}
		}
		std::cout << std::endl;

    }
    catch(...)
    {
    }
}
