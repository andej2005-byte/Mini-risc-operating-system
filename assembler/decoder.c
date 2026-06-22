#include "decoder.h"
#include "../display/screen.h"

static void print_uint(unsigned int value) {
    char buffer[16];
    int i = 0;
    int j;

    if (value == 0) {
        print_char('0', -1, -1);
        return;
    }

    while (value > 0) {
        buffer[i] = '0' + (value % 10);
        value = value / 10;
        i++;
    }

    for (j = i - 1; j >= 0; j--) {
        print_char(buffer[j], -1, -1);
    }
}

static void print_int(int value) {
    if (value < 0) {
        print_char('-', -1, -1);
        value = -value;
    }

    print_uint((unsigned int)value);
}

static void print_reg(unsigned int reg) {
    print_char('x', -1, -1);
    print_uint(reg);
}

static int sign_extend(unsigned int value, int bits) {
    unsigned int sign_bit = 1 << (bits - 1);

    if (value & sign_bit) {
        value = value | (~((1 << bits) - 1));
    }

    return (int)value;
}

static void print_r_type(char* name, u32 rd, u32 rs1, u32 rs2) {
    print_string(name, -1, -1);
    print_char(' ', -1, -1);
    print_reg(rd);
    print_string(", ", -1, -1);
    print_reg(rs1);
    print_string(", ", -1, -1);
    print_reg(rs2);
    print_string("\n", -1, -1);
}

static void print_i_type(char* name, u32 rd, u32 rs1, int imm) {
    print_string(name, -1, -1);
    print_char(' ', -1, -1);
    print_reg(rd);
    print_string(", ", -1, -1);
    print_reg(rs1);
    print_string(", ", -1, -1);
    print_int(imm);
    print_string("\n", -1, -1);
}

static void print_load_type(char* name, u32 rd, u32 rs1, int imm) {
    print_string(name, -1, -1);
    print_char(' ', -1, -1);
    print_reg(rd);
    print_string(", ", -1, -1);
    print_int(imm);
    print_char('(', -1, -1);
    print_reg(rs1);
    print_string(")\n", -1, -1);
}

static void print_store_type(char* name, u32 rs2, u32 rs1, int imm) {
    print_string(name, -1, -1);
    print_char(' ', -1, -1);
    print_reg(rs2);
    print_string(", ", -1, -1);
    print_int(imm);
    print_char('(', -1, -1);
    print_reg(rs1);
    print_string(")\n", -1, -1);
}

static void print_branch_type(char* name, u32 rs1, u32 rs2, int imm) {
    print_string(name, -1, -1);
    print_char(' ', -1, -1);
    print_reg(rs1);
    print_string(", ", -1, -1);
    print_reg(rs2);
    print_string(", ", -1, -1);
    print_int(imm);
    print_string("\n", -1, -1);
}

static void print_u_type(char* name, u32 rd, int imm) {
    print_string(name, -1, -1);
    print_char(' ', -1, -1);
    print_reg(rd);
    print_string(", ", -1, -1);
    print_int(imm);
    print_string("\n", -1, -1);
}

void decode_and_print(u32 instruction) {
    u32 opcode = instruction & 0x7F;
    u32 rd     = (instruction >> 7)  & 0x1F;
    u32 funct3 = (instruction >> 12) & 0x07;
    u32 rs1    = (instruction >> 15) & 0x1F;
    u32 rs2    = (instruction >> 20) & 0x1F;
    u32 funct7 = (instruction >> 25) & 0x7F;

    if (opcode == 0x37) {
        int imm = instruction & 0xFFFFF000;
        print_u_type("lui", rd, imm);
        return;
    }

    if (opcode == 0x17) {
        int imm = instruction & 0xFFFFF000;
        print_u_type("auipc", rd, imm);
        return;
    }

    if (opcode == 0x33) {
        if (funct3 == 0x00 && funct7 == 0x00) {
            print_r_type("add", rd, rs1, rs2);
            return;
        }

        if (funct3 == 0x00 && funct7 == 0x20) {
            print_r_type("sub", rd, rs1, rs2);
            return;
        }

        if (funct3 == 0x01 && funct7 == 0x00) {
            print_r_type("sll", rd, rs1, rs2);
            return;
        }

        if (funct3 == 0x02 && funct7 == 0x00) {
            print_r_type("slt", rd, rs1, rs2);
            return;
        }

        if (funct3 == 0x03 && funct7 == 0x00) {
            print_r_type("sltu", rd, rs1, rs2);
            return;
        }

        if (funct3 == 0x04 && funct7 == 0x00) {
            print_r_type("xor", rd, rs1, rs2);
            return;
        }

        if (funct3 == 0x05 && funct7 == 0x00) {
            print_r_type("srl", rd, rs1, rs2);
            return;
        }

        if (funct3 == 0x05 && funct7 == 0x20) {
            print_r_type("sra", rd, rs1, rs2);
            return;
        }

        if (funct3 == 0x06 && funct7 == 0x00) {
            print_r_type("or", rd, rs1, rs2);
            return;
        }

        if (funct3 == 0x07 && funct7 == 0x00) {
            print_r_type("and", rd, rs1, rs2);
            return;
        }
    }

    if (opcode == 0x13) {
        int imm = sign_extend((instruction >> 20) & 0xFFF, 12);
        u32 shamt = (instruction >> 20) & 0x1F;

        if (funct3 == 0x00) {
            print_i_type("addi", rd, rs1, imm);
            return;
        }

        if (funct3 == 0x02) {
            print_i_type("slti", rd, rs1, imm);
            return;
        }

        if (funct3 == 0x03) {
            print_i_type("sltiu", rd, rs1, imm);
            return;
        }

        if (funct3 == 0x04) {
            print_i_type("xori", rd, rs1, imm);
            return;
        }

        if (funct3 == 0x06) {
            print_i_type("ori", rd, rs1, imm);
            return;
        }

        if (funct3 == 0x07) {
            print_i_type("andi", rd, rs1, imm);
            return;
        }

        if (funct3 == 0x01 && funct7 == 0x00) {
            print_i_type("slli", rd, rs1, shamt);
            return;
        }

        if (funct3 == 0x05 && funct7 == 0x00) {
            print_i_type("srli", rd, rs1, shamt);
            return;
        }

        if (funct3 == 0x05 && funct7 == 0x20) {
            print_i_type("srai", rd, rs1, shamt);
            return;
        }
    }

    if (opcode == 0x03) {
        int imm = sign_extend((instruction >> 20) & 0xFFF, 12);

        if (funct3 == 0x00) {
            print_load_type("lb", rd, rs1, imm);
            return;
        }

        if (funct3 == 0x01) {
            print_load_type("lh", rd, rs1, imm);
            return;
        }

        if (funct3 == 0x02) {
            print_load_type("lw", rd, rs1, imm);
            return;
        }

        if (funct3 == 0x04) {
            print_load_type("lbu", rd, rs1, imm);
            return;
        }

        if (funct3 == 0x05) {
            print_load_type("lhu", rd, rs1, imm);
            return;
        }
    }

    if (opcode == 0x23) {
        u32 imm_11_5 = (instruction >> 25) & 0x7F;
        u32 imm_4_0  = (instruction >> 7) & 0x1F;
        int imm = sign_extend((imm_11_5 << 5) | imm_4_0, 12);

        if (funct3 == 0x00) {
            print_store_type("sb", rs2, rs1, imm);
            return;
        }

        if (funct3 == 0x01) {
            print_store_type("sh", rs2, rs1, imm);
            return;
        }

        if (funct3 == 0x02) {
            print_store_type("sw", rs2, rs1, imm);
            return;
        }
    }

    if (opcode == 0x63) {
        u32 imm_12   = (instruction >> 31) & 0x01;
        u32 imm_11   = (instruction >> 7)  & 0x01;
        u32 imm_10_5 = (instruction >> 25) & 0x3F;
        u32 imm_4_1  = (instruction >> 8)  & 0x0F;

        int imm = sign_extend(
            (imm_12 << 12) |
            (imm_11 << 11) |
            (imm_10_5 << 5) |
            (imm_4_1 << 1),
            13
        );

        if (funct3 == 0x00) {
            print_branch_type("beq", rs1, rs2, imm);
            return;
        }

        if (funct3 == 0x01) {
            print_branch_type("bne", rs1, rs2, imm);
            return;
        }

        if (funct3 == 0x04) {
            print_branch_type("blt", rs1, rs2, imm);
            return;
        }

        if (funct3 == 0x05) {
            print_branch_type("bge", rs1, rs2, imm);
            return;
        }

        if (funct3 == 0x06) {
            print_branch_type("bltu", rs1, rs2, imm);
            return;
        }

        if (funct3 == 0x07) {
            print_branch_type("bgeu", rs1, rs2, imm);
            return;
        }
    }

    if (opcode == 0x6F) {
        u32 imm_20    = (instruction >> 31) & 0x01;
        u32 imm_10_1  = (instruction >> 21) & 0x3FF;
        u32 imm_11    = (instruction >> 20) & 0x01;
        u32 imm_19_12 = (instruction >> 12) & 0xFF;

        int imm = sign_extend(
            (imm_20 << 20) |
            (imm_19_12 << 12) |
            (imm_11 << 11) |
            (imm_10_1 << 1),
            21
        );

        print_string("jal ", -1, -1);
        print_reg(rd);
        print_string(", ", -1, -1);
        print_int(imm);
        print_string("\n", -1, -1);
        return;
    }

    if (opcode == 0x67 && funct3 == 0x00) {
        int imm = sign_extend((instruction >> 20) & 0xFFF, 12);

        print_load_type("jalr", rd, rs1, imm);
        return;
    }

    print_string("Unknown instruction\n", -1, -1);
}

