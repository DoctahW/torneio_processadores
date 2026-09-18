#include "minicpu.h"

/* grava uma instrução (3 bytes) em mem[addr] */
static void emit(CPU *cpu, uint8_t addr, uint8_t op, uint8_t a, uint8_t b) {
    cpu->mem[addr]     = op;
    cpu->mem[addr + 1] = a;
    cpu->mem[addr + 2] = b;
}

/* fibonacci: fib(N), N em 0x10, resultado em 0x20
 *
 * R0 = fib(i), R1 = fib(i+1), R2 = contador, R3 = temporário
 * repete N vezes: t = R0 + R1; R0 = R1; R1 = t
 * no fim R0 = fib(N)
 *
 * o código não cabe antes de 0x10 (dados), então começa em 0x30
 * e a posição 0x00 só faz JMP pra lá.
 */
void load_program(CPU *cpu) {
    enum { OP_LOAD = 1, OP_STORE, OP_ADD, OP_SUB, OP_MOV, OP_CMP,
           OP_JMP, OP_JZ, OP_JNZ, OP_HALT };
    const uint8_t inicio = 0x30;
    const uint8_t loop   = 0x39;

    cpu->mem[0x10] = 7; /* N */

    emit(cpu, 0x00, OP_JMP, inicio, 0);

    emit(cpu, 0x30, OP_LOAD, 2, 0x10);  /* R2 = N            */
    emit(cpu, 0x33, OP_MOV,  0, 0);     /* R0 = 0            */
    emit(cpu, 0x36, OP_MOV,  1, 1);     /* R1 = 1            */
    /* loop: */
    emit(cpu, 0x39, OP_MOV,  3, 0);     /* R3 = 0            */
    emit(cpu, 0x3C, OP_ADD,  3, 0);     /* R3 += R0          */
    emit(cpu, 0x3F, OP_ADD,  3, 1);     /* R3 += R1  (t)     */
    emit(cpu, 0x42, OP_MOV,  0, 0);     /* R0 = 0            */
    emit(cpu, 0x45, OP_ADD,  0, 1);     /* R0 = R1           */
    emit(cpu, 0x48, OP_MOV,  1, 0);     /* R1 = 0            */
    emit(cpu, 0x4B, OP_ADD,  1, 3);     /* R1 = t            */
    emit(cpu, 0x4E, OP_MOV,  3, 1);     /* R3 = 1            */
    emit(cpu, 0x51, OP_SUB,  2, 3);     /* R2--              */
    emit(cpu, 0x54, OP_MOV,  3, 0);     /* R3 = 0            */
    emit(cpu, 0x57, OP_CMP,  2, 3);     /* R2 == 0 ?         */
    emit(cpu, 0x5A, OP_JNZ,  loop, 0);  /* se não, repete    */
    emit(cpu, 0x5D, OP_STORE, 0, 0x20); /* MEM[0x20] = R0    */
    emit(cpu, 0x60, OP_HALT, 0, 0);
}
