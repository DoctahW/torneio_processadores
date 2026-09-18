# Torneio de Processadores — MiniCPU

**Equipe:** Grupo 1 — Productions Developments
**Disciplina:** Infraestrutura de Hardware
**Desafio:** Fibonacci

Simulador da **MiniCPU** em C, que executa o ciclo **Fetch → Decode → Execute** e imprime o estado do processador a cada ciclo (trace).

## Desafio do Grupo 1: Fibonacci

Pré-carregar **N = 7** na memória (`0x10`), calcular o N-ésimo número de Fibonacci e gravar o resultado em `0x20`.

**Validação:** fib(7) = **13**.

## Integrantes e divisões de tarefa

| Integrante | Parte |
|---|---|
| João Euclides | Estrutura do simulador: `minicpu.h`, `cpu_init`, `fetch`, loop principal (`run`), `main.c`, Makefile |
| Daniel Donaire | Instruções de fluxo de dados: `LOAD`, `STORE`, `MOV` (`decode_dataflow.c`) |
| Vinícius Barbosa | Instruções aritméticas: `ADD`, `SUB`, `CMP` e flag ZF (`decode_arith.c`) |
| Ariel Cavalcanti | Instruções de controle: `JMP`, `JZ`, `JNZ`, `HALT` (`decode_control.c`) |
| Pedro Bedor | Trace de execução (`trace.c`) |
| Beatriz Loyola | Programa de teste do Fibonacci em memória (`program.c`) |

## Como compilar e executar

Requer `gcc` e `make`.

```
make        # compila e gera o executável ./minicpu
make run    # compila e executa
make clean  # remove o executável
```

## Estrutura do projeto

```
include/minicpu.h        struct CPU (memória, registradores, PC, ZF) e protótipos
src/main.c               inicializa a CPU, carrega o programa e executa
src/cpu_core.c           cpu_init, fetch, decode_execute e o loop run
src/decode_dataflow.c    LOAD, STORE, MOV
src/decode_arith.c       ADD, SUB, CMP
src/decode_control.c     JMP, JZ, JNZ, HALT
src/trace.c              impressão do estado a cada ciclo
src/program.c            programa e dados do Fibonacci na memória
```

## Arquitetura da MiniCPU

- **Registradores:** R0–R3 (8 bits), PC e ZF (zero flag).
- **Memória:** 256 posições de 1 byte, compartilhada entre instruções e dados.
- **Instrução:** 3 bytes, no formato `[opcode] [operando1] [operando2]`.

| Opcode | Mnemônico | Operação |
|---|---|---|
| 0x01 | `LOAD R, addr` | R ← MEM[addr] |
| 0x02 | `STORE R, addr` | MEM[addr] ← R |
| 0x03 | `ADD R1, R2` | R1 ← R1 + R2 |
| 0x04 | `SUB R1, R2` | R1 ← R1 − R2 |
| 0x05 | `MOV R, imm` | R ← imm |
| 0x06 | `CMP R1, R2` | ZF ← (R1 == R2) |
| 0x07 | `JMP addr` | PC ← addr |
| 0x08 | `JZ addr` | se ZF: PC ← addr |
| 0x09 | `JNZ addr` | se !ZF: PC ← addr |
| 0x0A | `HALT` | encerra a execução |

## Como o ciclo Fetch-Decode-Execute foi implementado

O loop está em `run()` ([src/cpu_core.c](src/cpu_core.c)) e se repete até o `HALT`:

1. **Fetch** (`fetch`): lê `mem[PC]`, `mem[PC+1]` e `mem[PC+2]` (opcode e dois operandos) e avança `PC += 3`.
2. **Decode** (`decode_execute`): identifica a operação a partir do opcode. O decode é dividido em três grupos, cada um com um `switch` sobre o opcode: dados (`decode_execute_dataflow`), aritmética (`decode_execute_arith`) e controle (`decode_execute_control`). Cada função retorna 1 se tratou o opcode.
3. **Execute**: cada `case` realiza a operação: altera registradores, memória, ZF ou PC (nos saltos).
4. **Trace** (`trace`): imprime o estado ao fim de cada ciclo.

Como o `PC` já foi incrementado no fetch, um salto só precisa sobrescrever o `PC` com o endereço de destino.

## Programa do Fibonacci

Lógica: `R0 = fib(i)`, `R1 = fib(i+1)`, `R2` = contador, `R3` = temporário. Repete N vezes: `t = R0 + R1; R0 = R1; R1 = t`. Ao final, `R0 = fib(N)`.

A MiniCPU não tem `MOV` entre registradores, então `R0 = R1` é feito com `MOV R0, 0` seguido de `ADD R0, R1`.

O código ocupa 51 bytes e não caberia antes do dado em `0x10`. Por isso a posição `0x00` tem só um `JMP 0x30`, e o programa de fato começa em `0x30`.

Dados: `MEM[0x10] = 7` (N). Resultado: `MEM[0x20]`.

| Endereço | Instrução | Comentário |
|---|---|---|
| 0x00 | `JMP 0x30` | desvia para depois da área de dados |
| 0x30 | `LOAD R2, 0x10` | R2 = N |
| 0x33 | `MOV R0, 0` | R0 = 0 |
| 0x36 | `MOV R1, 1` | R1 = 1 |
| 0x39 | `MOV R3, 0` | **início do loop**: R3 = 0 |
| 0x3C | `ADD R3, R0` | R3 = R0 |
| 0x3F | `ADD R3, R1` | R3 = R0 + R1 (t) |
| 0x42 | `MOV R0, 0` | R0 = 0 |
| 0x45 | `ADD R0, R1` | R0 = R1 |
| 0x48 | `MOV R1, 0` | R1 = 0 |
| 0x4B | `ADD R1, R3` | R1 = t |
| 0x4E | `MOV R3, 1` | R3 = 1 |
| 0x51 | `SUB R2, R3` | R2 = R2 − 1 |
| 0x54 | `MOV R3, 0` | R3 = 0 |
| 0x57 | `CMP R2, R3` | R2 == 0? |
| 0x5A | `JNZ 0x39` | se não, repete o loop |
| 0x5D | `STORE R0, 0x20` | MEM[0x20] = R0 |
| 0x60 | `HALT` | encerra |

Limitação: com N = 0 o contador dá a volta em 8 bits e o loop roda 256 vezes. O desafio usa N = 7.

## Exemplo de trace

Formato: `Ciclo n: instrução operando1,operando2 | registradores | PC e ZF` (operandos em decimal). A execução completa tem 90 ciclos. Início e fim:

```
Ciclo 1: JMP   48,0 | R0=  0 R1=  0 R2=  0 R3=  0 | PC= 48 ZF=0
Ciclo 2: LOAD  2,16 | R0=  0 R1=  0 R2=  7 R3=  0 | PC= 51 ZF=0
Ciclo 3: MOV   0,0 | R0=  0 R1=  0 R2=  7 R3=  0 | PC= 54 ZF=0
Ciclo 4: MOV   1,1 | R0=  0 R1=  1 R2=  7 R3=  0 | PC= 57 ZF=0
...
Ciclo 87: CMP   2,3 | R0= 13 R1= 21 R2=  0 R3=  0 | PC= 90 ZF=1
Ciclo 88: JNZ   57,0 | R0= 13 R1= 21 R2=  0 R3=  0 | PC= 93 ZF=1
Ciclo 89: STORE 0,32 | R0= 13 R1= 21 R2=  0 R3=  0 | PC= 96 ZF=1
Ciclo 90: HALT  0,0 | R0= 13 R1= 21 R2=  0 R3=  0 | PC= 99 ZF=1
```

O `STORE` no ciclo 89 grava R0 = **13** em `MEM[0x20]`, que é fib(7).

## Repositório

https://github.com/DoctahW/torneio_processadores
