/* verilator lint_off UNUSEDPARAM */
localparam OPCODE_LUI   = 7'b0110111;
localparam OPCODE_AUIPC = 7'b0010111;
localparam OPCODE_JAL   = 7'b1101111;
localparam OPCODE_JALR  = 7'b1100111;

localparam OPCODE_BRANCH = 7'b1100011;
localparam FUNCT3_BRANCH_beq = 3'h0;
localparam FUNCT3_BRANCH_bne = 3'h1;
localparam FUNCT3_BRANCH_blt = 3'h4;
localparam FUNCT3_BRANCH_bge = 3'h5;
localparam FUNCT3_BRANCH_bltu = 3'h6;
localparam FUNCT3_BRANCH_bgeu = 3'h7;

localparam OPCODE_LOAD = 7'b0000011;
localparam FUNCT3_LOAD_lb = 3'h0;
localparam FUNCT3_LOAD_lh = 3'h1;
localparam FUNCT3_LOAD_lw = 3'h2;
localparam FUNCT3_LOAD_lbu = 3'h4;
localparam FUNCT3_LOAD_lhu = 3'h5;

localparam OPCODE_STORE = 7'b0100011;
localparam FUNCT3_STORE_sb = 3'h0;
localparam FUNCT3_STORE_sh = 3'h1;
localparam FUNCT3_STORE_sw = 3'h2;

localparam OPCODE_ALUIMM = 7'b0010011;
localparam OPCODE_ALU = 7'b0110011;

localparam FUNCT3_ALU_ADD = 3'h0;
localparam FUNCT3_ALU_SLL = 3'h1;
localparam FUNCT3_ALU_SLT = 3'h2;
localparam FUNCT3_ALU_SLTU = 3'h3;
localparam FUNCT3_ALU_XOR = 3'h4;
localparam FUNCT3_ALU_SR = 3'h5;
localparam FUNCT3_ALU_OR = 3'h6;
localparam FUNCT3_ALU_AND = 3'h7;

localparam ALUCTL_ADD  = 4'h0;
localparam ALUCTL_SUB  = 4'h8;
localparam ALUCTL_SLL  = 4'h1;
localparam ALUCTL_SLT  = 4'h2;
localparam ALUCTL_SLTU = 4'h3;
localparam ALUCTL_XOR  = 4'h4;
localparam ALUCTL_SRL  = 4'h5;
localparam ALUCTL_SRA  = 4'hd;
localparam ALUCTL_OR   = 4'h6;
localparam ALUCTL_AND  = 4'h7;
/* verilator lint_on UNUSEDPARAM */
