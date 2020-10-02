go new
solution file add ./test.cpp
go analyze
go compile
solution library add mgc_Xilinx-VIRTEX-7-2_beh -- -rtlsyntool Vivado -manufacturer Xilinx -family VIRTEX-7 -speed -2 -part xc7vx485tffg1157-2
solution library add Xilinx_RAMS
solution library add Xilinx_ROMS
solution library add Xilinx_FIFO
go libraries
directive set -CLOCKS {clk {-CLOCK_PERIOD 10 -CLOCK_EDGE rising -CLOCK_HIGH_TIME 5 -CLOCK_OFFSET 0.000000 -CLOCK_UNCERTAINTY 0.0 -RESET_KIND sync -RESET_SYNC_NAME rst -RESET_SYNC_ACTIVE high -RESET_ASYNC_NAME arst_n -RESET_ASYNC_ACTIVE low -ENABLE_NAME {} -ENABLE_ACTIVE high}}
go assembly
go allocate
go extract

