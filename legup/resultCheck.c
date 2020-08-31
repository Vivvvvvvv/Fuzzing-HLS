#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LEN 256
#define MAXCHAR 1000

// This function checks if the HEX to decimal conversion in the c program is correct
int checkConversion(char hex[MAXCHAR], unsigned long int dec) {
    //strtol converts numbers with a specified base
   unsigned long int re = strtol(hex, NULL, 16);
   //printf("Hexadecimal is%sDecimal is %lu\n", hex, re);
   if(re != dec) return 1; // conversion error detected!
   return 0;
}

int main() {
    FILE *fp1;
    FILE *fp2;
    int size1;
    int size2;
    char str1[MAXCHAR], checkSum[MAXCHAR], returnC[MAXCHAR], returnV[MAXCHAR];
    char str2[MAXCHAR];
    char* file1 = "goldenResult.txt";
    char* file2 = "verilogResult.txt";
    fp1 = fopen(file1, "r");
    fp2 = fopen(file2, "r");
    unsigned long int c, v;

    if (fp1 == NULL || fp2 == NULL){
        printf("Could not open file");
        return 1;
    }

    // check if the c's hex matched with the decimal
    if(fgets(str1, MAXCHAR, fp1) != NULL) {
        strcpy(checkSum, str1); // checkSum = str1;
        printf("%s", str1);
    }
    if(fgets(str1, MAXCHAR, fp1) != NULL) {
        strcpy(returnC, str1); //returnC = str1;
        c = strtol(returnC+11, NULL, 10); 
        printf("C output is %s", str1);
    }

    if(checkConversion(checkSum+10, c) == 1) {
       printf("error!");
       return 1; //error found!
    }

    if(fgets(str2, MAXCHAR, fp2) != NULL) {
        v = strtol(str2+11, NULL, 10);
        printf("Verilog output is %s", str2);
    }
   
   if (str2 == NULL || str2[0] == '\0') {
	printf("Result: Verilog does not produce an output!\n\n");
   } else if(c != v) {
        printf("Result: Output does not match!\n\n");
   } else {
	printf("Result: Output matches!\n\n");
   }

   fclose(fp1);
   fclose(fp2);
   return 0;

}
