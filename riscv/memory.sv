module memory(input logic clk, we,
              input logic [31:0]  ra,
              input logic [31:0]  wa, wd,
              output logic [31:0] rd);

  parameter                       MEMORY_WORDS = 1 << 16;

  logic [31:0]                    RAM [(MEMORY_WORDS)-1:0];

  /* verilator lint_off WIDTH */
  always_comb
    if (ra[31:2] >= MEMORY_WORDS)
      assign rd = 32'b0;
    else
      assign rd = RAM[ra[31:2]];

  always_ff @(posedge(clk))
    if (we && wa[31:2] < MEMORY_WORDS)
      RAM[wa[31:2]] = wd;

  task initialize_memory(input string filename);
    $readmemh(filename, RAM);
  endtask

  export "DPI-C" task initialize_memory;
endmodule
