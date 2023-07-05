module fetchstore(
                  // input logic         clk,

                  // memory bus
                  // read port
                  output logic [31:0] busra,
                  input logic [31:0]  busrd,

                  // write port
                  output logic [31:0] buswa,
                  output logic [31:0] buswd,
                  output logic        buswe,

                  // connection to core
                  input logic [31:0]  coreaddr,
                  input logic [31:0]  corewd,
                  output logic [31:0] corerd,

                  input logic         corew, coresext,
                  input logic [1:0]   corewidth

                  // TODO unaligned exceptions
                  );
`include "membus_consts.sv"

  logic [31:0]                        addr_aligned;
  logic [31:0]                        rd_shifted;

  logic [4:0]                         shift;
  logic [31:0]                        widthmask;
  logic [31:0]                        mask_shifted;
  logic [31:0]                        corewd_shifted;

  assign addr_aligned = {coreaddr[31:2], 2'b00};
  assign shift = {coreaddr[1:0], 3'b0};

  always_comb
    if (corew)
      begin
        busra = addr_aligned;
        buswa = addr_aligned;
        buswe = 1;

        case (corewidth)
          MEMW_BYTE: widthmask = 32'hff;
          MEMW_HALF: widthmask = 32'hffff;
          MEMW_WORD: widthmask = 32'hffffffff;
          default:   widthmask = 32'X;
        endcase
        mask_shifted = widthmask << shift;
        corewd_shifted = corewd << shift;

        // buswd = mask_shifted ? corewd : busrd;
        buswd = (busrd & ~mask_shifted) | (corewd_shifted & mask_shifted);
      end
    else
      begin
        busra = addr_aligned;
        buswa = 32'X;
        buswe = 0;

        rd_shifted = busrd >> shift;

        case (corewidth)
          MEMW_BYTE: corerd = {
                               {coresext ? {24{rd_shifted[7]}} : 24'b0},
                               rd_shifted[7:0]
                               };
          MEMW_HALF: corerd = {
                               {coresext ? {16{rd_shifted[15]}} : 16'b0},
                               rd_shifted[15:0]
                               };
          MEMW_WORD: corerd = rd_shifted;
          default:   corerd = 32'X;
        endcase
      end
endmodule
