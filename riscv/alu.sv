
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
      case (ctl[2:0])
        // ADD
        3'b000: result = (ctl[3] == 0) ? a + b : a - b;
        // SLT
        3'b010: result = {31'b0, signed'(a) < signed'(b)};
        // SLTU
        3'b011: result = {31'b0, a < b};
        // XOR
        3'b100: result = a^b;
        // OR
        3'b110: result = a|b;
        // AND
        3'b111: result = a&b;
        // SLL
        3'b001: result = a << b[4:0];
        // SR[AL]
        3'b101: result = (ctl[3] == 0) ? (a >> b[4:0]) : signed'(signed'(a) >>> b[4:0]);

        default: result = 0;
      endcase
    end

endmodule
