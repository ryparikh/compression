#include <map>
#include <iostream>

#include "huffmancompressor.h"

Result HuffmanCompressor::compress_byte_stream(const std::vector<std::byte>& uncompressedStream, /* out */ std::vector<std::byte>& compressedStream) noexcept
{
	std::map<std::byte, unsigned int> frequencyCount;
	for (const std::byte& byte : uncompressedStream)
	{
		frequencyCount[byte]++;
	}

	// make reversed vector to make use of pop_back/push_back for stack.
	std::vector<std::unique_ptr<Node>> orderedDistribution;
	Result result = order_frequency_count(frequencyCount, /* out */ orderedDistribution);
	if (result != Result::Ok)
	{
		return result;
	}

	std::unique_ptr<Node> spHuffmanTree;
	result = build_encoding_tree(orderedDistribution, /* out */ spHuffmanTree);
	if (result != Result::Ok)
	{
		return result;
	}

	std::map<std::byte, Encoding> encodingMap;
	result = build_encoding_map(spHuffmanTree.get(), encodingMap);
	if (result != Result::Ok)
	{
		return result;
	}

	result = compress(uncompressedStream, encodingMap, /* out */ compressedStream);
	if (result != Result::Ok)
	{
		return result;
	}

	return Result::Ok;
}

Result HuffmanCompressor::compress(const std::vector<std::byte>& uncompressedStream, const std::map<std::byte, Encoding>& encodingMap, /* out */ std::vector<std::byte>& compressedStream)
{
	int currentBitWriting = 7;
	std::byte compressedByte {0};
	for (const std::byte& nextByte : uncompressedStream)
	{
		auto iter = encodingMap.find(nextByte);
		if (iter == encodingMap.end())
		{
			return Result::Fail;
		}

		const Encoding& code = iter->second;
		int bitsWritten = 0;
		for(int i = code.m_encodingLength - 1; i >= 0; --i, --currentBitWriting)
		{
			if (currentBitWriting < 0)
			{
				currentBitWriting = 7;
				compressedStream.emplace_back(compressedByte);
				compressedByte = static_cast<std::byte>(0);
			}

			std::byte codeBit = ((code.m_encodedByte & static_cast<std::byte>(1 << i)) == static_cast<std::byte>(0)) ? static_cast<std::byte>(0) : static_cast<std::byte>(1);
			compressedByte |= (codeBit << currentBitWriting);
		}
	}

	compressedStream.emplace_back(compressedByte);
	return Result::Ok;
}

Result HuffmanCompressor::build_encoding_map(Node* pRoot, /* out */ std::map<std::byte, Encoding>& encodingMap)
{
	std::byte code {};
	return build_encoding_map_core(pRoot, true /* fZero */, 0 /* codeLength */, code, encodingMap);
}



Result HuffmanCompressor::build_encoding_map_core(Node* node, bool fZero, unsigned int codeLength, std::byte code, /* out */ std::map<std::byte, Encoding>& encodingMap)
{
	if (node == nullptr)
	{
		return Result::Ok;
	}

	code |= fZero ? static_cast<std::byte>(0) : static_cast<std::byte>(1);
	Result result = build_encoding_map_core(node->m_spLeft.get(), true /* fZero */, codeLength + 1, code << 1, encodingMap);
	if (result != Result::Ok)
	{
		return result;
	}

	result = build_encoding_map_core(node->m_spRight.get(), false /* fZero */, codeLength + 1, code << 1, encodingMap);
	if (result != Result::Ok)
	{
		return result;
	}

	if (node->m_spLeft == nullptr && node->m_spRight == nullptr)
	{
		std::byte plaintextByte = node->m_data.second;
		encodingMap.emplace(std::make_pair<std::byte, Encoding>(std::move(plaintextByte), {node->m_data.second, code, codeLength}));
	}

	return Result::Ok;
}


// This is an n^2 insertion sort
Result HuffmanCompressor::order_frequency_count(const std::map<std::byte, unsigned int> frequencyCount, /* out */ std::vector<std::unique_ptr<Node>>& orderedDistribution) noexcept
{
	try
	{
		orderedDistribution.reserve(frequencyCount.size() - orderedDistribution.capacity());
	
		for (const auto& insertFrequency : frequencyCount)
		{
			auto iter = orderedDistribution.begin();

			// Find bucket we are less than
			for(; iter != orderedDistribution.end() && insertFrequency.second < (*iter)->m_data.first ; ++iter);

			// If we are equal, find the letter we are less than
			for(; iter != orderedDistribution.end() && insertFrequency.second == (*iter)->m_data.first && (*iter)->m_data.second > insertFrequency.first ; ++iter);

			// Insert the pair
			if (iter == orderedDistribution.end())
			{
				orderedDistribution.emplace_back(std::make_unique<Node>(insertFrequency.second, insertFrequency.first));
			}
			else
			{
				orderedDistribution.insert(iter, std::make_unique<Node>(insertFrequency.second, insertFrequency.first));
			}
		}
	}
	catch (std::bad_alloc&)
	{
		return Result::OutOfMemory;
	}
	catch (...)
	{
		return Result::Fail;
	}

	return Result::Ok;
}

Result HuffmanCompressor::build_encoding_tree(std::vector<std::unique_ptr<Node>>& orderedDistribution, std::unique_ptr<Node>& spTreeRoot) noexcept
{
	try
	{
		while(orderedDistribution.size() > 1)
		{
			std::unique_ptr<Node> spNodeLeft(std::move(orderedDistribution.back()));
			orderedDistribution.pop_back();
			std::unique_ptr<Node> spNodeRight(std::move(orderedDistribution.back()));
			orderedDistribution.pop_back();

			std::unique_ptr<Node> spNewNode(std::make_unique<Node>(spNodeLeft->m_data.first + spNodeRight->m_data.first, std::move(spNodeLeft), std::move(spNodeRight)));
			auto iter = orderedDistribution.begin();
			for (; iter != orderedDistribution.end() && (*iter)->m_data.first > spNewNode->m_data.first; ++iter);
			for (; iter != orderedDistribution.end() && (*iter)->m_data.first == spNewNode->m_data.first; ++iter);
			if (iter == orderedDistribution.end())
			{
				orderedDistribution.emplace_back(std::move(spNewNode));
			}
			else
			{
				orderedDistribution.insert(iter, std::move(spNewNode));
			}
		}

		spTreeRoot.reset(orderedDistribution[0].release());
		orderedDistribution.clear();
	}
	catch (std::bad_alloc&)
	{
		return Result::OutOfMemory;
	}
	catch (...)
	{
		return Result::Fail;
	}
	return Result::Ok;
}
