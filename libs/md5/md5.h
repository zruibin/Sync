#ifndef MD5_H
#define MD5_H

#include <string>
#include <fstream>

namespace security {

/* Type define */
typedef unsigned char uint8;
typedef unsigned int uint32;

using std::string;
using std::ifstream;

/* MD5 declaration. */
class MD5 {
public:
    MD5();
    MD5(const void* input, size_t length);
    MD5(const string& str);
    MD5(ifstream& in);
    void update(const void* input, size_t length);
    void update(const string& str);
    void update(ifstream& in);
    const uint8* digest();
    string toString();
    void reset();
    
private:
    void update(const uint8* input, size_t length);
    void final();
    void transform(const uint8 block[64]);
    void encode(const uint32* input, uint8* output, size_t length);
    void decode(const uint8* input, uint32* output, size_t length);
    string bytesToHexString(const uint8* input, size_t length);
    
    /* class uncopyable */
    MD5(const MD5&);
    MD5& operator=(const MD5&);
    
private:
    uint32 _state[4];	/* state (ABCD) */
    uint32 _count[2];	/* number of bits, modulo 2^64 (low-order word first) */
    uint8 _buffer[64];	/* input buffer */
    uint8 _digest[16];	/* message digest */
    bool _finished;		/* calculate finished ? */
    
    static const uint8 PADDING[64];	/* padding for calculate */
    static const char HEX[16];
    enum { BUFFER_SIZE = 1024 };
};

};

#endif /*MD5_H*/
