`define MEMW_BYTE 2'b00
`define MEMW_HALF 2'b01
`define MEMW_WORD 2'b10

module hart(input logic clk, reset,
            input logic [31:0]  insn,
            // input logic [31:0]  pc,

            logic [31:0]        memdata,
            output logic [31:0] memaddr,
            output logic        memw, memsext,
            output logic [1:0]  memwidth
            );
`include "opcodes.sv";

  logic [4:0]                   rs1, rs2, rd;
  logic       regw;
  logic [31:0] r1, r2, regwdata;
  logic [31:0] imm;

  logic [31:0] alu_a, alu_b, alu_out;

  // control signals
  logic [6:0]  opcode;
  logic [2:0]  funct3;
  /* verilator lint_off UNUSEDSIGNAL */
  logic [6:0]  funct7;
  /* verilator lint_on UNUSEDSIGNAL */
  logic [3:0]  aluctl;
  logic        asel, bsel;
  logic        rwsel;

  // datapath

  regfile regs(clk, reset, regw,
               rs1, rs2, rd,
               regwdata,
               r1, r2);


  alu alu(alu_a, alu_b, aluctl, alu_out);

  decoder decode(insn, opcode, funct3, funct7,
                 rs1, rs2, rd, imm);


  always_comb
    case (asel)
      default: alu_a = r1;
    endcase

  always_comb
    case (bsel)
      1'0: alu_b = r2;
      1'1: alu_b = imm;
    endcase

  always_comb
    case (rwsel)
      1'0: regwdata = alu_out;
      1'1: regwdata = memdata;
    endcase

  assign memaddr = alu_out;

  // controller
  assign asel = 0;
  assign memwidth = funct3[1:0];
  assign memsext = ~funct3[2];

  always_comb
    case (opcode)
      OPCODE_ALUIMM:
        begin
          bsel = 1;
          regw = 1;
          memw = 0;
          rwsel = 0;
          aluctl = {funct7[5], funct3};
        end

      OPCODE_ALU:
        begin
          bsel = 0;
          regw = 1;
          memw = 0;
          rwsel = 0;
          aluctl = ALUCTL_ADD;
        end

      OPCODE_LOAD:
        begin
          memw = 0;
          regw = 1;
          bsel = 1;
          rwsel = 1;
          aluctl = ALUCTL_ADD;
        end

      OPCODE_STORE:
        begin
          memw = 1;
          regw = 0;
          bsel = 1;
          aluctl = ALUCTL_ADD;

          rwsel = 1'X;
        end

      OPCODE_LUI:
        begin
          memw = 0;
          regw = 1;
          bsel = 1;
          aluctl = ALUCTL_ADD;
          rwsel = 0;
        end

      default:
        begin
          bsel = 0;
          regw = 0;
          memw = 0;
          aluctl = 0;
          rwsel = 0;
        end
    endcase


  // controller control(insn);
endmodule
