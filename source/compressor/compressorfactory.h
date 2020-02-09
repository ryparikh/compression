#include <ICompressorFactory.h>

class CompressorFactory final : public ICompressorFactory
{
public:
	virtual std::unique_ptr<ICompressor> make_unique_compressor(CompressionType compressionType) noexcept override;
};

