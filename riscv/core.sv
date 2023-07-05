module core(input logic clk, reset,

            // memory read ports
            output logic [31:0] busra1,
            input logic [31:0]  busrd1,
            output logic [31:0] busra2,
            input logic [31:0]  busrd2,

            // memory write ports
            output logic [31:0] buswa3,
            output logic [31:0] buswd3,
            output logic        buswe3
            );

  parameter                     RESET_VECTOR = 32'b0;

  logic [31:0]                  pc;
  logic [31:0]                  insn;
  logic [31:0]                  nextpc;

  logic [31:0]                  memaddr;
  logic [31:0]                  memrdata;
  logic [31:0]                  memwdata;
  logic                         memw, memsext;
  logic [1:0]                   memwidth;

  always_ff @(posedge clk, reset)
    begin
      if (reset)
        pc <= RESET_VECTOR;
      else
        pc <= nextpc;
    end

  assign busra1 = pc;
  assign insn = busrd1;

  hart exec
    (
     .nextpc(nextpc[31:0]),
     .memaddr(memaddr[31:0]),
     .memwdata(memwdata[31:0]),
     .memw(memw),
     .memsext(memsext),
     .memwidth(memwidth[1:0]),

     .clk(clk),
     .reset(reset),
     .insn(insn[31:0]),
     .pc(pc[31:0]),
     .memrdata(memrdata[31:0])
     );

  fetchstore fetchstore
    (
     .busra(busra2),
     .busrd(busrd2),
     .buswa(buswa3),
     .buswd(buswd3),
     .buswe(buswe3),

     .coreaddr(memaddr),
     .corewd(memwdata),
     .corerd(memrdata),
     .corew(memw),
     .coresext(memsext),
     .corewidth(memwidth)
     );
endmodule
