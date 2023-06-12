
module alu(
           input logic [31:0]  a, b,
           input logic [3:0]   ctl,
           output logic [31:0] result
);
`include "opcodes.sv"
  // always
  //   begin
  //     $display("ctl=%b a=%b b=%b", ctl, a, b);
  //     $display("a<b=%b, signed[a<b]=%b", a<b, signed_a<signed_b);
  //   end

  always_comb
    begin
      case (ctl)
        ALUCTL_ADD: result = a + b;
        ALUCTL_SUB: result = a - b;
        ALUCTL_SLT: result = {31'b0, signed'(a) < signed'(b)};
        ALUCTL_SLTU: result = {31'b0, a < b};
        ALUCTL_XOR: result = a^b;
        ALUCTL_OR: result = a|b;
        ALUCTL_AND: result = a&b;
        ALUCTL_SLL: result = a << b[4:0];
        ALUCTL_SRL: result = (a >> b[4:0]);
        ALUCTL_SRA: result = signed'(signed'(a) >>> b[4:0]);
        default: result = 0;
      endcase
    end

endmodule
