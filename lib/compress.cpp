#include <string>

#include <bzlib.h>
#include <lzma.h>
#include <zlib.h>

namespace rtl {

namespace compress {

std::string gzip(const std::string &input)
{
    std::string buf;
    uLong destLen = compressBound(static_cast<uLong>(input.length()));
    buf.resize(destLen);
    int r = ::compress(reinterpret_cast<Bytef *>(const_cast<char *>(buf.data())), &destLen, reinterpret_cast<Bytef *>(const_cast<char *>(input.data())), static_cast<uLong>(input.length()));
    if (r != Z_OK) {
        fprintf(stderr, "gzip r %d\n", r);
        return std::string(); // TODO: exception
    }
    buf.resize(destLen);
    return buf;
}

std::string gunzip(const std::string &input)
{
    std::string buf;
    uLong destLen = 12 * static_cast<uLong>(input.length());
    int r;
    for (;;) {
        buf.resize(destLen);
        r = uncompress(reinterpret_cast<Bytef *>(const_cast<char *>(buf.data())), &destLen, reinterpret_cast<Bytef *>(const_cast<char *>(input.data())), static_cast<uLong>(input.length()));
        if (r != Z_BUF_ERROR) {
            break;
        }
        destLen *= 2;
    }
    if (r != Z_OK) {
        fprintf(stderr, "gunzip r %d\n", r);
        return std::string(); // TODO: exception
    }
    buf.resize(destLen);
    return buf;
}

std::string bzip2(const std::string &input)
{
    std::string buf;
    unsigned int destLen = static_cast<unsigned int>(input.length() + input.length()/100 + 600);
    buf.resize(destLen);
    int r = BZ2_bzBuffToBuffCompress(const_cast<char *>(buf.data()), &destLen, const_cast<char *>(input.data()), static_cast<unsigned int>(input.length()), 5, 0, 30);
    if (r != BZ_OK) {
        fprintf(stderr, "bzip2 r %d\n", r);
        return std::string(); // TODO: exception
    }
    buf.resize(destLen);
    return buf;
}

std::string bunzip2(const std::string &input)
{
    std::string buf;
    unsigned int destLen = 20 * static_cast<unsigned int>(input.length());
    int r;
    for (;;) {
        buf.resize(destLen);
        r = BZ2_bzBuffToBuffDecompress(const_cast<char *>(buf.data()), &destLen, const_cast<char *>(input.data()), static_cast<unsigned int>(input.length()), 0, 0);
        if (r != BZ_OUTBUFF_FULL) {
            break;
        }
        destLen *= 2;
    }
    if (r != BZ_OK) {
        fprintf(stderr, "bunzip2 r %d\n", r);
        return std::string(); // TODO: exception
    }
    buf.resize(destLen);
    return buf;
}

std::string lzma(const std::string &input)
{
    std::string buf;
    size_t destLen = input.length() + 1000;
    buf.resize(destLen);
    size_t destPos = 0;
    int r = lzma_easy_buffer_encode(LZMA_PRESET_EXTREME, LZMA_CHECK_CRC64, NULL, reinterpret_cast<const uint8_t *>(input.data()), input.length(), reinterpret_cast<uint8_t *>(const_cast<char *>(buf.data())), &destPos, destLen);
    if (r != LZMA_OK) {
        fprintf(stderr, "lzma r %d\n", r);
        return std::string(); // TODO: exception
    }
    buf.resize(destPos);
    return buf;
}

std::string unlzma(const std::string &input)
{
    std::string buf;
    size_t destLen = 20 * input.length();
    int r;
    size_t outPos;
    for (;;) {
        buf.resize(destLen);
        uint64_t memlimit = UINT64_MAX;
        size_t inPos = 0;
        outPos = 0;
        r = lzma_stream_buffer_decode(&memlimit, 0, NULL, reinterpret_cast<const uint8_t *>(input.data()), &inPos, input.length(), reinterpret_cast<uint8_t *>(const_cast<char *>(buf.data())), &outPos, destLen);
        if (r != LZMA_BUF_ERROR) {
            break;
        }
        destLen *= 2;
    }
    if (r != LZMA_OK) {
        fprintf(stderr, "unlzma r %d\n", r);
        return std::string(); // TODO: exception
    }
    buf.resize(outPos);
    return buf;
}

} // namespace compress

} // namespace rtl
