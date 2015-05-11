#ifndef TINYMT32_H
#define TINYMT32_H
/**
 * @file tinymt32.h
 *
 * @brief Tiny Mersenne Twister only 127 bit internal state
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (University of Tokyo)
 *
 * Copyright (C) 2011 Mutsuo Saito, Makoto Matsumoto,
 * Hiroshima University and The University of Tokyo.
 * All rights reserved.
 *
 * The 3-clause BSD License is applied to this software, see
 * LICENSE.txt
 */


#define TINYMT32_MEXP 127
#define TINYMT32_SH0 1
#define TINYMT32_SH1 10
#define TINYMT32_SH8 8
#define TINYMT32_MASK ((const Uint32)(0x7fffffff))
#define TINYMT32_MUL (1.0f / 4294967296.0f)

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * tinymt32 internal state vector and parameters
 */
struct TINYMT32_T {
    Uint32 status[4];
    Uint32 mat1;
    Uint32 mat2;
    Uint32 tmat;
};

typedef struct TINYMT32_T tinymt32_t;

void tinymt32_init(tinymt32_t * random, Uint32 seed);
void tinymt32_init_by_array(tinymt32_t * random, Uint32 init_key[],
			    int key_length);

float tinymt32_generate_float(tinymt32_t * random);

#if defined(__cplusplus)
}
#endif

#endif
