#include "../local-include/reg.h"
#include <cpu/ifetch.h>
#include <isa-all-instr.h>

/* Metaprogramming, FUCK YOU!!! */
def_all_THelper();

static uint32_t get_instr(Decode *s)
{
    return s->isa.instr.val;
}

/* Start Helper */

/* End Helper */


/* Start decode operand helper */

#define def_DopHelper(name) \
    void concat(decode_op_, name) (Decode *s, Operand *op, word_t val, bool flag)

static def_DopHelper(i)
{
    op->imm = val;
}

static def_DopHelper(r)
{
    bool is_write = flag;
    static word_t zero_null = 0;
    op->preg = (is_write && val == 0) ? &zero_null : &gpr(val);
}

/* End decode operand helper */



/* Start Decode helper functions */

static def_DHelper(R)
{
    decode_op_r(s, id_src1, s->isa.instr.r.rs1, false);
    decode_op_r(s, id_src2, s->isa.instr.r.rs2, false);
    decode_op_r(s, id_dest, s->isa.instr.r.rd, true);
}

static def_DHelper(I)
{
    decode_op_r(s, id_src1, s->isa.instr.i.rs1, false);
    decode_op_i(s, id_src2, s->isa.instr.i.simm11_0, false);
    decode_op_r(s, id_dest, s->isa.instr.i.rd, true);
}

static def_DHelper(S)
{
    decode_op_r(s, id_src1, s->isa.instr.s.rs1, false);
    sword_t simm = (s->isa.instr.s.simm11_5 << 5) | s->isa.instr.s.imm4_0;
    decode_op_i(s, id_src2, simm, false);
    decode_op_r(s, id_dest, s->isa.instr.s.rs2, false);
}

static def_DHelper(B)
{
    // rs1
    decode_op_r(s, id_src1, s->isa.instr.b.rs1, false);
    
    // rs2
    decode_op_r(s, id_src2, s->isa.instr.b.rs2, false);

    word_t simm = s->isa.instr.b.imm4_1 << 1
                    | s->isa.instr.b.imm10_5 << 5
                    | s->isa.instr.b.imm11 << 11
                    | s->isa.instr.b.simm12 << 12;

    // imm
    rtl_li(s, s0, simm);
}

static def_DHelper(U)
{
    // imm[31:12]
    decode_op_i(s, id_src1, s->isa.instr.u.imm31_12 << 12, true);
    // rd[11:7]
    decode_op_r(s, id_dest, s->isa.instr.u.rd, true);
}

static def_DHelper(J)
{
    word_t cur_imm = s->isa.instr.j.imm20 << 20;
    cur_imm |= s->isa.instr.j.imm19_12 << 12;
    cur_imm |= s->isa.instr.j.imm11 << 11;
    cur_imm |= s->isa.instr.j.imm10_1 << 1;

    // imm -> src1
    // rd -> id_dest.
    decode_op_i(s, id_src1, cur_imm, false);
    decode_op_r(s, id_dest, s->isa.instr.j.rd, true);
}

/* End with Decode Helper functions */



/* Start table helper functions. The macro define in nemu/include/cpu/decode.h */

// Get the real execution function by looking up the secondary table
def_THelper(load)
{
    //
    // def_INSTR_TAB("??????? ????? ????? 000 ????? ????? ??", lb);

    //
    // def_INSTR_TAB("??????? ????? ????? 001 ????? ????? ??", lh);

    //
    def_INSTR_TAB("??????? ????? ????? 010 ????? ????? ??", lw);
    
    //
    // def_INSTR_TAB("??????? ????? ????? 100 ????? ????? ??", lbu);

    //
    // def_INSTR_TAB("??????? ????? ????? 101 ????? ????? ??", lhu);
    
    return EXEC_ID_inv;
}

def_THelper(store)
{
    // //
    // def_INSTR_TAB("??????? ????? ????? 000 ????? ????? ??", sb);
    
    // //
    // def_INSTR_TAB("??????? ????? ????? 001 ????? ????? ??", su);
    
    //
    def_INSTR_TAB("??????? ????? ????? 010 ????? ????? ??", sw);
    
    return EXEC_ID_inv;
}

// For all immediate instructions. Opcode 0010011 means OP-IMM.
def_THelper(OP_IMM)
{
    // funct3 == 000. addi
    def_INSTR_TAB("??????? ????? ????? 000 ????? ????? ??", addi);

    //
    def_INSTR_TAB("??????? ????? ????? 010 ????? ????? ??", slti);

    //
    def_INSTR_TAB("??????? ????? ????? 011 ????? ????? ??", sltiu);
    // 
    // //
    // def_INSTR_TAB("??????? ????? ????? 100 ????? ????? ??", xori);
 
    // //
    // def_INSTR_TAB("??????? ????? ????? 110 ????? ????? ??", ori);
 
    // //
    // def_INSTR_TAB("??????? ????? ????? 111 ????? ????? ??", andi);
 
    // //
    // def_INSTR_TAB("0000000 ????? ????? 001 ????? ????? ??", slli);
 
    // //
    // def_INSTR_TAB("0000000 ????? ????? 101 ????? ????? ??", srli);

    // //
    // def_INSTR_TAB("0100000 ????? ????? 101 ????? ????? ??", srai);
 
    return EXEC_ID_inv;
}

// Opcode JALR == 1100111
def_THelper(JALR)
{
    // funct3 == 000, jalr. JALR (jump and link register). Also call "ret" in pesudo instruction.
    def_INSTR_TAB("??????? ????? ????? 000 ????? ????? ??", jalr);

    return EXEC_ID_inv;
}

// Opcode OP == 0110011
def_THelper(OP)
{
    // funct3 == 000, funct7 == 0000000;
    def_INSTR_TAB("0000000 ????? ????? 000 ????? ????? ??", add);

    //
    def_INSTR_TAB("0100000 ????? ????? 000 ????? ????? ??", sub);

    // SLL
    // def_INSTR_TAB("0000000 ????? ????? 001 ????? ????? ??", sll);

    // // SLT
    // def_INSTR_TAB("0000000 ????? ????? 010 ????? ????? ??", slt);

    // // SLTU
    // def_INSTR_TAB("0000000 ????? ????? 011 ????? ????? ??", sltu);

    // // XOR
    // def_INSTR_TAB("0000000 ????? ????? 100 ????? ????? ??", xor);

    // // SRL
    // def_INSTR_TAB("0000000 ????? ????? 101 ????? ????? ??", srl);

    // // SRA
    // def_INSTR_TAB("0100000 ????? ????? 101 ????? ????? ??", sra);

    // // OR
    // def_INSTR_TAB("0000000 ????? ????? 110 ????? ????? ??", or);

    // // AND
    // def_INSTR_TAB("0000000 ????? ????? 111 ????? ????? ??", and);

    return EXEC_ID_inv;
}

// Opcode BRANCH == 1100011
def_THelper(BRANCH)
{
    // BEQ
    def_INSTR_TAB("??????? ????? ????? 000 ????? ????? ??", beq);

    // BNE
    def_INSTR_TAB("??????? ????? ????? 001 ????? ????? ??", bne);

    // // BLT
    // def_INSTR_TAB("??????? ????? ????? 100 ????? ????? ??", blt);

    // // BGE
    // def_INSTR_TAB("??????? ????? ????? 101 ????? ????? ??", bge);

    // // BLTU
    // def_INSTR_TAB("??????? ????? ????? 110 ????? ????? ??", bltu);
    // 
    // // BGEU
    // def_INSTR_TAB("??????? ????? ????? 111 ????? ????? ??", bgeu);

    return EXEC_ID_inv;
}

/* Edn table helper functions. */

/* Decode start from here. */

def_THelper(main)
{
    // TODO:
    // beqz

    /* Load and store */

    def_INSTR_IDTAB("??????? ????? ????? ??? ????? 00000 11", I, load);
    def_INSTR_IDTAB("??????? ????? ????? ??? ????? 01000 11", S, store);

    /* End Load and store */


    /* Integer Computational Instructions */ 
    
    // Lookup secondary table for all immediate instructions. 
    // Opcode 0010011 == OP-IMM.
    def_INSTR_IDTAB("??????? ????? ????? ??? ????? 00100 11", I, OP_IMM);

    // LUI (load upper immediate) is used to build 32-bit constants and uses the U-type format.
    // Opcode 0110111 == LUI
    def_INSTR_IDTAB("??????? ????? ????? ??? ????? 01101 11", U, lui);

    // AUIPC (add upper immediate to pc)
    // Opcode 0010111 == AUIPC
    def_INSTR_IDTAB("??????? ????? ????? ??? ????? 00101 11", U, auipc);

    // OP
    def_INSTR_IDTAB("??????? ????? ????? ??? ????? 01100 11", R, OP);

    // BRANCH 
    def_INSTR_IDTAB("??????? ????? ????? ??? ????? 11000 11", B, BRANCH);

    

    /* End Integer Computational Instructions */


    /* Control transfer */

    // Jal (jump and link)
    // Opcode == JAL
    def_INSTR_IDTAB("??????? ????? ????? ??? ????? 11011 11", J, jal);

    // Opcode JALR == 1100111
    def_INSTR_IDTAB("??????? ????? ????? ??? ????? 11001 11", I, JALR);

    /* End Control transfer */


    /* Other */
    
    def_INSTR_TAB("??????? ????? ????? ??? ????? 11010 11", nemu_trap);

    /* End other */

    return table_inv(s);
};

int isa_fetch_decode(Decode *s)
{
    s->isa.instr.val = instr_fetch(&s->snpc, (int) sizeof(word_t));
    int idx = table_main(s);
    return idx;
}
