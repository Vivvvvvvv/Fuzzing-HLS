module testbench();
  reg rst, clk;
  wire [31:0] return;
  wire rdy;

  result result_dat(clk, rst, return, rdy);

  initial begin
    rst = 1;
    clk = 0;
    #10
    rst = 0;
  end

  always #5 clk = ~clk;

  always @(posedge clk) begin
    if (rdy) begin
      $display("checksum = %08h", return);
      $finish();
    end
  end

endmodule
