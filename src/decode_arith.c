#include "minicpu.h"

/* ADD (0x03), SUB (0x04), CMP (0x06) + flag ZF. */
int decode_execute_arith(CPU *cpu, uint8_t op, uint8_t a, uint8_t b) {
    switch (op) {
    case 0x03:
        cpu->reg[a] += cpu->reg[b];
        cpu->zf = (cpu->reg[a] == 0);
        return 1;
    case 0x04:
        cpu->reg[a] -= cpu->reg[b];
        cpu->zf = (cpu->reg[a] == 0);
        return 1;
    case 0x06:
        cpu->zf = (cpu->reg[a] == cpu->reg[b]);
        return 1;
    }
    return 0;
}
