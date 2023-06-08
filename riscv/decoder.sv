module decoder(input logic [31:0] insn,
               output logic [6:0]  opcode,
               output logic [2:0]  funct3,
               output logic [6:0]  funct7,
               output logic [4:0]  rs1, rs2, rd,
               output logic [31:0] imm);
  logic [31:0] imm_Itype, imm_Stype, imm_Btype, imm_Utype, imm_Jtype;

  assign opcode = insn[6:0];
  assign funct3 = insn[14:12];
  assign funct7 = insn[31:25];
  assign rs1 = insn[24:20];
  assign rs2 = insn[19:15];
  assign rd = insn[11:7];

  assign imm_Itype = {{21{insn[31]}}, insn[30:20]};
  assign imm_Stype = {{21{insn[31]}}, insn[30:25], insn[11:7]};
  assign imm_Btype = {{20{insn[31]}}, insn[7], insn[30:25], insn[11:8], 1'b0};
  assign imm_Utype = {insn[31:12], 12'b0};
  assign imm_Jtype = {{12{insn[31]}}, insn[19:12], insn[20], insn[30:21], 1'b0};

  always_comb
    case (opcode[6:2])
      // LUI, AIUPC
      5'b01101, 5'b00101: imm = imm_Utype;
      // JAL
      5'b11011: imm = imm_Jtype;
      // B*
      5'b11000: imm = imm_Btype;
      // JALR, [arith]I, L*
      5'b11001, 5'b00100, 5'b00000: imm = imm_Itype;
      // S*
      5'b01000: imm = imm_Stype;
      default: imm = 32'bX;
    endcase
endmodule
