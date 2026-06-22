#include "assembler.h"
#include "parser.h"
#include "encoder.h"

static int is_separator(char c) {
    return c == ' ' || c == '\t' || c == ',' || c == '(' || c == ')';
}

static int str_equal(char* a, char* b) {
    int i = 0;

    while (a[i] != 0 && b[i] != 0) {
        if (a[i] != b[i]) {
            return 0;
        }

        i++;
    }

    return a[i] == 0 && b[i] == 0;
}

static void copy_string(char* dest, char* src, int max) {
    int i = 0;

    while (src[i] != 0 && i < max - 1) {
        dest[i] = src[i];
        i++;
    }

    dest[i] = 0;
}

static int next_token(char** p, char* out, int max) {
    int i = 0;

    while (**p != 0 && is_separator(**p)) {
        (*p)++;
    }

    while (**p != 0 && !is_separator(**p)) {
        if (i >= max - 1) {
            return 0;
        }

        out[i] = **p;
        i++;
        (*p)++;
    }

    out[i] = 0;

    return i > 0;
}

static int parse_r(char* a, char* b, char* c, int* rd, int* rs1, int* rs2) {
    *rd = parse_register(a);
    *rs1 = parse_register(b);
    *rs2 = parse_register(c);

    if (*rd < 0 || *rs1 < 0 || *rs2 < 0) {
        return -1;
    }

    return 0;
}

static int parse_i(char* a, char* b, char* c, int* rd, int* rs1, int* imm) {
    *rd = parse_register(a);
    *rs1 = parse_register(b);
    *imm = parse_int(c);

    if (*rd < 0 || *rs1 < 0) {
        return -1;
    }

    return 0;
}

static int parse_load(char* a, char* b, char* c, int* rd, int* rs1, int* imm) {
    *rd = parse_register(a);
    *imm = parse_int(b);
    *rs1 = parse_register(c);

    if (*rd < 0 || *rs1 < 0) {
        return -1;
    }

    return 0;
}

static int parse_store(char* a, char* b, char* c, int* rs2, int* rs1, int* imm) {
    *rs2 = parse_register(a);
    *imm = parse_int(b);
    *rs1 = parse_register(c);

    if (*rs2 < 0 || *rs1 < 0) {
        return -1;
    }

    return 0;
}

static int parse_branch(char* a, char* b, char* c, int* rs1, int* rs2, int* imm) {
    *rs1 = parse_register(a);
    *rs2 = parse_register(b);
    *imm = parse_int(c);

    if (*rs1 < 0 || *rs2 < 0) {
        return -1;
    }

    return 0;
}

int assemble_line(char* line, u32* out_instruction) {
    char buffer[80];
    char* p;

    char op[16];
    char a[16];
    char b[16];
    char c[16];

    int rd;
    int rs1;
    int rs2;
    int imm;

    copy_string(buffer, line, 80);
    p = buffer;

    if (!next_token(&p, op, 16)) {
        return -1;
    }

    if (str_equal(op, "lui")) {
        if (!next_token(&p, a, 16)) return -1;
        if (!next_token(&p, b, 16)) return -1;

        rd = parse_register(a);
        imm = parse_int(b);

        if (rd < 0) return -1;

        *out_instruction = encode_lui(rd, imm);
        return 0;
    }

    if (str_equal(op, "auipc")) {
        if (!next_token(&p, a, 16)) return -1;
        if (!next_token(&p, b, 16)) return -1;

        rd = parse_register(a);
        imm = parse_int(b);

        if (rd < 0) return -1;

        *out_instruction = encode_auipc(rd, imm);
        return 0;
    }

    if (str_equal(op, "jal")) {
        if (!next_token(&p, a, 16)) return -1;
        if (!next_token(&p, b, 16)) return -1;

        rd = parse_register(a);
        imm = parse_int(b);

        if (rd < 0) return -1;

        *out_instruction = encode_jal(rd, imm);
        return 0;
    }

    if (str_equal(op, "jalr")) {
        if (!next_token(&p, a, 16)) return -1;
        if (!next_token(&p, b, 16)) return -1;
        if (!next_token(&p, c, 16)) return -1;

        if (parse_load(a, b, c, &rd, &rs1, &imm) < 0) return -1;

        *out_instruction = encode_jalr(rd, rs1, imm);
        return 0;
    }

    if (str_equal(op, "beq")) {
        if (!next_token(&p, a, 16)) return -1;
        if (!next_token(&p, b, 16)) return -1;
        if (!next_token(&p, c, 16)) return -1;

        if (parse_branch(a, b, c, &rs1, &rs2, &imm) < 0) return -1;

        *out_instruction = encode_beq(rs1, rs2, imm);
        return 0;
    }

    if (str_equal(op, "bne")) {
        if (!next_token(&p, a, 16)) return -1;
        if (!next_token(&p, b, 16)) return -1;
        if (!next_token(&p, c, 16)) return -1;

        if (parse_branch(a, b, c, &rs1, &rs2, &imm) < 0) return -1;

        *out_instruction = encode_bne(rs1, rs2, imm);
        return 0;
    }

    if (str_equal(op, "blt")) {
        if (!next_token(&p, a, 16)) return -1;
        if (!next_token(&p, b, 16)) return -1;
        if (!next_token(&p, c, 16)) return -1;

        if (parse_branch(a, b, c, &rs1, &rs2, &imm) < 0) return -1;

        *out_instruction = encode_blt(rs1, rs2, imm);
        return 0;
    }

    if (str_equal(op, "bge")) {
        if (!next_token(&p, a, 16)) return -1;
        if (!next_token(&p, b, 16)) return -1;
        if (!next_token(&p, c, 16)) return -1;

        if (parse_branch(a, b, c, &rs1, &rs2, &imm) < 0) return -1;

        *out_instruction = encode_bge(rs1, rs2, imm);
        return 0;
    }

    if (str_equal(op, "bltu")) {
        if (!next_token(&p, a, 16)) return -1;
        if (!next_token(&p, b, 16)) return -1;
        if (!next_token(&p, c, 16)) return -1;

        if (parse_branch(a, b, c, &rs1, &rs2, &imm) < 0) return -1;

        *out_instruction = encode_bltu(rs1, rs2, imm);
        return 0;
    }

    if (str_equal(op, "bgeu")) {
        if (!next_token(&p, a, 16)) return -1;
        if (!next_token(&p, b, 16)) return -1;
        if (!next_token(&p, c, 16)) return -1;

        if (parse_branch(a, b, c, &rs1, &rs2, &imm) < 0) return -1;

        *out_instruction = encode_bgeu(rs1, rs2, imm);
        return 0;
    }

    if (str_equal(op, "lb") || str_equal(op, "lh") || str_equal(op, "lw") ||
        str_equal(op, "lbu") || str_equal(op, "lhu")) {

        if (!next_token(&p, a, 16)) return -1;
        if (!next_token(&p, b, 16)) return -1;
        if (!next_token(&p, c, 16)) return -1;

        if (parse_load(a, b, c, &rd, &rs1, &imm) < 0) return -1;

        if (str_equal(op, "lb"))  *out_instruction = encode_lb(rd, rs1, imm);
        if (str_equal(op, "lh"))  *out_instruction = encode_lh(rd, rs1, imm);
        if (str_equal(op, "lw"))  *out_instruction = encode_lw(rd, rs1, imm);
        if (str_equal(op, "lbu")) *out_instruction = encode_lbu(rd, rs1, imm);
        if (str_equal(op, "lhu")) *out_instruction = encode_lhu(rd, rs1, imm);

        return 0;
    }

    if (str_equal(op, "sb") || str_equal(op, "sh") || str_equal(op, "sw")) {
        if (!next_token(&p, a, 16)) return -1;
        if (!next_token(&p, b, 16)) return -1;
        if (!next_token(&p, c, 16)) return -1;

        if (parse_store(a, b, c, &rs2, &rs1, &imm) < 0) return -1;

        if (str_equal(op, "sb")) *out_instruction = encode_sb(rs2, rs1, imm);
        if (str_equal(op, "sh")) *out_instruction = encode_sh(rs2, rs1, imm);
        if (str_equal(op, "sw")) *out_instruction = encode_sw(rs2, rs1, imm);

        return 0;
    }

    if (str_equal(op, "addi") || str_equal(op, "slti") || str_equal(op, "sltiu") ||
        str_equal(op, "xori") || str_equal(op, "ori") || str_equal(op, "andi") ||
        str_equal(op, "slli") || str_equal(op, "srli") || str_equal(op, "srai")) {

        if (!next_token(&p, a, 16)) return -1;
        if (!next_token(&p, b, 16)) return -1;
        if (!next_token(&p, c, 16)) return -1;

        if (parse_i(a, b, c, &rd, &rs1, &imm) < 0) return -1;

        if (str_equal(op, "addi"))  *out_instruction = encode_addi(rd, rs1, imm);
        if (str_equal(op, "slti"))  *out_instruction = encode_slti(rd, rs1, imm);
        if (str_equal(op, "sltiu")) *out_instruction = encode_sltiu(rd, rs1, imm);
        if (str_equal(op, "xori"))  *out_instruction = encode_xori(rd, rs1, imm);
        if (str_equal(op, "ori"))   *out_instruction = encode_ori(rd, rs1, imm);
        if (str_equal(op, "andi"))  *out_instruction = encode_andi(rd, rs1, imm);
        if (str_equal(op, "slli"))  *out_instruction = encode_slli(rd, rs1, imm);
        if (str_equal(op, "srli"))  *out_instruction = encode_srli(rd, rs1, imm);
        if (str_equal(op, "srai"))  *out_instruction = encode_srai(rd, rs1, imm);

        return 0;
    }

    if (str_equal(op, "add") || str_equal(op, "sub") || str_equal(op, "sll") ||
        str_equal(op, "slt") || str_equal(op, "sltu") || str_equal(op, "xor") ||
        str_equal(op, "srl") || str_equal(op, "sra") || str_equal(op, "or") ||
        str_equal(op, "and")) {

        if (!next_token(&p, a, 16)) return -1;
        if (!next_token(&p, b, 16)) return -1;
        if (!next_token(&p, c, 16)) return -1;

        if (parse_r(a, b, c, &rd, &rs1, &rs2) < 0) return -1;

        if (str_equal(op, "add"))  *out_instruction = encode_add(rd, rs1, rs2);
        if (str_equal(op, "sub"))  *out_instruction = encode_sub(rd, rs1, rs2);
        if (str_equal(op, "sll"))  *out_instruction = encode_sll(rd, rs1, rs2);
        if (str_equal(op, "slt"))  *out_instruction = encode_slt(rd, rs1, rs2);
        if (str_equal(op, "sltu")) *out_instruction = encode_sltu(rd, rs1, rs2);
        if (str_equal(op, "xor"))  *out_instruction = encode_xor(rd, rs1, rs2);
        if (str_equal(op, "srl"))  *out_instruction = encode_srl(rd, rs1, rs2);
        if (str_equal(op, "sra"))  *out_instruction = encode_sra(rd, rs1, rs2);
        if (str_equal(op, "or"))   *out_instruction = encode_or(rd, rs1, rs2);
        if (str_equal(op, "and"))  *out_instruction = encode_and(rd, rs1, rs2);

        return 0;
    }

    return -1;
}
