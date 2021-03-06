// Jump and link
// PAGE: 20
// The offset is sign-extended and added to the address of the
// jump instruction to form the jump target address
def_EHelper(jal)
{
	// Save next instuctions address to rd, and set new insturction address to pc.

	// Store next inst to ddest.
	rtl_addi(s, ddest, &s->pc, 4);

	// Sign Extend.
	rtl_li(s, s0, id_src1->imm);
	rtl_sign_ext_pos(s, s0, s0, 20);

	// Compute the inst address jump to.
	rtl_add(s, s0, &s->pc, s0);

	// Jump *s0
	rtl_jr(s, s0);
}

// Jump and link register
// Page: 
// The indirect jump instruction JALR (jump and link register) uses the I-type encoding. The target
// address is obtained by adding the sign-extended 12-bit I-immediate to the register rs1, then setting
// the least-significant bit of the result to zero. The address of the instruction following the jump
// (pc+4) is written to register rd. Register x0 can be used as the destination if the result is not
// required.
def_EHelper(jalr)
{
	// Store the next instuction(pc + 4) to ddest(rd).
	rtl_addi(s, ddest, &s->pc, 4);

	// Adding sign-extended 12-bit I-immediate to the register rs1.
	
	// s0 = imm[11:0].
	rtl_li(s, s0, id_src2->imm);

	// sign ext imm[11:0] in s0.
	rtl_sign_ext_pos(s, s0, s0, 11);

	// rd = src_1(rs1) (NOTE: The original register!) + s0(imm[11:0])
	rtl_add(s, s0, s0, dsrc1);

	// setting the least-significant bit of the result to zero
	rtl_srli(s, s0, s0, 1);
	rtl_slli(s, s0, s0, 1);

	// Jump to *s0.
	rtl_jr(s, s0);
}
