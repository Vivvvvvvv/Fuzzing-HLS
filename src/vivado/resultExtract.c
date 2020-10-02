#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <ctype.h>
#define MAXCHAR 10000





int main() {
    FILE *fp;
    FILE *fp_tmp;
    char str[MAXCHAR];
    char* filename = "vivado_hls.log";
    int foundDEC = 0;
    int line, i;
    char word[MAXCHAR];
    char atLine[MAXCHAR];
    char wordCount;
    int count = 0;
    int start = 0;
    int resultFound = 0;
    fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open file %s",filename);
        return 1;
    }
 

    // fgets read a line  fgets(str, n, stream)
    // str: pointer to an array oc chars where the string is stored
    // n: max num of char to be read
    // stream: pointer to file
    // this block moves to the line before declarations
    
    while(fgets(str, MAXCHAR, fp) != NULL && foundDEC < 2) {
        if(strstr(str, "C TB testing") != NULL) {
            start = 1;
            printf("The c simulation result is...\n");
        }else if(strstr(str, "C post checking") != NULL) {
            start = 1;
            printf("The C/RTL cosimulation result is...\n");
        }
        if(strstr(str, "Verilog result is")!= NULL && start == 1) {
            printf("%s", str);
            if(fgets(str, MAXCHAR, fp) != NULL && strstr(str, "C output")) printf("%s", str);
            resultFound = 1;
        } 
		if(strstr(str, "golden") != NULL && start == 1 && resultFound == 1) {
            printf("%s\n", str);
            foundDEC += 1;
            start = 0;
            resultFound = 0;
		}
		line++;
	}


    
	//Close the file if still open.
	if(fp) {
		fclose(fp);
	}
    return 0;
}