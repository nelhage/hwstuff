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
`include "opcodes.sv";
  // controller
  assign memwidth = funct3[1:0];
  assign memsext = ~funct3[2];

  always_comb
    case (opcode)
      OPCODE_ALUIMM:
        begin
          asel = 0;
          bsel = 1;
          regw = 1;
          memw = 0;
          rwsel = 0;
          pcsel = 0;
          aluctl = {funct7[5], funct3};
        end

      OPCODE_ALU:
        begin
          asel = 0;
          bsel = 0;
          regw = 1;
          memw = 0;
          rwsel = 0;
          pcsel = 0;
          aluctl = ALUCTL_ADD;
        end

      OPCODE_LOAD:
        begin
          asel = 0;
          memw = 0;
          regw = 1;
          bsel = 1;
          rwsel = 1;
          pcsel = 0;
          aluctl = ALUCTL_ADD;
        end

      OPCODE_STORE:
        begin
          asel = 0;
          bsel = 1;
          memw = 1;
          regw = 0;
          pcsel = 0;
          aluctl = ALUCTL_ADD;

          rwsel = 1'X;
        end

      OPCODE_LUI:
        begin
          asel = 0;
          memw = 0;
          regw = 1;
          bsel = 1;
          pcsel = 0;
          aluctl = ALUCTL_ADD;
          rwsel = 0;
        end

      OPCODE_AUIPC:
        begin
          asel = 1;
          memw = 0;
          regw = 1;
          bsel = 1;
          pcsel = 0;
          aluctl = ALUCTL_ADD;
          rwsel = 0;
        end

      OPCODE_JAL:
        begin
          asel = 1;
          bsel = 1;
          memw = 0;
          aluctl = ALUCTL_ADD;
          pcsel = 1;
          regw = 1;
          rwsel = 2;
        end

      OPCODE_JALR:
        begin
          asel = 0;
          bsel = 1;
          memw = 0;
          aluctl = ALUCTL_ADD;
          pcsel = 1;
          regw = 1;
          rwsel = 2;
        end

      OPCODE_BRANCH:
        begin
          asel = 0;
          bsel = 0;
          memw = 0;
          regw = 0;
          rwsel = 'X;
          pcsel[1] = 1;

          case (funct3)
            FUNCT3_BRANCH_BEQ:
              begin
                aluctl = ALUCTL_SUB;
                pcsel[0] = 0;
              end
            FUNCT3_BRANCH_BNE:
              begin
                aluctl = ALUCTL_SUB;
                pcsel[0] = 1;
              end

            FUNCT3_BRANCH_BLT:
              begin
                aluctl = ALUCTL_SLT;
                pcsel[0] = 1;
              end
            FUNCT3_BRANCH_BGE:
              begin
                aluctl = ALUCTL_SLT;
                pcsel[0] = 0;
              end

            FUNCT3_BRANCH_BLTU:
              begin
                aluctl = ALUCTL_SLTU;
                pcsel[0] = 1;
              end
            FUNCT3_BRANCH_BGEU:
              begin
                aluctl = ALUCTL_SLTU;
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
