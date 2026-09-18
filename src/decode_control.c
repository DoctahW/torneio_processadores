#include "minicpu.h"

/* JMP (0x07), JZ (0x08), JNZ (0x09), HALT (0x0A). */
int decode_execute_control(CPU *cpu, uint8_t op, uint8_t a, uint8_t b) {
    (void)b;
    switch (op) {
    case 0x07: /* pula para o endereço a */
        cpu->pc = a;
        return 1;

    case 0x08: /* pula para o endereço a se zf == 1 */
        if(cpu->zf){
            cpu->pc = a;
        }
        return 1;

    case 0x09: /* pula para o endereço a se zf == 0 */
        if(!cpu->zf){
            cpu->pc = a;
        }
        return 1;

    case 0x0A: /* para  */
        cpu->running = 0;
        return 1;
    }
    return 0;
}
