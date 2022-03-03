#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "emu8080c.h"


void load_file(struct Emu8080c* emu, char* filename, uint32_t offset) {
    FILE* file = fopen(filename, "rb");

    if (!file) {
        printf("ERROR -- FILE: %s\n", filename);
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    fseek(file, 0, SEEK_SET);

    uint8_t* buffer = &emu->memory[offset];
    fread(buffer, size, 1, file);

    fclose(file);
}

int main() {
    struct Emu8080c emu = {0};
    emu.memory = (uint8_t*)malloc(0x10000);

    load_file(&emu, "..\\bin\\cpudiag.bin", 0x100);

    emu.pc = 0x100;
    for (;;) {
        emu.pc += decode(&emu);
        if (emu.pc == 0x5) exit(0);
    }

    return 0;
}
