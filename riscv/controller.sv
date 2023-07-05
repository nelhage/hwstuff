`include "opcodes.sv"

module controller(
                  input logic [6:0]  opcode,
                  input logic [2:0]  funct3,
                  /* verilator lint_off UNUSEDSIGNAL */
                  input logic [6:0]  funct7,
                  /* verilator lint_on UNUSEDSIGNAL */

                  output logic [1:0] memwidth,
                  output logic       memw, memsext,
                  output logic [3:0] aluctl,
                  output logic       asel, bsel,
                  output logic       regw,
                  output logic [1:0] rwsel,
                  output logic [1:0] pcsel
);
  // controller
  assign memwidth = funct3[1:0];
  assign memsext = ~funct3[2];

  always_comb
    case (opcode)
      opcodes::OPCODE_ALUIMM:
        begin
          asel = 0;
          bsel = 1;
          regw = 1;
          memw = 0;
          rwsel = 0;
          pcsel = 0;
          aluctl = {funct7[5], funct3};
        end

      opcodes::OPCODE_ALU:
        begin
          asel = 0;
          bsel = 0;
          regw = 1;
          memw = 0;
          rwsel = 0;
          pcsel = 0;
          aluctl = opcodes::ALUCTL_ADD;
        end

      opcodes::OPCODE_LOAD:
        begin
          asel = 0;
          memw = 0;
          regw = 1;
          bsel = 1;
          rwsel = 1;
          pcsel = 0;
          aluctl = opcodes::ALUCTL_ADD;
        end

      opcodes::OPCODE_STORE:
        begin
          asel = 0;
          bsel = 1;
          memw = 1;
          regw = 0;
          pcsel = 0;
          aluctl = opcodes::ALUCTL_ADD;

          rwsel = 1'X;
        end

      opcodes::OPCODE_LUI:
        begin
          asel = 0;
          memw = 0;
          regw = 1;
          bsel = 1;
          pcsel = 0;
          aluctl = opcodes::ALUCTL_ADD;
          rwsel = 0;
        end

      opcodes::OPCODE_AUIPC:
        begin
          asel = 1;
          memw = 0;
          regw = 1;
          bsel = 1;
          pcsel = 0;
          aluctl = opcodes::ALUCTL_ADD;
          rwsel = 0;
        end

      opcodes::OPCODE_JAL:
        begin
          asel = 1;
          bsel = 1;
          memw = 0;
          aluctl = opcodes::ALUCTL_ADD;
          pcsel = 1;
          regw = 1;
          rwsel = 2;
        end

      opcodes::OPCODE_JALR:
        begin
          asel = 0;
          bsel = 1;
          memw = 0;
          aluctl = opcodes::ALUCTL_ADD;
          pcsel = 1;
          regw = 1;
          rwsel = 2;
        end

      opcodes::OPCODE_BRANCH:
        begin
          asel = 0;
          bsel = 0;
          memw = 0;
          regw = 0;
          rwsel = 'X;
          pcsel[1] = 1;

          case (funct3)
            opcodes::FUNCT3_BRANCH_BEQ:
              begin
                aluctl = opcodes::ALUCTL_SUB;
                pcsel[0] = 0;
              end
            opcodes::FUNCT3_BRANCH_BNE:
              begin
                aluctl = opcodes::ALUCTL_SUB;
                pcsel[0] = 1;
              end

            opcodes::FUNCT3_BRANCH_BLT:
              begin
                aluctl = opcodes::ALUCTL_SLT;
                pcsel[0] = 1;
              end
            opcodes::FUNCT3_BRANCH_BGE:
              begin
                aluctl = opcodes::ALUCTL_SLT;
                pcsel[0] = 0;
              end

            opcodes::FUNCT3_BRANCH_BLTU:
              begin
                aluctl = opcodes::ALUCTL_SLTU;
                pcsel[0] = 1;
              end
            opcodes::FUNCT3_BRANCH_BGEU:
              begin
                aluctl = opcodes::ALUCTL_SLTU;
                pcsel[0] = 0;
              end
            default:
              begin
                aluctl = 'X;
                pcsel[0] = 'X;
              end
          endcase
        end

      default:
        begin
          asel = 0;
          bsel = 0;
          regw = 0;
          memw = 0;
          aluctl = 0;
          pcsel = 0;
          rwsel = 0;
        end
    endcase
endmodule
