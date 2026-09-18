#include "minicpu.h"

/* LOAD (0x01), STORE (0x02), MOV (0x05). */
int decode_execute_dataflow(CPU *cpu, uint8_t op, uint8_t a, uint8_t b) {
    switch (op) {
    case 0x01: // load
        cpu->reg[a] = cpu->mem[b];
        return 1;
    case 0x02: // store
        cpu->mem[b] = cpu->reg[a];
        return 1;
    case 0x05: // mov
        cpu->reg[a] = b;
        return 1;
    }
    return 0;
}
