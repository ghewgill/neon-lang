#include <vector>

#include <bzlib.h>
#include <lzma.h>
#include <zlib.h>

#include "neonext.h"

const Ne_MethodTable *Ne;

extern "C" {

Ne_EXPORT int Ne_INIT(const Ne_MethodTable *methodtable)
{
    Ne = methodtable;
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_gzip)
{
    Ne_Bytes data = Ne_PARAM_BYTES(0);

    std::vector<unsigned char> buf;
    uLong destLen = compressBound(static_cast<uLong>(data.len));
    buf.resize(destLen);
    int r = ::compress(reinterpret_cast<Bytef *>(const_cast<unsigned char *>(buf.data())), &destLen, reinterpret_cast<Bytef *>(const_cast<unsigned char *>(data.ptr)), static_cast<uLong>(data.len));
    if (r != Z_OK) {
        Ne_RAISE_EXCEPTION("CompressException", "gzip", r);
    }
    Ne_RETURN_BYTES(buf.data(), destLen);
}

Ne_FUNC(Ne_gunzip)
{
    Ne_Bytes data = Ne_PARAM_BYTES(0);

    std::vector<unsigned char> buf;
    uLong destLen = 12 * static_cast<uLong>(data.len);
    int r;
    for (;;) {
        buf.resize(destLen);
        r = uncompress(reinterpret_cast<Bytef *>(const_cast<unsigned char *>(buf.data())), &destLen, reinterpret_cast<Bytef *>(const_cast<unsigned char *>(data.ptr)), static_cast<uLong>(data.len));
        if (r != Z_BUF_ERROR) {
            break;
        }
        destLen *= 2;
    }
    if (r != Z_OK) {
        Ne_RAISE_EXCEPTION("CompressException", "gunzip", r);
    }
    Ne_RETURN_BYTES(buf.data(), destLen);
}

Ne_FUNC(Ne_bzip2)
{
    Ne_Bytes data = Ne_PARAM_BYTES(0);

    std::vector<unsigned char> buf;
    unsigned int destLen = static_cast<unsigned int>(data.len + data.len/100 + 600);
    buf.resize(destLen);
    int r = BZ2_bzBuffToBuffCompress(reinterpret_cast<char *>(const_cast<unsigned char *>(buf.data())), &destLen, reinterpret_cast<char *>(const_cast<unsigned char *>(data.ptr)), static_cast<unsigned int>(data.len), 5, 0, 30);
    if (r != BZ_OK) {
        Ne_RAISE_EXCEPTION("CompressException", "bzip2", r);
    }
    Ne_RETURN_BYTES(buf.data(), destLen);
}

Ne_FUNC(Ne_bunzip2)
{
    Ne_Bytes data = Ne_PARAM_BYTES(0);

    std::vector<unsigned char> buf;
    unsigned int destLen = 20 * static_cast<unsigned int>(data.len);
    int r;
    for (;;) {
        buf.resize(destLen);
        r = BZ2_bzBuffToBuffDecompress(reinterpret_cast<char *>(const_cast<unsigned char *>(buf.data())), &destLen, reinterpret_cast<char *>(const_cast<unsigned char *>(data.ptr)), static_cast<unsigned int>(data.len), 0, 0);
        if (r != BZ_OUTBUFF_FULL) {
            break;
        }
        destLen *= 2;
    }
    if (r != BZ_OK) {
        Ne_RAISE_EXCEPTION("CompressException", "bunzip2", r);
    }
    Ne_RETURN_BYTES(buf.data(), destLen);
}

Ne_FUNC(Ne_lzma)
{
    Ne_Bytes data = Ne_PARAM_BYTES(0);

    std::vector<unsigned char> buf;
    size_t destLen = data.len + 1000;
    buf.resize(destLen);
    size_t destPos = 0;
    int r = lzma_easy_buffer_encode(LZMA_PRESET_EXTREME, LZMA_CHECK_CRC64, NULL, reinterpret_cast<const uint8_t *>(data.ptr), data.len, reinterpret_cast<uint8_t *>(const_cast<unsigned char *>(buf.data())), &destPos, destLen);
    if (r != LZMA_OK) {
        Ne_RAISE_EXCEPTION("CompressException", "lzma", r);
    }
    Ne_RETURN_BYTES(buf.data(), static_cast<int>(destLen));
}

Ne_FUNC(Ne_unlzma)
{
    Ne_Bytes data = Ne_PARAM_BYTES(0);

    std::vector<unsigned char> buf;
    size_t destLen = 20 * data.len;
    int r;
    size_t outPos;
    for (;;) {
        buf.resize(destLen);
        uint64_t memlimit = UINT64_MAX;
        size_t inPos = 0;
        outPos = 0;
        r = lzma_stream_buffer_decode(&memlimit, 0, NULL, reinterpret_cast<const uint8_t *>(data.ptr), &inPos, data.len, reinterpret_cast<uint8_t *>(const_cast<unsigned char *>(buf.data())), &outPos, destLen);
        if (r != LZMA_BUF_ERROR) {
            break;
        }
        destLen *= 2;
    }
    if (r != LZMA_OK) {
        Ne_RAISE_EXCEPTION("CompressException", "unlzma", r);
    }
    Ne_RETURN_BYTES(buf.data(), static_cast<int>(outPos));
}

} // extern "C"
