`include "opcodes.sv"

module alu(
           input logic [31:0]  a, b,
           input logic [3:0]   ctl,
           output logic [31:0] result
);
  // always
  //   begin
  //     $display("ctl=%b a=%b b=%b", ctl, a, b);
  //     $display("a<b=%b, signed[a<b]=%b", a<b, signed_a<signed_b);
  //   end

  always_comb
    begin
      case (ctl)
        opcodes::ALUCTL_ADD: result = a + b;
        opcodes::ALUCTL_SUB: result = a - b;
        opcodes::ALUCTL_SLT: result = {31'b0, signed'(a) < signed'(b)};
        opcodes::ALUCTL_SLTU: result = {31'b0, a < b};
        opcodes::ALUCTL_XOR: result = a^b;
        opcodes::ALUCTL_OR: result = a|b;
        opcodes::ALUCTL_AND: result = a&b;
        opcodes::ALUCTL_SLL: result = a << b[4:0];
        opcodes::ALUCTL_SRL: result = (a >> b[4:0]);
        opcodes::ALUCTL_SRA: result = signed'(signed'(a) >>> b[4:0]);
        default: result = 0;
      endcase
    end

endmodule
