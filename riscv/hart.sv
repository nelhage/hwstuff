module hart(input logic clk, reset,
            input logic [31:0]  insn,
            input logic [31:0]  pc,


            output logic [31:0] nextpc,
            output logic [31:0] memaddr,
            input logic [31:0]  memrdata,
            output logic [31:0] memwdata,
            output logic        memw, memsext,
            output logic [1:0]  memwidth
            );
`include "opcodes.sv";

  logic [4:0]                   rs1, rs2, rd;
  logic [31:0]                  r1, r2, regwdata;
  logic [31:0]                  imm;

  logic [31:0]                  alu_a, alu_b, alu_out;
  logic [31:0]                  pcplus4;
  logic [31:0]                  pcplusimm;


  // control signals
  logic [6:0]                   opcode;
  logic [2:0]                   funct3;
  logic [6:0]                   funct7;
  logic [3:0]                   aluctl;
  logic                         asel, bsel;
  logic                         regw;
  logic [1:0]                   rwsel;
  logic [1:0]                   pcsel;

  controller control(opcode, funct3, funct7,
                     memwidth, memw, memsext,
                     aluctl, asel, bsel,
                     regw, rwsel, pcsel);

  // datapath

  regfile regs(clk, reset, regw,
               rs1, rs2, rd,
               regwdata,
               r1, r2);


  alu alu(alu_a, alu_b, aluctl, alu_out);

  decoder decode(insn, opcode, funct3, funct7,
                 rs1, rs2, rd, imm);


  assign pcplus4 = pc + 4;
  assign pcplusimm = pc + imm;

  assign memaddr = alu_out;
  assign memwdata = memw ? r2 : 32'hZZZZZZZZ;

  always_comb
    case (asel)
      1'0: alu_a = r1;
      1'1: alu_a = pc;
    endcase

  always_comb
    case (bsel)
      1'0: alu_b = r2;
      1'1: alu_b = imm;
    endcase

  always_comb
    case (rwsel)
      2'h0: regwdata = alu_out;
      2'h1: regwdata = memrdata;
      2'h2: regwdata = pcplus4;
      default: regwdata = 32'X;
    endcase

  always_comb
    case (pcsel)
      2'b00: nextpc = pcplus4;
      2'b01: nextpc = alu_out & ~32'b1;
      2'b10, 2'b11:
        if ((alu_out == 0) == pcsel[0])
          nextpc = pcplus4;
        else
          nextpc = pcplusimm;
      default: nextpc = 32'X;
    endcase
endmodule
