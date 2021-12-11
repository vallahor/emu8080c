#pragma once
#include <stdint.h>

#define PUSH(val) (emu->memory[--emu->sp] = (val))
#define POP()     (emu->memory[emu->sp++])

#define ADDR_MEM()   (emu->reg[H] << 8) | (emu->reg[L])
#define ADDR_IMM()   (emu->memory[emu->pc + 2] << 8 | emu->memory[emu->pc + 1])
#define ADDR_RP(x,y) (emu->reg[(x)] << 8 | emu->reg[(y)])

#define CALL_INST 3

void swap(uint8_t* val1, uint8_t* val2) {
    uint8_t tmp = *val1;
    *val1 = *val2;
    *val2 = tmp;
}

#define A 0x7
#define B 0x0
#define C 0x1
#define D 0x2
#define E 0x3
#define H 0x4
#define L 0x5

static const char reg_lookup[8] = {
    'B',
    'C',
    'D',
    'E',
    'H',
    'L',
    '~',
    'A'
};

#define BC 0x0
#define DE 0x1
#define HL 0x2
#define SP 0x3

static const char* rp_lookup[4] = {
    "BC",
    "DE",
    "HL",
    "SP"
};

#define NZ 0x0
#define Z  0x1
#define NC 0x2
#define CY 0x3
#define PO 0x4
#define PE 0x5
#define P  0x6
#define M  0x7

static const char* cond_lookup[8] = {
    "NZ",
    "Z",
    "NC",
    "C",
    "PO",
    "PE",
    "P",
    "M"
};

typedef struct Flags {
    uint8_t z;
    uint8_t s;
    uint8_t p;
    uint8_t cy;
    uint8_t ac;
    uint8_t pad[3];
} Flags;

typedef struct Emu8080c {
    uint8_t reg[8];
    Flags flags;
    union {
        uint16_t pc;
        struct {
            uint8_t pch;
            uint8_t pcl;
        };
    };
    uint16_t sp;
    uint8_t* memory;
    uint8_t int_enable;
} Emu8080c;

uint8_t aux_cy(uint8_t val_old, uint8_t val_new) {
    return (val_new & 0x1000) == ((val_old & 0x0100) << 1);
}

uint8_t parity(uint8_t value, uint8_t bit_length) {
    int count = 0;
    value = (value & ((1 << bit_length) - 1));
    while (value) {
        if (value & 0x1) { count++; }
        value = value >> 1;
    }
    return ((count & 0x1) == 0);
}

void set_flags_cmp(Emu8080c* emu, uint8_t cmp) {
    emu->flags.z = (cmp == 0);
    emu->flags.s = ((cmp & 0x80) == 0x80);
    emu->flags.p = parity(cmp, 8);
}

void set_flags_value(Emu8080c* emu, uint16_t value) {
    emu->flags.cy = (value > 0xff);
    emu->flags.z  = ((value & 0xff) == 0);
    emu->flags.s  = ((value & 0x80) == 0x80);
    emu->flags.p  = parity(value & 0xff, 8);
    emu->flags.ac = aux_cy(emu->reg[A], value);
}

void set_flags_logic(Emu8080c* emu) {
    emu->flags.cy ^= emu->flags.cy;
    emu->flags.ac ^= emu->flags.ac;
    emu->flags.z  = (emu->reg[A] == 0);
    emu->flags.s  = ((emu->reg[A] & 0x80) == 0x80);
    emu->flags.p  = parity(emu->reg[A], 8);
}

void add(Emu8080c* emu, uint8_t src) {
    uint16_t result = (uint16_t)emu->reg[A] + (uint16_t)src;
    set_flags_value(emu, result);
    emu->reg[A] = result & 0xff;
}

void sub(Emu8080c* emu, uint8_t src) {
    uint16_t result = (uint16_t)emu->reg[A] - (uint16_t)src;
    set_flags_value(emu, result);
    emu->reg[A] = result & 0xff;
}

void add_cy(Emu8080c* emu, uint8_t src) {
    uint16_t result = (uint16_t)emu->reg[A] + (uint16_t)src + (uint16_t)emu->flags.cy;
    set_flags_value(emu, result);
    emu->reg[A] = result & 0xff;
}

void sub_cy(Emu8080c* emu, uint8_t src) {
    uint16_t result = (uint16_t)emu->reg[A] - (uint16_t)src - (uint16_t)emu->flags.cy;
    set_flags_value(emu, result);
    emu->reg[A] = result & 0xff;
}

uint8_t cond(Emu8080c* emu, uint8_t ccc) {
    uint8_t result = 0;
    switch (ccc) {
        case NZ: {
            result = (emu->flags.z == 0);
        } break;
        case Z: {
            result = (emu->flags.z == 1);
        } break;
        case NC: {
            result = (emu->flags.cy == 0);
        } break;
        case CY: {
            result = (emu->flags.cy == 1);
        } break;
        case PO: {
            result = (emu->flags.p == 0);
        } break;
        case PE: {
            result = (emu->flags.p == 1);
        } break;
        case P: {
            result = (emu->flags.s == 0);
        } break;
        case M: {
            result = (emu->flags.s == 1);
        } break;
    }
    return result;
}

uint8_t decode(struct Emu8080c* emu) {
    uint8_t opbytes = 1;

    printf("%04x ", emu->pc);

    uint8_t inst = emu->memory[emu->pc];
    switch (inst) {

            /* DATA TRANSFER GROUP */

        case 0x040: /* MOV B, B */
        case 0x041: /* MOV B, C */
        case 0x042: /* MOV B, D */
        case 0x043: /* MOV B, E */
        case 0x044: /* MOV B, H */
        case 0x045: /* MOV B, L */
        case 0x047: /* MOV B, A */
        case 0x048: /* MOV C, B */
        case 0x049: /* MOV C, C */
        case 0x04a: /* MOV C, D */
        case 0x04b: /* MOV C, E */
        case 0x04c: /* MOV C, H */
        case 0x04d: /* MOV C, L */
        case 0x04f: /* MOV C, A */
        case 0x050: /* MOV D, B */
        case 0x051: /* MOV D, C */
        case 0x052: /* MOV D, D */
        case 0x053: /* MOV D, E */
        case 0x054: /* MOV D, H */
        case 0x055: /* MOV D, L */
        case 0x057: /* MOV D, A */
        case 0x058: /* MOV E, B */
        case 0x059: /* MOV E, C */
        case 0x05a: /* MOV E, D */
        case 0x05b: /* MOV E, E */
        case 0x05c: /* MOV E, H */
        case 0x05d: /* MOV E, L */
        case 0x05f: /* MOV E, A */
        case 0x060: /* MOV H, B */
        case 0x061: /* MOV H, C */
        case 0x062: /* MOV H, D */
        case 0x063: /* MOV H, E */
        case 0x064: /* MOV H, H */
        case 0x065: /* MOV H, L */
        case 0x067: /* MOV H, A */
        case 0x068: /* MOV L, B */
        case 0x069: /* MOV L, C */
        case 0x06a: /* MOV L, D */
        case 0x06b: /* MOV L, E */
        case 0x06c: /* MOV L, H */
        case 0x06d: /* MOV L, L */
        case 0x06f: /* MOV L, A */
        case 0x078: /* MOV A, B */
        case 0x079: /* MOV A, C */
        case 0x07a: /* MOV A, D */
        case 0x07b: /* MOV A, E */
        case 0x07c: /* MOV A, H */
        case 0x07d: /* MOV A, L */
        case 0x07f: /* MOV A, A */ {
            uint8_t sss = (inst & 0b00000111);
            uint8_t ddd = (inst & 0b00111000) >> 3;
            emu->reg[ddd] = emu->reg[sss];
            printf("MOV %c, %c\n", reg_lookup[ddd], reg_lookup[sss]);
        } break;

        case 0x046: /* MOV B, M */
        case 0x04e: /* MOV C, M */
        case 0x056: /* MOV D, M */
        case 0x05e: /* MOV E, M */
        case 0x066: /* MOV H, M */
        case 0x06e: /* MOV L, M */
        case 0x07e: /* MOV A, M */ {
            uint8_t ddd = (inst & 0b00111000) >> 3;
            uint16_t addr = ADDR_MEM();
            emu->reg[ddd] = emu->memory[addr];
            printf("MOV %c, [0x%04x]\n", reg_lookup[ddd], addr);
        } break;

        case 0x070: /* MOV M, B */
        case 0x071: /* MOV M, C */
        case 0x072: /* MOV M, D */
        case 0x073: /* MOV M, E */
        case 0x074: /* MOV M, H */
        case 0x075: /* MOV M, L */
        case 0x077: /* MOV M, A */ {
            uint8_t sss = inst & 0b00000111;
            uint16_t addr = ADDR_MEM();
            emu->memory[addr] = emu->reg[sss];
            printf("MOV [0x%04x], %c\n", addr, reg_lookup[sss]);
        } break;

        case 0x006: /* MVI B, data */
        case 0x00e: /* MVI C, data */
        case 0x016: /* MVI D, data */
        case 0x01e: /* MVI E, data */
        case 0x026: /* MVI H, data */
        case 0x02e: /* MVI L, data */
        case 0x03e: /* MVI A, data */ {
            opbytes = 2;
            uint8_t ddd = (inst & 0b00111000) >> 3;
            uint8_t data = emu->memory[emu->pc + 1];
            emu->reg[ddd] = data;
            printf("MVI %c, 0x%02x\n", reg_lookup[ddd], data);
        } break;

        case 0x036: /* MVI M, data */ {
            opbytes = 2;
            uint16_t addr = ADDR_MEM();
            uint8_t data = emu->memory[emu->pc + 1];
            emu->memory[addr] = data;
            printf("MVI [0x%04x], %04x\n", addr, data);
        } break;

        case 0x001: /* LXI BC, data 16 */
        case 0x011: /* LXI DE, data 16 */
        case 0x021: /* LXI HL, data 16 */
        case 0x031: /* LXI SP, data 16 */ {
            opbytes = 3;
            uint8_t addr_l = emu->memory[emu->pc + 1];
            uint8_t addr_h = emu->memory[emu->pc + 2];
            uint8_t rp = (inst & 0b00110000) >> 4;
            switch (rp) {
                case BC: {
                    emu->reg[B] = addr_h;
                    emu->reg[C] = addr_l;
                } break;
                case DE: {
                    emu->reg[D] = addr_h;
                    emu->reg[E] = addr_l;
                } break;
                case HL: {
                    emu->reg[H] = addr_h;
                    emu->reg[L] = addr_l;
                } break;
                case SP: {
                    emu->sp = (addr_h << 8 | addr_l);
                } break;
            }
            printf("LXI %s, 0x%04x\n", rp_lookup[rp],(addr_h << 8 | addr_l));
        } break;

        case 0x03a: /* LDA addr */ {
            opbytes = 3;
            uint16_t addr = ADDR_IMM();
            emu->reg[A] = emu->memory[addr];
            printf("LDA [0x%04x]\n", addr);
        } break;

        case 0x032: /* STA addr */ {
            opbytes = 3;
            uint16_t addr = ADDR_IMM();
            emu->memory[addr] = emu->reg[A];
            printf("STA [0x%04x]\n", addr);
        } break;

        case 0x02a: /* LHLD addr */ {
            opbytes = 3;
            uint16_t addr = ADDR_IMM();
            emu->reg[L] = emu->memory[addr];
            emu->reg[H] = emu->memory[addr + 1];
            printf("LHLD [0x%04x]\n", addr);
        } break;

        case 0x022: /* SHLD addr */ {
            opbytes = 3;
            uint16_t addr = ADDR_IMM();
            emu->memory[addr] = emu->reg[L];
            emu->memory[addr + 1] = emu->reg[H];
            printf("SHLD [0x%04x]\n", addr);
        } break;

        case 0x00a: /* LDAX BC */
        case 0x01a: /* LDAX DE */ {
            uint16_t addr;
            uint8_t rp = (inst & 0b00110000) >> 4;
            switch (rp) {
                case BC: {
                    addr = ADDR_RP(B, C);
                } break;
                case DE: {
                    addr = ADDR_RP(D, E);
                } break;
            }
            emu->reg[A] = emu->memory[addr];
            printf("LDAX %s\n", rp_lookup[rp]);
        } break;

        case 0x002: /* STAX BC */
        case 0x012: /* STAX DE */ {
            uint16_t addr;
            uint8_t rp = (inst & 0b00110000) >> 4;
            switch (rp) {
                case BC: {
                    addr = ADDR_RP(B, C);
                } break;
                case DE: {
                    addr = ADDR_RP(D, E);
                } break;
            }
            emu->memory[addr] = emu->reg[A];
            printf("STAX %s\n", rp_lookup[rp]);
        } break;

        case 0x0eb: /* XCHG */ {
            swap(&emu->reg[H], &emu->reg[D]);
            swap(&emu->reg[L], &emu->reg[E]);
            printf("XCHG\n");
        } break;


            /* ARITHMETIC GROUP */

        case 0x080: /* ADD B */
        case 0x081: /* ADD C */
        case 0x082: /* ADD D */
        case 0x083: /* ADD E */
        case 0x084: /* ADD H */
        case 0x085: /* ADD L */
        case 0x087: /* ADD A */ {
            uint8_t sss = (inst & 0b00000111);
            uint8_t src = emu->reg[sss];
            add(emu, src);
            printf("ADD %c\n", reg_lookup[sss]);
        } break;

        case 0x086: /* ADD M */ {
            uint16_t addr = ADDR_MEM();
            uint8_t src = emu->memory[addr];
            add(emu, src);
            printf("ADD [0x%04x]\n", addr);
        } break;

        case 0x0c6: /* ADI data */ {
            opbytes = 2;
            uint8_t value = emu->memory[emu->pc + 1];
            add(emu, value);
            printf("ADI 0x%02x\n", value);
        } break;

        case 0x088: /* ADC B */
        case 0x089: /* ADC C */
        case 0x08a: /* ADC D */
        case 0x08b: /* ADC E */
        case 0x08c: /* ADC H */
        case 0x08d: /* ADC L */
        case 0x08f: /* ADC A */ {
            uint8_t sss = (inst & 0b00000111);
            uint8_t src = emu->reg[sss];
            add_cy(emu, src);
            printf("ADC %c\n", reg_lookup[sss]);
        } break;

        case 0x08e: /* ADC M */ {
            uint16_t addr = ADDR_MEM();
            uint8_t src = emu->memory[addr];
            add_cy(emu, src);
            printf("ADC [0x%04x]\n", addr);
        } break;

        case 0x0ce: /* ACI data */ {
            opbytes = 2;
            uint8_t value = emu->memory[emu->pc + 1];
            add_cy(emu, value);
            printf("ACI 0x%02x\n", value);
        } break;

        case 0x090: /* SUB B */
        case 0x091: /* SUB C */
        case 0x092: /* SUB D */
        case 0x093: /* SUB E */
        case 0x094: /* SUB H */
        case 0x095: /* SUB L */
        case 0x097: /* SUB A */ {
            uint8_t sss = (inst & 0b00000111);
            uint8_t src = emu->reg[sss];
            sub(emu, src);
            printf("SUB %c\n", reg_lookup[sss]);
        } break;

        case 0x096: /* SUB M */ {
            uint16_t addr = ADDR_MEM();
            uint8_t src = emu->memory[addr];
            sub(emu, src);
            printf("SUB [0x%04x]\n", addr);
        } break;

        case 0x0d6: /* SUI data */ {
            opbytes = 2;
            uint8_t value = emu->memory[emu->pc + 1];
            sub(emu, value);
            printf("SUI 0x%02x\n", value);
        } break;

        case 0x098: /* SBB B */
        case 0x099: /* SBB C */
        case 0x09a: /* SBB D */
        case 0x09b: /* SBB E */
        case 0x09c: /* SBB H */
        case 0x09d: /* SBB L */
        case 0x09f: /* SBB A */ {
            uint8_t sss = (inst & 0b00000111);
            uint8_t src = emu->reg[sss];
            sub_cy(emu, src);
            printf("SBB %c\n", reg_lookup[sss]);
        } break;

            // SBB MEMORY
        case 0x09e: /* SBB M */ {
            uint16_t addr = ADDR_MEM();
            uint8_t src = emu->memory[addr];
            sub_cy(emu, src);
            printf("SBB [0x%02x]\n", addr);
        } break;

        case 0x0de: /* SBI data */ {
            opbytes = 2;
            uint8_t value = emu->memory[emu->pc + 1];
            sub_cy(emu, value);
            printf("SBI 0x%02x\n", value);
        } break;

        case 0x004: /* INR B, data */
        case 0x00c: /* INR C, data */
        case 0x014: /* INR D, data */
        case 0x01c: /* INR E, data */
        case 0x024: /* INR H, data */
        case 0x02c: /* INR L, data */
        case 0x03c: /* INR A, data */ {
            uint8_t sss = (inst & 0b00111000) >> 3;
            emu->reg[sss] += 1;
            uint8_t value = emu->reg[sss];
            set_flags_value(emu, value);
            printf("INR %c\n", reg_lookup[sss]);
        } break;

        case 0x034: /* INR M */ {
            uint16_t addr = ADDR_MEM();
            emu->memory[addr] += 1;
            uint8_t value = emu->memory[addr];
            set_flags_value(emu, value);
            printf("INR [0x%04x]\n", addr);
        } break;

        case 0x005: /* DCR B, data */
        case 0x00d: /* DCR C, data */
        case 0x015: /* DCR D, data */
        case 0x01d: /* DCR E, data */
        case 0x025: /* DCR H, data */
        case 0x02d: /* DCR L, data */
        case 0x03d: /* DCR A, data */ {
            uint8_t sss = (inst & 0b00111000) >> 3;
            emu->reg[sss] -= 1;
            uint8_t value = emu->reg[sss];
            set_flags_value(emu, value);
            printf("DCR %c\n", reg_lookup[sss]);
        } break;

        case 0x035: /* DCR M */ {
            uint16_t addr = ADDR_MEM();
            emu->memory[addr] -= 1;
            uint8_t value = emu->memory[addr];
            set_flags_value(emu, value);
            printf("DCR [0x%04x]\n", addr);
        } break;

        case 0x003: /* INX BC */
        case 0x013: /* INX DE */
        case 0x023: /* INX HL */
        case 0x033: /* INX SP */ {
            uint16_t value;
            uint8_t rp = (inst & 0b00110000) >> 4;
            switch (rp) {
                case BC: {
                    value = ADDR_RP(B, C) + 1;
                    emu->reg[B] = value >> 8;
                    emu->reg[C] = value & 0xff;
                } break;
                case DE: {
                    value = ADDR_RP(D, E) + 1;
                    emu->reg[D] = value >> 8;
                    emu->reg[E] = value & 0xff;
                } break;
                case HL: {
                    value = ADDR_RP(H, L) + 1;
                    emu->reg[H] = value >> 8;
                    emu->reg[L] = value & 0xff;
                } break;
                case SP: {
                    emu->sp += 1;
                } break;
            }
            printf("INX %s\n", rp_lookup[rp]);
        } break;

        case 0x00b: /* DCX BC */
        case 0x01b: /* DCX DE */
        case 0x02b: /* DCX HL */
        case 0x03b: /* DCX SP */ {
            uint16_t value;
            uint8_t rp = (inst & 0b00110000) >> 4;
            switch (rp) {
                case BC: {
                    value = ADDR_RP(B, C) - 1;
                    emu->reg[B] = value >> 8;
                    emu->reg[C] = value & 0xff;
                } break;
                case DE: {
                    value = ADDR_RP(D, E) - 1;
                    emu->reg[D] = value >> 8;
                    emu->reg[E] = value & 0xff;
                } break;
                case HL: {
                    value = ADDR_RP(H, L) - 1;
                    emu->reg[H] = value >> 8;
                    emu->reg[L] = value & 0xff;
                } break;
                case SP: {
                    emu->sp -= 1;
                } break;
            }
            printf("DCX %s\n", rp_lookup[rp]);
        } break;

        case 0x009: /* DAD BC */
        case 0x019: /* DAD DE */
        case 0x029: /* DAD HL */
        case 0x039: /* DAD SP */ {
            uint16_t rp_val;
            uint16_t hl_val = ADDR_MEM();
            uint8_t rp = (inst & 0b00110000) >> 4;
            switch (rp) {
                case BC: {
                    rp_val = ADDR_RP(B, C);
                } break;
                case DE: {
                    rp_val = ADDR_RP(D, E);
                } break;
                case HL: {
                    rp_val = ADDR_RP(H, L);
                } break;
                case SP: {
                    rp_val = emu->sp;
                } break;
            }
            hl_val += rp_val;
            emu->reg[H] = hl_val >> 8;
            emu->reg[L] = hl_val & 0xff;
            emu->flags.cy = (hl_val > 0xff);
            printf("DAD %s\n", rp_lookup[rp]);
        } break;

        case 0x027: /* DAA */ {
            uint16_t acc = emu->reg[A];
            uint8_t least_sig = acc & 0x0f;
            if (least_sig > 0x9 || emu->flags.ac) {
                acc += 6;
                uint8_t most_sig = acc >> 4;
                if (most_sig > 0x9 || emu->flags.cy) {
                    most_sig += 6;
                    acc &= 0x0f;
                    acc = (most_sig << 4 | acc);
                }
                emu->reg[A] = (uint8_t)acc;
                set_flags_value(emu, acc);
            }
            printf("DAA\n");
        } break;


            /* LOGICAL GROUP */

        case 0x0a0: /* ANA B */
        case 0x0a1: /* ANA C */
        case 0x0a2: /* ANA D */
        case 0x0a3: /* ANA E */
        case 0x0a4: /* ANA H */
        case 0x0a5: /* ANA L */
        case 0x0a7: /* ANA A */ {
            uint8_t sss = inst & 0b00000111;
            emu->reg[A] &= emu->reg[sss];
            set_flags_logic(emu);
            printf("ANA %c\n", reg_lookup[sss]);
        } break;

        case 0x0a6: /* ANA M */ {
            uint16_t addr = ADDR_MEM();
            emu->reg[A] &= emu->memory[addr];
            set_flags_logic(emu);
            printf("ANA [0x%04x]\n", addr);
        } break;

        case 0x0e6: /* ANI data */ {
            opbytes = 2;
            uint8_t value = emu->memory[emu->pc + 1];
            emu->reg[A] &= value;
            set_flags_logic(emu);
            printf("ANI 0x%02x\n", value);
        } break;

        case 0x0a8: /* XRA B */
        case 0x0a9: /* XRA C */
        case 0x0aa: /* XRA D */
        case 0x0ab: /* XRA E */
        case 0x0ac: /* XRA H */
        case 0x0ad: /* XRA L */
        case 0x0af: /* XRA A */ {
            uint8_t sss = inst & 0b00000111;
            emu->reg[A] ^= emu->reg[sss];
            set_flags_logic(emu);
            printf("XRA %c\n", reg_lookup[sss]);
        } break;

        case 0x0ae: /* XRA M */ {
            uint16_t addr = ADDR_MEM();
            uint8_t value = emu->memory[addr];
            emu->reg[A] ^= value;
            set_flags_logic(emu);
            printf("XRA [0x%04x]\n", addr);
        } break;

        case 0x0ee: /* XRI data */ {
            opbytes = 2;
            uint8_t value = emu->memory[emu->pc + 1];
            emu->reg[A] ^= value;
            set_flags_logic(emu);
            printf("XRI 0x%02x\n", value);
        } break;

        case 0x0b0: /* ORA B */
        case 0x0b1: /* ORA C */
        case 0x0b2: /* ORA D */
        case 0x0b3: /* ORA E */
        case 0x0b4: /* ORA H */
        case 0x0b5: /* ORA L */
        case 0x0b7: /* ORA A */ {
            uint8_t sss = inst & 0b00000111;
            emu->reg[A] |= emu->reg[sss];
            set_flags_logic(emu);
            printf("ORA %c\n", reg_lookup[sss]);
        } break;

        case 0x0b6: /* ORA M */ {
            uint16_t addr = ADDR_MEM();
            uint8_t value = emu->memory[addr];
            emu->reg[A] |= value;
            set_flags_logic(emu);
            printf("ORA [0x%04x]\n", addr);
        } break;

        case 0x0f6: /* ORI data */ {
            opbytes = 2;
            uint8_t value = emu->memory[emu->pc + 1];
            emu->reg[A] |= value;
            set_flags_logic(emu);
            printf("ORI 0x%02x\n", value);
        } break;

        case 0x0b8: /* CMP B */
        case 0x0b9: /* CMP C */
        case 0x0ba: /* CMP D */
        case 0x0bb: /* CMP E */
        case 0x0bc: /* CMP H */
        case 0x0bd: /* CMP L */
        case 0x0bf: /* CMP A */ {
            uint8_t sss = inst & 0b00000111;
            uint8_t cmp = emu->reg[A] - emu->reg[sss];
            set_flags_cmp(emu, cmp);
            emu->flags.cy = (cmp < emu->reg[sss]);
            printf("CMP %c\n", reg_lookup[sss]);
        } break;

        case 0x0be: /* CMP M */ {
            uint16_t addr = ADDR_MEM();
            uint8_t value = emu->memory[addr];
            uint8_t cmp = emu->reg[A] - value;
            set_flags_cmp(emu, cmp);
            emu->flags.cy = (emu->reg[A] < value);
            printf("CMP [0x%04x]\n", addr);
        } break;

        case 0x0fe: /* CPI data */ {
            opbytes = 2;
            uint8_t value = emu->memory[emu->pc + 1];
            uint8_t cmp = emu->reg[A] - value;
            set_flags_cmp(emu, cmp);
            emu->flags.cy = (emu->reg[A] < value);
            printf("CPI 0x%02x\n", value);
        } break;

        case 0x007: /* RLC */ {
            uint8_t A7 = (emu->reg[A] & 0x80) >> 7;
            emu->reg[A] = emu->reg[A] << 1;
            emu->reg[A] |= A7;
            emu->flags.cy = A7;
            printf("RLC\n");
        } break;

        case 0x00f: /* RRC */ {
            uint8_t A0 = (emu->reg[A] & 0x1) << 7;
            emu->reg[A] = emu->reg[A] >> 1;
            emu->reg[A] |= A0;
            emu->flags.cy = A0;
            printf("RRC\n");
        } break;

        case 0x017: /* RAL */ {
            uint8_t cy = emu->flags.cy;
            emu->flags.cy = (emu->reg[A] & 0x80) >> 7;
            emu->reg[A] = emu->reg[A] << 1;
            emu->reg[A] |= cy;
            printf("RAL\n");
        } break;

        case 0x01f: /* RAR */ {
            uint8_t cy = emu->reg[A] & 0x1;
            emu->reg[A] = emu->reg[A] >> 1;
            emu->reg[A] |= emu->flags.cy << 7;
            emu->flags.cy = cy;
            printf("RAR\n");
        } break;

        case 0x02f: /* CMA */ {
            emu->reg[A] = ~emu->reg[A];
            printf("CMA\n");
        } break;

        case 0x03f: /* CMC */ {
            emu->flags.cy = ~emu->flags.cy;
            printf("CMC\n");
        } break;

        case 0x037: /* STC */ {
            emu->flags.cy = 1;
            printf("STC\n");
        } break;


            /* BRANCH GROUP */

        case 0x0c3: /* JMP addr */ {
            opbytes = 0;
            uint16_t addr = ADDR_IMM();
            emu->pc = addr;
            printf("JMP [0x%04x]\n", emu->pc + opbytes);
        } break;

        case 0x0c2: /* JNZ  addr */
        case 0x0ca: /* JZ   addr */
        case 0x0d2: /* JNC  addr */
        case 0x0da: /* JC   addr */
        case 0x0e2: /* JPO  addr */
        case 0x0ea: /* JPE  addr */
        case 0x0f2: /* JP   addr */
        case 0x0fa: /* JM   addr */ {
            opbytes = 3;
            uint16_t addr = ADDR_IMM();
            uint8_t ccc = (inst & 0b00111000) >> 3;
            if (cond(emu, ccc)) {
                opbytes = 0;
                emu->pc = addr;
            }
            printf("J%s [0x%04x]\n", cond_lookup[ccc], addr);
        } break;

        case 0x0cd: /* CALL addr */ {
            opbytes = 0;
            PUSH(emu->pch);
            PUSH(emu->pcl);
            uint16_t addr = ADDR_IMM();
            emu->pc = addr;
            printf("CALL [0x%04x]\n", addr);
        } break;

        case 0x0c4: /* CNZ addr */
        case 0x0cc: /* CZ  addr */
        case 0x0d4: /* CNC addr */
        case 0x0dc: /* CC  addr */
        case 0x0e4: /* CPO addr */
        case 0x0ec: /* CPE addr */
        case 0x0f4: /* CP  addr */
        case 0x0fc: /* CM  addr */ {
            opbytes = 3;
            uint16_t addr = ADDR_IMM();
            uint8_t ccc = (inst & 0b00111000) >> 3;
            if (cond(emu, ccc)) {
                opbytes = 0;
                PUSH(emu->pch);
                PUSH(emu->pcl);
                emu->pc = addr;
            }
            printf("C%s [0x%04x]\n", cond_lookup[ccc], addr);
        } break;

        case 0x0c9: /* RET */ {
            opbytes = 0;
            emu->pcl = POP();
            emu->pch = POP();
            emu->pc += CALL_INST;
            printf("RET\n");
        } break;

        case 0x0c0: /* RNZ */
        case 0x0c8: /* RZ  */
        case 0x0d0: /* RNC */
        case 0x0d8: /* RC  */
        case 0x0e0: /* RPO */
        case 0x0e8: /* RPE */
        case 0x0f0: /* RP  */
        case 0x0f8: /* RM  */ {
            uint8_t ccc = (inst & 0b00111000) >> 3;
            if (cond(emu, ccc)) {
                opbytes = 0;
                emu->pcl = POP();
                emu->pch = POP();
                emu->pc += CALL_INST;
            }
            printf("R%s\n", cond_lookup[ccc]);
        } break;

        case 0x0c7: /* RST 0 */
        case 0x0cf: /* RST 1 */
        case 0x0d7: /* RST 2 */
        case 0x0df: /* RST 3 */
        case 0x0e7: /* RST 4 */
        case 0x0ef: /* RST 5 */
        case 0x0f7: /* RST 6 */
        case 0x0ff: /* RST 7 */ {
            uint8_t nnn = (inst & 0b00111000) >> 3;
            PUSH(emu->pch);
            PUSH(emu->pcl);
            emu->pc = 8 * nnn;
            printf("RST\n");
        } break;

        case 0x0e9: /* PCHL */ {
            opbytes = 0;
            emu->pc = ADDR_MEM();
            printf("PCHL\n");
        } break;


            /* STACK, I/O, MACHINE CONTROL GROUP */

        case 0x0c5: /* PUSH BC */
        case 0x0d5: /* PUSH DE */
        case 0x0e5: /* PUSH HL */ {
            uint8_t h, l;
            uint8_t rp = (inst & 0b0110000) >> 4;
            switch (rp) {
                case BC: {
                    h = emu->reg[B];
                    l = emu->reg[C];
                } break;
                case DE: {
                    h = emu->reg[D];
                    l = emu->reg[E];
                } break;
                case HL: {
                    h = emu->reg[H];
                    l = emu->reg[L];
                } break;
            }
            PUSH(h);
            PUSH(l);
            printf("PUSH %s\n", rp_lookup[rp]);
        } break;

        case 0x0f5: /* PUSH PSW */ {
            uint8_t flag_word = 0b00000010;
            flag_word |= emu->flags.cy << 0;
            flag_word |= emu->flags.p  << 2;
            flag_word |= emu->flags.ac << 4;
            flag_word |= emu->flags.z  << 6;
            flag_word |= emu->flags.s  << 7;
            PUSH(emu->reg[A]);
            PUSH(flag_word);
            printf("PUSH PSW\n");
        } break;

        case 0x0c1: /* POP BC */
        case 0x0d1: /* POP DE */
        case 0x0e1: /* POP HL */ {
            uint8_t pop_l = POP();
            uint8_t pop_h = POP();
            uint8_t rp = (inst & 0b00110000) >> 4;
            switch (rp) {
                case BC: {
                    emu->reg[B] = pop_h;
                    emu->reg[C] = pop_l;
                } break;
                case DE: {
                    emu->reg[D] = pop_h;
                    emu->reg[E] = pop_l;
                } break;
                case HL: {
                    emu->reg[H] = pop_h;
                    emu->reg[L] = pop_l;
                } break;
            }
            printf("POP %s\n", rp_lookup[rp]);
        } break;

        case 0x0f1: /* POP PSW */ {
            uint8_t flag_word = POP();
            emu->reg[A] = POP();
            emu->flags.cy = ((flag_word & 0x01) == 0x01);
            emu->flags.p  = ((flag_word & 0x04) == 0x04);
            emu->flags.ac = ((flag_word & 0x10) == 0x10);
            emu->flags.z  = ((flag_word & 0x40) == 0x40);
            emu->flags.s  = ((flag_word & 0x80) == 0x80);
            printf("POP PSW\n");
        } break;

        case 0x0e3: /* XTHL */ {
            swap(&emu->reg[L], &emu->memory[emu->sp]);
            swap(&emu->reg[H], &emu->memory[emu->sp + 1]);
            printf("XTHL\n");
        } break;

        case 0x0f9: /* SPHL */ {
            emu->sp = ADDR_MEM();
            printf("SPHL\n");
        } break;

        case 0x0db: /* IN port */ {
            opbytes = 2;
            /*  */
            printf("IN port\n");
        } break;

        case 0x0d3: /* OUT port */ {
            opbytes = 2;
            /*  */
            printf("OUT 0x%2x\n", 0x0);
        } break;

        case 0x0fb: /* EI */ {
            emu->int_enable = 1;
            printf("EI\n");
        } break;

        case 0x0f3: /* DI */ {
            emu->int_enable = 0;
            printf("DI\n");
        } break;

        case 0x076: /* HLT */ {
            printf("HLT");
            exit(0);
        } break;

        case 0x000: /* NOP */ {
            printf("NOP");
        } break;
    }

    return opbytes;
}

