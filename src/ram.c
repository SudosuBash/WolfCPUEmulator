/**
 * 注意:
 *  此结构的内存接口模拟由于占用实际内存过大，现在已经将其停用。
 */

// #include "ram.h"


// RAM_OPERATOR_RESULT operate(RAM_INTERFACE_UNIT* unit,RAM_IN_ARGS input) {
//     RAM_OPERATOR_RESULT result = {0};
    
// }
// RAM_INTERFACE_UNIT* init_ram_unit() {
//     RAM_INTERFACE_UNIT* unit = (RAM_INTERFACE_UNIT*) malloc(sizeof(RAM_INTERFACE_UNIT));

//     for(uint8_t i = 0;i< RAM_BLOCK_COUNT;i++) {
//         unit->blocks[i] = malloc(sizeof(uint8_t) * RAM_BLOCK_RECT_WIDTH * RAM_BLOCK_RECT_WIDTH);
//         if(unit->blocks[i] == NULL) {
//             goto FREE_RAM_BLOCKS;
//         }
//     }
// FREE_RAM_BLOCKS: 
//     for(uint8_t i = 0;i< RAM_BLOCK_COUNT;i++) {
//         if(unit->blocks[i] == NULL) break;
//         free(unit->blocks[i]);
//     }
//     free(unit);
// FREE_RAM_UNIT:
//     return NULL;
// }

// void free_ram_unit(RAM_INTERFACE_UNIT* unit) {
//     if(unit != NULL) {
//         for(uint8_t i = 0;i< RAM_BLOCK_COUNT;i++) {
//             if(unit->blocks[i] == NULL) break;
//             free(unit->blocks[i]);
//         }
//         free(unit);
//         unit = NULL;
//     }
// }