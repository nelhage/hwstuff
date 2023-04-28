
module alu(
           input logic [31:0] a, b,
           input logic [2:0] ctl,
           output logic [31:0] result
);
  always_comb
    begin
      case (ctl)
        // ADD
        3'b000: result = a + b;
        // SLT
        3'b010: result = {31'b0, a < b};
        // SLTI
        3'b011: result = 0;
        // XOR
        3'b100: result = a^b;
        // OR
        3'b110: result = a|b;
        // AND
        3'b111: result = a&b;
        default: result = 0;
      endcase
    end

endmodule
