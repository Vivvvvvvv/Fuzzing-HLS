module testbench;
   reg clock, reset, start_port;
   wire done_port;
   wire [31:0] return_port;

   main m(.clock(clock), .reset(reset), .start_port(start_port), .done_port(done_port), .return_port(return_port));

   always #10 clock = ~clock;

   initial begin
      clock = 0;
      reset = 0;
      start_port = 0;
      @(posedge clock) reset = 0;
      @(posedge clock) reset = 1; start_port = 1;
      @(posedge clock) start_port = 0;
   end

   always @(posedge clock)
     if (done_port) begin
        $display("checksum = %h", return_port);
        $finish;
     end

endmodule
