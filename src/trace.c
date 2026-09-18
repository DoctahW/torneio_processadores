#include <stdio.h>
#include "minicpu.h"

//mapeia opcode pro nome legível
static const char *mnemonic(uint8_t op){
    static const char *names[] = {
        "????", "LOAD", "STORE", "ADD",
        "SUB",  "MOV",  "CMP",   "JMP",
        "JZ",   "JNZ",  "HALT"
    };
    if(op <= 0x0A) return names[op];
    return names[0];
}

/* imprime o estado da CPU a cada ciclo. */
void trace(CPU *cpu, uint8_t op, uint8_t a, uint8_t b) {
    printf("Ciclo %d: %-5s %d,%d | "
           "R0=%3d R1=%3d R2=%3d R3=%3d | "
           "PC=%3d ZF=%d\n",
           cpu->ciclo, mnemonic(op), a, b,
           cpu->reg[0], cpu->reg[1], cpu->reg[2], cpu->reg[3],
           cpu->pc, cpu->zf);
}
