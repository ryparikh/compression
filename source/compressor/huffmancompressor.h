#include <map>

#include <Compressor/ICompressor.h>

class HuffmanCompressor final : public ICompressor
{
	using Data = std::pair<unsigned int, std::byte>;

	struct Node final
	{
		Node() = default;
		Node(unsigned int frequency, std::byte _byte)
		{
			m_data.first = frequency;
			m_data.second = _byte;
		}

		Node(unsigned int frequency, std::unique_ptr<Node>&& spLeft, std::unique_ptr<Node>&& spRight) :
			m_spLeft(std::move(spLeft)),
			m_spRight(std::move(spRight))
		{
			m_data.first = frequency;
		}

		Data m_data {};
		std::unique_ptr<Node> m_spLeft {};
		std::unique_ptr<Node> m_spRight {};
	};

	struct Encoding final
	{
		Encoding(std::byte byte, std::byte encodedByte, unsigned int encodingLength) :
			m_byte(byte),
			m_encodedByte(encodedByte),
			m_encodingLength(encodingLength)
		{}

		std::byte m_byte {};
		std::byte m_encodedByte {};
		unsigned int m_encodingLength {};
	};

public:
	Result compress_byte_stream(const std::vector<std::byte>& uncompressedStream, /* out */ std::vector<std::byte>& compressedStream) noexcept override;

private:
	Result compress(const std::vector<std::byte>& uncompressedStream, const std::map<std::byte, Encoding>& encodingMap, /* out */ std::vector<std::byte>& compressedStream);
	Result order_frequency_count(const std::map<std::byte, unsigned int> frequencyCount, /* out */ std::vector<std::unique_ptr<Node>>& orderedDistribution) noexcept;
	Result build_encoding_tree(std::vector<std::unique_ptr<Node>>& orderedDistribution, /* out */ std::unique_ptr<Node>& pTreeRoot) noexcept;
	Result build_encoding_map(Node* pRoot, /* out */ std::map<std::byte, Encoding>& encodingMap);
	Result build_encoding_map_core(Node* node, bool fZero, unsigned int codeLength, std::byte code, /* out */ std::map<std::byte, Encoding>& encodingMap);
};
