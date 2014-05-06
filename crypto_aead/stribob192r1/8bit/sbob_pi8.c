// sbob_pi8.c
// 16-Jan-14  Markku-Juhani O. Saarinen <mjos@iki.fi>
// 8-bit version of the Stribob AEAD

#include "stribob.h"

// discrete log of bit-reversed sbox

const uint8_t sbob_pi8log[0x100] = {
    0x5F, 0xDB, 0x3B, 0xA3, 0x11, 0x4D, 0x52, 0x18,
    0x1D, 0x50, 0x13, 0x81, 0xD8, 0x8C, 0x06, 0x76,
    0x12, 0xDC, 0xB8, 0x56, 0x90, 0x85, 0xC3, 0xCE,
    0x86, 0x0E, 0x59, 0xB4, 0xD3, 0xFB, 0xC7, 0xDA,
    0x31, 0xEB, 0xBF, 0xF4, 0xA8, 0x84, 0x30, 0x7D,
    0x97, 0x3F, 0xBA, 0x7C, 0x1A, 0x08, 0x09, 0x7F,
    0xD5, 0x7B, 0x07, 0x49, 0x57, 0xDF, 0xEF, 0xD0,
    0xED, 0x19, 0x92, 0xF6, 0xB0, 0xDE, 0xB5, 0x96,
    0x1C, 0x9D, 0x17, 0x37, 0xF1, 0x43, 0x25, 0x55,
    0x32, 0x79, 0x9C, 0x36, 0xB7, 0xFE, 0x98, 0x88,
    0xDD, 0x3D, 0x40, 0x2C, 0x05, 0xEC, 0x8E, 0x27,
    0x65, 0x6C, 0xF9, 0x0A, 0xE4, 0x69, 0xCF, 0xFA,
    0x7A, 0xC8, 0x53, 0x74, 0x04, 0x5B, 0x75, 0x91,
    0x1E, 0x62, 0xB9, 0xCA, 0xE7, 0x7E, 0x34, 0xB1,
    0x45, 0xF2, 0x46, 0x23, 0x58, 0xE0, 0x29, 0x60,
    0x82, 0x78, 0x63, 0x01, 0x2F, 0x9A, 0x9F, 0x4A,
    0xAD, 0x20, 0x26, 0x80, 0x95, 0x77, 0xA5, 0xF0,
    0x5D, 0x33, 0xB2, 0x5A, 0x94, 0xA2, 0xCB, 0xEE,
    0x3C, 0xBC, 0x8D, 0xC9, 0x22, 0xE1, 0x15, 0xC0,
    0x4C, 0xFC, 0xD2, 0xD1, 0xE5, 0x8A, 0xD4, 0xE2,
    0x3A, 0x8B, 0xE9, 0x68, 0xBB, 0x00, 0x51, 0xA1,
    0x9E, 0x83, 0x3E, 0x47, 0x44, 0xD9, 0xF8, 0x48,
    0x0B, 0x2A, 0xD7, 0x6E, 0xE6, 0xC1, 0x71, 0xC2,
    0xD6, 0xAC, 0x4B, 0x14, 0xCD, 0x6A, 0x4F, 0x8F,
    0x41, 0x16, 0x5C, 0x02, 0xA4, 0x2B, 0xFF, 0x1F,
    0xEA, 0x4E, 0xB3, 0x6B, 0xA0, 0xA7, 0xB6, 0xBD,
    0xAE, 0x0F, 0x10, 0x6F, 0xF7, 0x61, 0x93, 0xAF,
    0x42, 0x39, 0x70, 0x24, 0xBE, 0x0C, 0x87, 0xC5,
    0x03, 0xA9, 0x99, 0x72, 0xF5, 0x2D, 0x28, 0xAB,
    0x1B, 0x2E, 0xE3, 0x9B, 0xC6, 0x5E, 0x0D, 0x73,
    0x35, 0x38, 0xFD, 0xCC, 0x67, 0x21, 0xF3, 0xE8,
    0x66, 0x89, 0xA6, 0xC4, 0x6D, 0x54, 0x64, 0xAA
};

// bit-reversed discrete exponent

const uint8_t sbob_rv8exp[0x100] = {
    0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01,
    0x8E, 0x47, 0xAD, 0xD8, 0x6C, 0x36, 0x1B, 0x83,
    0xCF, 0xE9, 0xFA, 0x7D, 0xB0, 0x58, 0x2C, 0x16,
    0x0B, 0x8B, 0xCB, 0xEB, 0xFB, 0xF3, 0xF7, 0xF5,
    0xF4, 0x7A, 0x3D, 0x90, 0x48, 0x24, 0x12, 0x09,
    0x8A, 0x45, 0xAC, 0x56, 0x2B, 0x9B, 0xC3, 0xEF,
    0xF9, 0xF2, 0x79, 0xB2, 0x59, 0xA2, 0x51, 0xA6,
    0x53, 0xA7, 0xDD, 0xE0, 0x70, 0x38, 0x1C, 0x0E,
    0x07, 0x8D, 0xC8, 0x64, 0x32, 0x19, 0x82, 0x41,
    0xAE, 0x57, 0xA5, 0xDC, 0x6E, 0x37, 0x95, 0xC4,
    0x62, 0x31, 0x96, 0x4B, 0xAB, 0xDB, 0xE3, 0xFF,
    0xF1, 0xF6, 0x7B, 0xB3, 0xD7, 0xE5, 0xFC, 0x7E,
    0x3F, 0x91, 0xC6, 0x63, 0xBF, 0xD1, 0xE6, 0x73,
    0xB7, 0xD5, 0xE4, 0x72, 0x39, 0x92, 0x49, 0xAA,
    0x55, 0xA4, 0x52, 0x29, 0x9A, 0x4D, 0xA8, 0x54,
    0x2A, 0x15, 0x84, 0x42, 0x21, 0x9E, 0x4F, 0xA9,
    0xDA, 0x6D, 0xB8, 0x5C, 0x2E, 0x17, 0x85, 0xCC,
    0x66, 0x33, 0x97, 0xC5, 0xEC, 0x76, 0x3B, 0x93,
    0xC7, 0xED, 0xF8, 0x7C, 0x3E, 0x1F, 0x81, 0xCE,
    0x67, 0xBD, 0xD0, 0x68, 0x34, 0x1A, 0x0D, 0x88,
    0x44, 0x22, 0x11, 0x86, 0x43, 0xAF, 0xD9, 0xE2,
    0x71, 0xB6, 0x5B, 0xA3, 0xDF, 0xE1, 0xFE, 0x7F,
    0xB1, 0xD6, 0x6B, 0xBB, 0xD3, 0xE7, 0xFD, 0xF0,
    0x78, 0x3C, 0x1E, 0x0F, 0x89, 0xCA, 0x65, 0xBC,
    0x5E, 0x2F, 0x99, 0xC2, 0x61, 0xBE, 0x5F, 0xA1,
    0xDE, 0x6F, 0xB9, 0xD2, 0x69, 0xBA, 0x5D, 0xA0,
    0x50, 0x28, 0x14, 0x0A, 0x05, 0x8C, 0x46, 0x23,
    0x9F, 0xC1, 0xEE, 0x77, 0xB5, 0xD4, 0x6A, 0x35,
    0x94, 0x4A, 0x25, 0x9C, 0x4E, 0x27, 0x9D, 0xC0,
    0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x8F, 0xC9,
    0xEA, 0x75, 0xB4, 0x5A, 0x2D, 0x98, 0x4C, 0x26,
    0x13, 0x87, 0xCD, 0xE8, 0x74, 0x3A, 0x1D, 0x80
};

// logs of MDS matrix elements

const uint8_t sbob_matlog[8][8] = {
    {   0x08, 0x02, 0x12, 0x39, 0x2C, 0xCF, 0xF2, 0x3C  },
    {   0xCF, 0xA2, 0xB4, 0x00, 0x96, 0x08, 0xEE, 0x01  },
    {   0x23, 0x80, 0xDC, 0x78, 0x3D, 0x21, 0xB5, 0xC9  },
    {   0xE6, 0x75, 0xB7, 0xCE, 0xC6, 0x88, 0xD2, 0x36  },
    {   0xA3, 0xE5, 0xCE, 0xB7, 0xE3, 0xFB, 0x6F, 0x76  },
    {   0x29, 0x0C, 0x9C, 0x9F, 0x21, 0x3D, 0xD4, 0xCA  },
    {   0x6A, 0x12, 0x02, 0x77, 0x76, 0x18, 0x44, 0xE3  },
    {   0x3C, 0x37, 0x4A, 0x4C, 0x25, 0x01, 0x34, 0x08  }
};

// round constants

const uint8_t sbob_rdcnst[12][64] = {

    {   0xB1, 0x08, 0x5B, 0xDA, 0x1E, 0xCA, 0xDA, 0xE9, //  1
        0xEB, 0xCB, 0x2F, 0x81, 0xC0, 0x65, 0x7C, 0x1F,
        0x2F, 0x6A, 0x76, 0x43, 0x2E, 0x45, 0xD0, 0x16,
        0x71, 0x4E, 0xB8, 0x8D, 0x75, 0x85, 0xC4, 0xFC,
        0x4B, 0x7C, 0xE0, 0x91, 0x92, 0x67, 0x69, 0x01,
        0xA2, 0x42, 0x2A, 0x08, 0xA4, 0x60, 0xD3, 0x15,
        0x05, 0x76, 0x74, 0x36, 0xCC, 0x74, 0x4D, 0x23,
        0xDD, 0x80, 0x65, 0x59, 0xF2, 0xA6, 0x45, 0x07  },
        
    {   0x6F, 0xA3, 0xB5, 0x8A, 0xA9, 0x9D, 0x2F, 0x1A, //  2
        0x4F, 0xE3, 0x9D, 0x46, 0x0F, 0x70, 0xB5, 0xD7,
        0xF3, 0xFE, 0xEA, 0x72, 0x0A, 0x23, 0x2B, 0x98,
        0x61, 0xD5, 0x5E, 0x0F, 0x16, 0xB5, 0x01, 0x31,
        0x9A, 0xB5, 0x17, 0x6B, 0x12, 0xD6, 0x99, 0x58,
        0x5C, 0xB5, 0x61, 0xC2, 0xDB, 0x0A, 0xA7, 0xCA,
        0x55, 0xDD, 0xA2, 0x1B, 0xD7, 0xCB, 0xCD, 0x56,
        0xE6, 0x79, 0x04, 0x70, 0x21, 0xB1, 0x9B, 0xB7  },
        
    {   0xF5, 0x74, 0xDC, 0xAC, 0x2B, 0xCE, 0x2F, 0xC7, //  3
        0x0A, 0x39, 0xFC, 0x28, 0x6A, 0x3D, 0x84, 0x35,
        0x06, 0xF1, 0x5E, 0x5F, 0x52, 0x9C, 0x1F, 0x8B,
        0xF2, 0xEA, 0x75, 0x14, 0xB1, 0x29, 0x7B, 0x7B,
        0xD3, 0xE2, 0x0F, 0xE4, 0x90, 0x35, 0x9E, 0xB1,
        0xC1, 0xC9, 0x3A, 0x37, 0x60, 0x62, 0xDB, 0x09,
        0xC2, 0xB6, 0xF4, 0x43, 0x86, 0x7A, 0xDB, 0x31,
        0x99, 0x1E, 0x96, 0xF5, 0x0A, 0xBA, 0x0A, 0xB2  },
        
    {   0xEF, 0x1F, 0xDF, 0xB3, 0xE8, 0x15, 0x66, 0xD2, //  4
        0xF9, 0x48, 0xE1, 0xA0, 0x5D, 0x71, 0xE4, 0xDD,
        0x48, 0x8E, 0x85, 0x7E, 0x33, 0x5C, 0x3C, 0x7D,
        0x9D, 0x72, 0x1C, 0xAD, 0x68, 0x5E, 0x35, 0x3F,
        0xA9, 0xD7, 0x2C, 0x82, 0xED, 0x03, 0xD6, 0x75,
        0xD8, 0xB7, 0x13, 0x33, 0x93, 0x52, 0x03, 0xBE,
        0x34, 0x53, 0xEA, 0xA1, 0x93, 0xE8, 0x37, 0xF1,
        0x22, 0x0C, 0xBE, 0xBC, 0x84, 0xE3, 0xD1, 0x2E  },
        
    {   0x4B, 0xEA, 0x6B, 0xAC, 0xAD, 0x47, 0x47, 0x99, //  5
        0x9A, 0x3F, 0x41, 0x0C, 0x6C, 0xA9, 0x23, 0x63,
        0x7F, 0x15, 0x1C, 0x1F, 0x16, 0x86, 0x10, 0x4A,
        0x35, 0x9E, 0x35, 0xD7, 0x80, 0x0F, 0xFF, 0xBD,
        0xBF, 0xCD, 0x17, 0x47, 0x25, 0x3A, 0xF5, 0xA3,
        0xDF, 0xFF, 0x00, 0xB7, 0x23, 0x27, 0x1A, 0x16,
        0x7A, 0x56, 0xA2, 0x7E, 0xA9, 0xEA, 0x63, 0xF5,
        0x60, 0x17, 0x58, 0xFD, 0x7C, 0x6C, 0xFE, 0x57  },
        
    {   0xAE, 0x4F, 0xAE, 0xAE, 0x1D, 0x3A, 0xD3, 0xD9, //  6
        0x6F, 0xA4, 0xC3, 0x3B, 0x7A, 0x30, 0x39, 0xC0,
        0x2D, 0x66, 0xC4, 0xF9, 0x51, 0x42, 0xA4, 0x6C,
        0x18, 0x7F, 0x9A, 0xB4, 0x9A, 0xF0, 0x8E, 0xC6,
        0xCF, 0xFA, 0xA6, 0xB7, 0x1C, 0x9A, 0xB7, 0xB4,
        0x0A, 0xF2, 0x1F, 0x66, 0xC2, 0xBE, 0xC6, 0xB6,
        0xBF, 0x71, 0xC5, 0x72, 0x36, 0x90, 0x4F, 0x35,
        0xFA, 0x68, 0x40, 0x7A, 0x46, 0x64, 0x7D, 0x6E  },
        
    {   0xF4, 0xC7, 0x0E, 0x16, 0xEE, 0xAA, 0xC5, 0xEC, //  7
        0x51, 0xAC, 0x86, 0xFE, 0xBF, 0x24, 0x09, 0x54,
        0x39, 0x9E, 0xC6, 0xC7, 0xE6, 0xBF, 0x87, 0xC9,
        0xD3, 0x47, 0x3E, 0x33, 0x19, 0x7A, 0x93, 0xC9,
        0x09, 0x92, 0xAB, 0xC5, 0x2D, 0x82, 0x2C, 0x37,
        0x06, 0x47, 0x69, 0x83, 0x28, 0x4A, 0x05, 0x04,
        0x35, 0x17, 0x45, 0x4C, 0xA2, 0x3C, 0x4A, 0xF3,
        0x88, 0x86, 0x56, 0x4D, 0x3A, 0x14, 0xD4, 0x93  },
        
    {   0x9B, 0x1F, 0x5B, 0x42, 0x4D, 0x93, 0xC9, 0xA7, //  8
        0x03, 0xE7, 0xAA, 0x02, 0x0C, 0x6E, 0x41, 0x41,
        0x4E, 0xB7, 0xF8, 0x71, 0x9C, 0x36, 0xDE, 0x1E,
        0x89, 0xB4, 0x44, 0x3B, 0x4D, 0xDB, 0xC4, 0x9A,
        0xF4, 0x89, 0x2B, 0xCB, 0x92, 0x9B, 0x06, 0x90,
        0x69, 0xD1, 0x8D, 0x2B, 0xD1, 0xA5, 0xC4, 0x2F,
        0x36, 0xAC, 0xC2, 0x35, 0x59, 0x51, 0xA8, 0xD9,
        0xA4, 0x7F, 0x0D, 0xD4, 0xBF, 0x02, 0xE7, 0x1E  },
        
    {   0x37, 0x8F, 0x5A, 0x54, 0x16, 0x31, 0x22, 0x9B, //  9
        0x94, 0x4C, 0x9A, 0xD8, 0xEC, 0x16, 0x5F, 0xDE,
        0x3A, 0x7D, 0x3A, 0x1B, 0x25, 0x89, 0x42, 0x24,
        0x3C, 0xD9, 0x55, 0xB7, 0xE0, 0x0D, 0x09, 0x84,
        0x80, 0x0A, 0x44, 0x0B, 0xDB, 0xB2, 0xCE, 0xB1,
        0x7B, 0x2B, 0x8A, 0x9A, 0xA6, 0x07, 0x9C, 0x54,
        0x0E, 0x38, 0xDC, 0x92, 0xCB, 0x1F, 0x2A, 0x60,
        0x72, 0x61, 0x44, 0x51, 0x83, 0x23, 0x5A, 0xDB  },
        
    {   0xAB, 0xBE, 0xDE, 0xA6, 0x80, 0x05, 0x6F, 0x52, // 10
        0x38, 0x2A, 0xE5, 0x48, 0xB2, 0xE4, 0xF3, 0xF3,
        0x89, 0x41, 0xE7, 0x1C, 0xFF, 0x8A, 0x78, 0xDB,
        0x1F, 0xFF, 0xE1, 0x8A, 0x1B, 0x33, 0x61, 0x03,
        0x9F, 0xE7, 0x67, 0x02, 0xAF, 0x69, 0x33, 0x4B,
        0x7A, 0x1E, 0x6C, 0x30, 0x3B, 0x76, 0x52, 0xF4,
        0x36, 0x98, 0xFA, 0xD1, 0x15, 0x3B, 0xB6, 0xC3,
        0x74, 0xB4, 0xC7, 0xFB, 0x98, 0x45, 0x9C, 0xED  },
        
    {   0x7B, 0xCD, 0x9E, 0xD0, 0xEF, 0xC8, 0x89, 0xFB, // 11
        0x30, 0x02, 0xC6, 0xCD, 0x63, 0x5A, 0xFE, 0x94,
        0xD8, 0xFA, 0x6B, 0xBB, 0xEB, 0xAB, 0x07, 0x61,
        0x20, 0x01, 0x80, 0x21, 0x14, 0x84, 0x66, 0x79,
        0x8A, 0x1D, 0x71, 0xEF, 0xEA, 0x48, 0xB9, 0xCA,
        0xEF, 0xBA, 0xCD, 0x1D, 0x7D, 0x47, 0x6E, 0x98,
        0xDE, 0xA2, 0x59, 0x4A, 0xC0, 0x6F, 0xD8, 0x5D,
        0x6B, 0xCA, 0xA4, 0xCD, 0x81, 0xF3, 0x2D, 0x1B  },
        
    {   0x37, 0x8E, 0xE7, 0x67, 0xF1, 0x16, 0x31, 0xBA, // 12
        0xD2, 0x13, 0x80, 0xB0, 0x04, 0x49, 0xB1, 0x7A,
        0xCD, 0xA4, 0x3C, 0x32, 0xBC, 0xDF, 0x1D, 0x77,
        0xF8, 0x20, 0x12, 0xD4, 0x30, 0x21, 0x9F, 0x9B,
        0x5D, 0x80, 0xEF, 0x9D, 0x18, 0x91, 0xCC, 0x86,
        0xE7, 0x1D, 0xA4, 0xAA, 0x88, 0xE1, 0x28, 0x52,
        0xFA, 0xF4, 0x17, 0xD5, 0xD9, 0xB2, 0x1B, 0x99,
        0x48, 0xBC, 0x92, 0x4A, 0xF1, 0x1B, 0xD7, 0x20  }
};

void sbob_pi(w512_t *s512)
{
    int i, j, k, r;
    uint8_t x, y, t[64];
    
    for (r = 0; r < 12; r++) {
    
        for (i = 0; i < 64; i++) {
            t[(i >> 3) ^ ((i & 7) << 3)] =
                sbob_pi8log[s512->b[i] ^ sbob_rdcnst[r][i]];
            s512->b[i] = 0;
        }
    
        for (k = 0; k < 64; k += 8) {
            for (i = 0; i < 8; i++) {
                x = t[k + i];
                if (x > 0) {	// NO PRODUCTION CODE! side channel..!
                    for (j = 0; j < 8; j++) {
                        y = sbob_matlog[i][j] + x;
                        if (y >= x)		// NO PRODUCTION CODE! side channel..!
                            y--;
                        s512->b[k + j] ^= sbob_rv8exp[y];
                    }
                }
            }
        }
    }
}

