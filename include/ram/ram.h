/**
 * 注意:
 *  此结构的内存接口模拟由于占用实际内存过大，现在已经将其停用。
 */

// #ifndef __RAM_UNIT
// #define __RAM_UNIT

// #include "cpu/global.h"
// #include <stdint.h>
// /**
//  * 目的：模拟真实内存结构
//  *  真实内存就是一块一块的
//  */

//  /**
//  * 本 RAM 一次能读取的值为4 字节/128 字节
//  * 仅仅跟 L2 缓存打配合
//  */
// #define RAM_IN_OPCODE_RD 0
// #define RAM_IN_OPCODE_WR 1

// #define RAM_IN_OPBYTES_128BYTE 0
// #define RAM_IN_OPBYTES_4BYTE 1

// #define RAM_BLOCK_MAX_POSITION 14
// #define RAM_BLOCK_RECT_WIDTH (1 << RAM_BLOCK_MAX_POSITION)
// #define RAM_BLOCK_RECT_SIZE RAM_BLOCK_RECT_WIDTH*RAM_BLOCK_RECT_WIDTH
// #define RAM_BLOCK_COUNT 8

// typedef struct {
//     uint8_t status_flag:1; //就一位，1代表内存超限
//     uint32_t data[L2_GROUP_SIZE / sizeof(uint32_t)];
// } RAM_OPERATOR_RESULT;

// typedef struct {
//     uint8_t opcode:2;
//     uint8_t opbytes:1; 
//     //若是WRITE操作,此位忽略
//     uint32_t op_x:RAM_BLOCK_MAX_POSITION;
//     uint32_t op_y:RAM_BLOCK_MAX_POSITION;
//     uint32_t val4bIn;
// } RAM_IN_ARGS;

// typedef uint8_t (*RAM_BLOCK)[RAM_BLOCK_RECT_WIDTH][RAM_BLOCK_RECT_WIDTH];

// typedef struct RAM_INTERFACE_UNIT RAM_INTERFACE_UNIT;
// /**
//  * 为什么这么设计:
//  *  真实电路中的RAM就是一堆输入引脚，一堆输出引脚。
//  *  这是在模拟引脚操作，in1 in2对应真实电路的引脚输入
//  *  uint32_t代表引脚输出，最多能输出4字节
//  */

// typedef RAM_OPERATOR_RESULT (*ram_operator_func)(RAM_INTERFACE_UNIT* unit,RAM_IN_ARGS input);
// struct RAM_INTERFACE_UNIT {
//     RAM_BLOCK blocks[RAM_BLOCK_COUNT];
//     ram_operator_func operatorFunc;
// };

// RAM_INTERFACE_UNIT* init_ram_unit();
// void free_ram_unit(RAM_INTERFACE_UNIT* unit);
// #endif