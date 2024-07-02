#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#define MAX_LABELS 256

#define MOV_REG_IMM 0xB8
#define ADD_REG_REG 0x01
#define SUB_REG_IMM 0x81
#define CALL_REL 0xE8
#define INT 0xCD
#define PUSH_REG 0x50
#define CMP_REG_REG 0x39
#define JMP_REL 0xE9
#define JE_REL 0x84
#define JNE_REL 0x85
#define JG_REL 0x8F
#define JGE_REL 0x8D
#define JL_REL 0x8C
#define JLE_REL 0x8E

#define EAX 0
#define ECX 1
#define EDX 2
#define EBX 3
#define ESP 4
#define EBP 5
#define ESI 6
#define EDI 7
#define RAX 0
#define RCX 1
#define RDX 2
#define RBX 3
#define RSP 4
#define RBP 5
#define RSI 6
#define RDI 7
#define R10 10
#define R11 11

typedef struct {
    char label[32];
    int address;
} Label;

Label labels[MAX_LABELS];
int label_count = 0;
int instruction_count = 0;

unsigned char reg_encode(const char* reg_name) {
    if (strncmp(reg_name, "eax", 3) == 0) {
        return EAX;
    } else if (strncmp(reg_name, "ecx", 3) == 0) {
        return ECX;
    } else if (strncmp(reg_name, "edx", 3) == 0) {
        return EDX;
    } else if (strncmp(reg_name, "ebx", 3) == 0) {
        return EBX;
    } else if (strncmp(reg_name, "esp", 3) == 0) {
        return ESP;
    } else if (strncmp(reg_name, "ebp", 3) == 0) {
        return EBP;
    } else if (strncmp(reg_name, "esi", 3) == 0) {
        return ESI;
    } else if (strncmp(reg_name, "edi", 3) == 0) {
        return EDI;
    } else if (strncmp(reg_name, "rax", 3) == 0) {
        return RAX;
    } else if (strncmp(reg_name, "rcx", 3) == 0) {
        return RCX;
    } else if (strncmp(reg_name, "rdx", 3) == 0) {
        return RDX;
    } else if (strncmp(reg_name, "rbx", 3) == 0) {
        return RBX;
    } else if (strncmp(reg_name, "rsp", 3) == 0) {
        return RSP;
    } else if (strncmp(reg_name, "rbp", 3) == 0) {
        return RBP;
    } else if (strncmp(reg_name, "rsi", 3) == 0) {
        return RSI;
    } else if (strncmp(reg_name, "rdi", 3) == 0) {
        return RDI;
    } else if (strncmp(reg_name, "r10", 3) == 0) {
        return R10;
    } else if (strncmp(reg_name, "r11", 3) == 0) {
        return R11;
    }
    return 0xFF;
}

void add_label(const char* label, int address) {
    if (label_count < MAX_LABELS) {
        strcpy(labels[label_count].label, label);
        labels[label_count].address = address;
        label_count++;
    } else {
        printf("Error: Too many labels\n");
    }
}

int get_label_address(const char* label) {
    for (int i = 0; i < label_count; i++) {
        if (strcmp(labels[i].label, label) == 0) {
            return labels[i].address;
        }
    }
    return -1;
}

int convert_instruction(const char* instruction, unsigned char* machine_code) {
    char op[6], reg1[5], reg2[5], label[32];
    int imm_value;
    int len ;

    while (isspace(*instruction)) {
        instruction++;
    }

    if (sscanf(instruction, "mov %3s, %x", reg1, &imm_value) == 2) {
        machine_code[0] = MOV_REG_IMM | reg_encode(reg1);
        memcpy(machine_code + 1, &imm_value, 4);
        len = 5;
    } else if (sscanf(instruction, "mov %3s, %3s", reg1, reg2) == 2) {
        machine_code[0] = MOV_REG_IMM | reg_encode(reg1);
        memcpy(machine_code + 1, &imm_value, 4);
        len = 5;
    } else if (sscanf(instruction, "add %3s, %4s", reg1, reg2) == 2) {
        machine_code[0] = ADD_REG_REG;
        machine_code[1] = (reg_encode(reg2) << 3) | reg_encode(reg1);
        len = 2;
    } else if (sscanf(instruction, "sub %3s, %x", reg1, &imm_value) == 2) {
        machine_code[0] = SUB_REG_IMM;
        machine_code[1] = 0xE8 | reg_encode(reg1);
        memcpy(machine_code + 2, &imm_value, 4);
        len = 6;
    } else if (sscanf(instruction, "push %4s", reg1) == 1) {
        machine_code[0] = PUSH_REG | reg_encode(reg1);
        len = 1;
    } else if (sscanf(instruction, "cmp %3s, %4s", reg1, reg2) == 2) {
        machine_code[0] = CMP_REG_REG;
        machine_code[1] = (reg_encode(reg2) << 3) | reg_encode(reg1);
        len = 2;
    } else if (sscanf(instruction, "jmp %s", label) == 1) {
        machine_code[0] = JMP_REL;
        int addr = get_label_address(label);
        if (addr >= 0) {
            int rel_addr = addr - (instruction_count * 5) - 5;
            memcpy(machine_code + 1, &rel_addr, 4);
        } else {
            memset(machine_code + 1, 0, 4);
        }
        len = 5;
    } else if (sscanf(instruction, "call %s", label) == 1) {
        machine_code[0] = CALL_REL;
        int addr = get_label_address(label);
        if (addr >= 0) {
            int rel_addr = addr - (instruction_count * 5) - 5;
            memcpy(machine_code + 1, &rel_addr, 4);
        } else {
            memset(machine_code + 1, 0, 4);
        }
        len = 5;
    } else if (sscanf(instruction, "je %s", label) == 1) {
        machine_code[0] = JE_REL;
        int addr = get_label_address(label);
        if (addr >= 0) {
            int rel_addr = addr - (instruction_count * 6) - 6;
            memcpy(machine_code + 1, &rel_addr, 4);
        } else {
            memset(machine_code + 1, 0, 4);
        }
        len = 6;
    } else if (sscanf(instruction, "jne %s", label) == 1) {
        machine_code[0] = JNE_REL;
        int addr = get_label_address(label);
        if (addr >= 0) {
            int rel_addr = addr - (instruction_count * 6) - 6;
            memcpy(machine_code + 1, &rel_addr, 4);
        } else {
            memset(machine_code + 1, 0, 4);
        }
        len = 6;
    }else if (sscanf(instruction, "jg %s", label) == 1) {
        machine_code[0] = JG_REL;
        int addr = get_label_address(label);
        if (addr >= 0) {
            int rel_addr = addr - (instruction_count * 6) - 6;
            memcpy(machine_code + 1, &rel_addr, 4);
        } else {
            memset(machine_code + 1, 0, 4);
        }
        len = 6;
    }else if (sscanf(instruction, "jge %s", label) == 1) {
        machine_code[0] = JGE_REL;
        int addr = get_label_address(label);
        if (addr >= 0) {
            int rel_addr = addr - (instruction_count * 6) - 6;
            memcpy(machine_code + 1, &rel_addr, 4);
        } else {
            memset(machine_code + 1, 0, 4);
        }
        len = 6;
    }else if (sscanf(instruction, "jl %s", label) == 1) {
        machine_code[0] = JL_REL;
        int addr = get_label_address(label);
        if (addr >= 0) {
            int rel_addr = addr - (instruction_count * 6) - 6;
            memcpy(machine_code + 1, &rel_addr, 4);
        } else {
            memset(machine_code + 1, 0, 4);
        }
        len = 6;
    }else if (sscanf(instruction, "jle %s", label) == 1) {
        machine_code[0] = JLE_REL;
        int addr = get_label_address(label);
        if (addr >= 0) {
            int rel_addr = addr - (instruction_count * 6) - 6;
            memcpy(machine_code + 1, &rel_addr, 4);
        } else {
            memset(machine_code + 1, 0, 4);
        }
        len = 6;
    } else if (sscanf(instruction, "int %x", &imm_value) == 1) {
        machine_code[0] = INT;
        machine_code[1] = imm_value;
        len = 2;
    } else if (sscanf(instruction, "%31s:", label) == 1) {
        add_label(label, instruction_count * 8);
        len = 0;
    } else {
        printf("Unsupported instruction: %s\n", instruction);
        len = 0;
    }

    return len;
}


int start_assemble() {
    FILE* input_file = fopen("test.asm", "r");
    if (input_file == NULL) {
        printf("Error: Failed to open input file %s\n", "test.asm");
        return 1;
    }

    char instruction[32];
    unsigned char machine_code[8];
    int len;
    int skip = 0;
    while (fgets(instruction, sizeof(instruction), input_file) != NULL) {
        if (strcspn(instruction, "\n") != 0){
            len = convert_instruction(instruction, machine_code);

            if (len > 0) {
                printf("Instruction: %s\n", instruction);
                printf("Machine code: ");
                for (int i = 0; i < len; i++) {
                    printf("%02X ", machine_code[i]);
                }
                printf("\n\n");
            }
        }

    }
    fclose(input_file);
    return 0;

}
