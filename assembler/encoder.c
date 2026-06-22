#include "encoder.h"

u32 encode_r_type(u32 funct7, u32 rs2, u32 rs1, u32 funct3, u32 rd, u32 opcode) {
    return ((funct7 & 0x7F) << 25) |
           ((rs2    & 0x1F) << 20) |
           ((rs1    & 0x1F) << 15) |
           ((funct3 & 0x07) << 12) |
           ((rd     & 0x1F) << 7)  |
           (opcode  & 0x7F);
}

u32 encode_i_type(int imm, u32 rs1, u32 funct3, u32 rd, u32 opcode) {
    return ((imm    & 0xFFF) << 20) |
           ((rs1    & 0x1F)  << 15) |
           ((funct3 & 0x07)  << 12) |
           ((rd     & 0x1F)  << 7)  |
           (opcode  & 0x7F);
}

u32 encode_s_type(int imm, u32 rs2, u32 rs1, u32 funct3, u32 opcode) {
    u32 imm_11_5 = ((u32)imm >> 5) & 0x7F;
    u32 imm_4_0  = (u32)imm & 0x1F;

    return (imm_11_5 << 25) |
           ((rs2    & 0x1F) << 20) |
           ((rs1    & 0x1F) << 15) |
           ((funct3 & 0x07) << 12) |
           (imm_4_0 << 7) |
           (opcode  & 0x7F);
}

u32 encode_b_type(int imm, u32 rs2, u32 rs1, u32 funct3, u32 opcode) {
    u32 imm_u = (u32)imm;

    u32 imm_12   = (imm_u >> 12) & 0x01;
    u32 imm_10_5 = (imm_u >> 5)  & 0x3F;
    u32 imm_4_1  = (imm_u >> 1)  & 0x0F;
    u32 imm_11   = (imm_u >> 11) & 0x01;

    return (imm_12 << 31) |
           (imm_10_5 << 25) |
           ((rs2    & 0x1F) << 20) |
           ((rs1    & 0x1F) << 15) |
           ((funct3 & 0x07) << 12) |
           (imm_4_1 << 8) |
           (imm_11 << 7) |
           (opcode & 0x7F);
}

u32 encode_u_type(int imm, u32 rd, u32 opcode) {
    return ((u32)imm & 0xFFFFF000) |
           ((rd & 0x1F) << 7) |
           (opcode & 0x7F);
}

u32 encode_j_type(int imm, u32 rd, u32 opcode) {
    u32 imm_u = (u32)imm;

    u32 imm_20    = (imm_u >> 20) & 0x01;
    u32 imm_10_1  = (imm_u >> 1)  & 0x3FF;
    u32 imm_11    = (imm_u >> 11) & 0x01;
    u32 imm_19_12 = (imm_u >> 12) & 0xFF;

    return (imm_20 << 31) |
           (imm_10_1 << 21) |
           (imm_11 << 20) |
           (imm_19_12 << 12) |
           ((rd & 0x1F) << 7) |
           (opcode & 0x7F);
}

u32 encode_lui(int rd, int imm) { return encode_u_type(imm, rd, 0x37); }
u32 encode_auipc(int rd, int imm) { return encode_u_type(imm, rd, 0x17); }

u32 encode_jal(int rd, int imm) { return encode_j_type(imm, rd, 0x6F); }
u32 encode_jalr(int rd, int rs1, int imm) { return encode_i_type(imm, rs1, 0x00, rd, 0x67); }

u32 encode_beq(int rs1, int rs2, int imm) { return encode_b_type(imm, rs2, rs1, 0x00, 0x63); }
u32 encode_bne(int rs1, int rs2, int imm) { return encode_b_type(imm, rs2, rs1, 0x01, 0x63); }
u32 encode_blt(int rs1, int rs2, int imm) { return encode_b_type(imm, rs2, rs1, 0x04, 0x63); }
u32 encode_bge(int rs1, int rs2, int imm) { return encode_b_type(imm, rs2, rs1, 0x05, 0x63); }
u32 encode_bltu(int rs1, int rs2, int imm) { return encode_b_type(imm, rs2, rs1, 0x06, 0x63); }
u32 encode_bgeu(int rs1, int rs2, int imm) { return encode_b_type(imm, rs2, rs1, 0x07, 0x63); }

u32 encode_lb(int rd, int rs1, int imm) { return encode_i_type(imm, rs1, 0x00, rd, 0x03); }
u32 encode_lh(int rd, int rs1, int imm) { return encode_i_type(imm, rs1, 0x01, rd, 0x03); }
u32 encode_lw(int rd, int rs1, int imm) { return encode_i_type(imm, rs1, 0x02, rd, 0x03); }
u32 encode_lbu(int rd, int rs1, int imm) { return encode_i_type(imm, rs1, 0x04, rd, 0x03); }
u32 encode_lhu(int rd, int rs1, int imm) { return encode_i_type(imm, rs1, 0x05, rd, 0x03); }

u32 encode_sb(int rs2, int rs1, int imm) { return encode_s_type(imm, rs2, rs1, 0x00, 0x23); }
u32 encode_sh(int rs2, int rs1, int imm) { return encode_s_type(imm, rs2, rs1, 0x01, 0x23); }
u32 encode_sw(int rs2, int rs1, int imm) { return encode_s_type(imm, rs2, rs1, 0x02, 0x23); }

u32 encode_addi(int rd, int rs1, int imm) { return encode_i_type(imm, rs1, 0x00, rd, 0x13); }
u32 encode_slti(int rd, int rs1, int imm) { return encode_i_type(imm, rs1, 0x02, rd, 0x13); }
u32 encode_sltiu(int rd, int rs1, int imm) { return encode_i_type(imm, rs1, 0x03, rd, 0x13); }
u32 encode_xori(int rd, int rs1, int imm) { return encode_i_type(imm, rs1, 0x04, rd, 0x13); }
u32 encode_ori(int rd, int rs1, int imm) { return encode_i_type(imm, rs1, 0x06, rd, 0x13); }
u32 encode_andi(int rd, int rs1, int imm) { return encode_i_type(imm, rs1, 0x07, rd, 0x13); }

u32 encode_slli(int rd, int rs1, int shamt) {
    return encode_i_type(shamt & 0x1F, rs1, 0x01, rd, 0x13);
}

u32 encode_srli(int rd, int rs1, int shamt) {
    return encode_i_type(shamt & 0x1F, rs1, 0x05, rd, 0x13);
}

u32 encode_srai(int rd, int rs1, int shamt) {
    return encode_i_type(0x400 | (shamt & 0x1F), rs1, 0x05, rd, 0x13);
}

u32 encode_add(int rd, int rs1, int rs2) { return encode_r_type(0x00, rs2, rs1, 0x00, rd, 0x33); }
u32 encode_sub(int rd, int rs1, int rs2) { return encode_r_type(0x20, rs2, rs1, 0x00, rd, 0x33); }
u32 encode_sll(int rd, int rs1, int rs2) { return encode_r_type(0x00, rs2, rs1, 0x01, rd, 0x33); }
u32 encode_slt(int rd, int rs1, int rs2) { return encode_r_type(0x00, rs2, rs1, 0x02, rd, 0x33); }
u32 encode_sltu(int rd, int rs1, int rs2) { return encode_r_type(0x00, rs2, rs1, 0x03, rd, 0x33); }
u32 encode_xor(int rd, int rs1, int rs2) { return encode_r_type(0x00, rs2, rs1, 0x04, rd, 0x33); }
u32 encode_srl(int rd, int rs1, int rs2) { return encode_r_type(0x00, rs2, rs1, 0x05, rd, 0x33); }
u32 encode_sra(int rd, int rs1, int rs2) { return encode_r_type(0x20, rs2, rs1, 0x05, rd, 0x33); }
u32 encode_or(int rd, int rs1, int rs2) { return encode_r_type(0x00, rs2, rs1, 0x06, rd, 0x33); }
u32 encode_and(int rd, int rs1, int rs2) { return encode_r_type(0x00, rs2, rs1, 0x07, rd, 0x33); }

