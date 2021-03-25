#pragma once

#include <compact.h>
#include <iconv.h> //for gbk/big5/utf8
#include <string>
#include <cstring>

namespace husky {
    HUSKY_INLINE int
    code_convert(char *from_charset, char *to_charset, char *inbuf, size_t inlen, char *outbuf, size_t outlen) {
        iconv_t cd;
        char **pin = &inbuf;
        char **pout = &outbuf;
        cd = iconv_open(to_charset, from_charset);
        if (cd == 0)
            return -1;
        memset(outbuf, 0, outlen);
        if (iconv(cd, pin, &inlen, pout, &outlen) == ULONG_LONG_MAX)
            return -1;
        iconv_close(cd);
        return 0;
    }

    HUSKY_INLINE std::string any2utf8(std::string in, std::string fromEncode, std::string toEncode) {
        char *inbuf = (char *) in.c_str();
        const int inlen = strlen(inbuf);
        const int outlen = inlen * 3;//in case unicode 3 times than ascii
        char outbuf[outlen];
        memset(outbuf, 0, outlen);
        int rst = code_convert((char *) fromEncode.c_str(), (char *) toEncode.c_str(), inbuf, inlen, outbuf, outlen);
        if (rst == 0) {
            return std::string(outbuf);
        } else {
            return in;
        }
    }

    HUSKY_INLINE std::string gbk2utf8(const char *in) {
        return any2utf8(std::string(in), std::string("gbk"), std::string("utf-8"));
    }
}