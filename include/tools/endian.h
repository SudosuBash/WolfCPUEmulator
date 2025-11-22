#ifndef _WOLF_CPU_TOOLS_H
#define _WOLF_CPU_TOOLS_H
#include <stdint.h>
#include <logics/logic_alg.h>
#include <global.h>

#if defined(__LITTLE_ENDIAN__) || defined(_LITTLE_ENDIAN) || defined(LITTLE_ENDIAN) || \
    (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#define IS_LITTLE_ENDIAN 1
#else
#define IS_LITTLE_ENDIAN 0
#endif

#define SEP_INT_FOR_4_BYTES_B(intval) \
    (intval) >> 24,((intval) >> 16) & 0xff,((intval) >> 8) & 0xff,((intval)) & 0xff

#define SEP_INT_FOR_4_BYTES_L(intval) \
    ((intval)) & 0xff,((intval) >> 8) & 0xff,((intval) >> 16) & 0xff,(intval) >> 24


#define SEP_INT_FOR_2_BYTES_L(intval) \
    ((intval)) & 0xff,((intval) >> 8) & 0xff


#define SEP_INT_FOR_4_BYTES(intval) \
    IS_LITTLE_ENDIAN ? ((intval)) & 0xff : (intval) >> 24 , \
    IS_LITTLE_ENDIAN ? ((intval) >> 8) & 0xff : ((intval) >> 16) & 0xff, \
    IS_LITTLE_ENDIAN ? ((intval) >> 16) & 0xff : ((intval) >> 8) & 0xff, \
    IS_LITTLE_ENDIAN ? (intval) >> 24 : ((intval)) & 0xff

#define GET_INT_FROM_4_BYTES_B(bytearr) ((bytearr)[0] << 24 | (bytearr)[1] << 16 | (bytearr)[2] << 8 | (bytearr)[3])
#define GET_INT_FROM_4_BYTES_L(bytearr) ((bytearr)[3] << 24 | (bytearr)[2] << 16 | (bytearr)[1] << 8 | (bytearr)[0])

#define GET_INT_FROM_2_BYTES_B(bytearr) ((bytearr)[0] << 8 | (bytearr)[1])
#define GET_INT_FROM_2_BYTES_L(bytearr) ((bytearr)[1] << 8 | (bytearr)[0])

#define GET_INT_FROM_4_BYTES(bytearr) (IS_LITTLE_ENDIAN ? GET_INT_FROM_4_BYTES_L(bytearr) : GET_INT_FROM_4_BYTES_B(bytearr))
#define COPY_BYTE_4_ARRAY(tar,ori) {\
    (tar)[0] = (ori)[0]; \
    (tar)[1] = (ori)[1]; \
    (tar)[2] = (ori)[2]; \
    (tar)[3] = (ori)[3]; \
}

#define COPY_BYTE_2_ARRAY(tar,ori) {\
    (tar)[0] = (ori)[0]; \
    (tar)[1] = (ori)[1]; \
}

#define COPY_BYTE_4_ARRAY_WITH_BE(ori,tar,be) {\
    (ori)[0] = Through8(GET_DATA_BIT0(be),(tar)[0]); \
    (ori)[1] = Through8(GET_DATA_BIT1(be),(tar)[1]); \
    (ori)[2] = Through8(GET_DATA_BIT2(be),(tar)[2]); \
    (ori)[3] = Through8(GET_DATA_BIT3(be),(tar)[3]); \
}

#define COPY_BYTE_4_ARRAY_SELECTED_WITH_BE(ori,tar,be) {\
    (ori)[0] = Mux8(GET_DATA_BIT0(be),(ori)[0],(tar)[0]); \
    (ori)[1] = Mux8(GET_DATA_BIT1(be),(ori)[1],(tar)[1]); \
    (ori)[2] = Mux8(GET_DATA_BIT2(be),(ori)[2],(tar)[2]); \
    (ori)[3] = Mux8(GET_DATA_BIT3(be),(ori)[3],(tar)[3]); \
}
#endif