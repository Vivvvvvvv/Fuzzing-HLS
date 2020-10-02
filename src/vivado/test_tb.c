#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <inttypes.h>

unsigned int result();

int main () {
  FILE         *fp;
  FILE         *fp1;
  unsigned int resultOut;
  //unsigned long resultOut;
  char str[1000];
  fp=fopen("out.txt","w");
  fp1=fopen("out.gold.txt", "r");
		// Save the results.
  resultOut = result();
  printf("Verilog result is %X\n", resultOut);
  printf("C output:");
  while (fgets(str, 1000, fp1) != NULL) printf("%s", str);
  fprintf(fp,"checksum = %X\n",resultOut);
    

  fclose(fp);
  fclose(fp1);



  printf ("Comparing against output data \n");
  if (system("diff -w out.txt out.gold.txt")) {

	fprintf(stdout, "*******************************************\n");
	fprintf(stdout, "FAIL: Output DOES NOT match the golden output\n");
	fprintf(stdout, "*******************************************\n");
     return 1;
  } else {
	fprintf(stdout, "*******************************************\n");
	fprintf(stdout, "PASS: The output matches the golden output!\n");
	fprintf(stdout, "*******************************************\n");
     return 0;
  }

}
