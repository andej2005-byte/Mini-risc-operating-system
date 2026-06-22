#ifndef ENCODER_H
#define ENCODER_H

typedef unsigned int u32;

u32 encode_r_type(u32 funct7, u32 rs2, u32 rs1, u32 funct3, u32 rd, u32 opcode);
u32 encode_i_type(int imm, u32 rs1, u32 funct3, u32 rd, u32 opcode);
u32 encode_s_type(int imm, u32 rs2, u32 rs1, u32 funct3, u32 opcode);
u32 encode_b_type(int imm, u32 rs2, u32 rs1, u32 funct3, u32 opcode);
u32 encode_u_type(int imm, u32 rd, u32 opcode);
u32 encode_j_type(int imm, u32 rd, u32 opcode);

u32 encode_lui(int rd, int imm);
u32 encode_auipc(int rd, int imm);

u32 encode_jal(int rd, int imm);
u32 encode_jalr(int rd, int rs1, int imm);

u32 encode_beq(int rs1, int rs2, int imm);
u32 encode_bne(int rs1, int rs2, int imm);
u32 encode_blt(int rs1, int rs2, int imm);
u32 encode_bge(int rs1, int rs2, int imm);
u32 encode_bltu(int rs1, int rs2, int imm);
u32 encode_bgeu(int rs1, int rs2, int imm);

u32 encode_lb(int rd, int rs1, int imm);
u32 encode_lh(int rd, int rs1, int imm);
u32 encode_lw(int rd, int rs1, int imm);
u32 encode_lbu(int rd, int rs1, int imm);
u32 encode_lhu(int rd, int rs1, int imm);

u32 encode_sb(int rs2, int rs1, int imm);
u32 encode_sh(int rs2, int rs1, int imm);
u32 encode_sw(int rs2, int rs1, int imm);

u32 encode_addi(int rd, int rs1, int imm);
u32 encode_slti(int rd, int rs1, int imm);
u32 encode_sltiu(int rd, int rs1, int imm);
u32 encode_xori(int rd, int rs1, int imm);
u32 encode_ori(int rd, int rs1, int imm);
u32 encode_andi(int rd, int rs1, int imm);

u32 encode_slli(int rd, int rs1, int shamt);
u32 encode_srli(int rd, int rs1, int shamt);
u32 encode_srai(int rd, int rs1, int shamt);

u32 encode_add(int rd, int rs1, int rs2);
u32 encode_sub(int rd, int rs1, int rs2);
u32 encode_sll(int rd, int rs1, int rs2);
u32 encode_slt(int rd, int rs1, int rs2);
u32 encode_sltu(int rd, int rs1, int rs2);
u32 encode_xor(int rd, int rs1, int rs2);
u32 encode_srl(int rd, int rs1, int rs2);
u32 encode_sra(int rd, int rs1, int rs2);
u32 encode_or(int rd, int rs1, int rs2);
u32 encode_and(int rd, int rs1, int rs2);

#endif
