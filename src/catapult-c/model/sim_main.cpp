#include "Vresult.h"
#include "verilated.h"
#include <cstdio>

int main(int argc, char** argv, char** env) {
  Verilated::commandArgs(argc, argv);
  Vresult* top = new Vresult;
  top->rst = 1;
  top->eval();
  top->rst = 0;
  top->eval();

  while (true) {
    top->clk = 0;
    top->eval();
    
    top->clk = 1;
    top->eval();

    top->clk = 0;
    top->eval();

    if (top->return_rsc_triosy_lz) {
      printf("checksum = %X\n", top->return_rsc_dat);
      break;
    }
  }

  delete top;
  exit(0);
}
