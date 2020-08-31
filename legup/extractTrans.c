
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <ctype.h>
#include <math.h>
#define MAXCHAR 10000
#define FLT2HEX(a) (*(unsigned*)&a)
#define DBL2HEX(a) (*(unsigned long long*)&a)


// replace the main with own declaration 
int main() {
    FILE *fp;
    FILE *fp_tmp;
    char str[MAXCHAR];
    char* filename = "transcript.txt";
    int foundDEC = 0;
    int foundFunc = 0;
    int line, i, j;
    char final_printOut[MAXCHAR];
    char word[MAXCHAR];
    char atLine[MAXCHAR];
    char wordCount;
    int count1 = 0;
    unsigned long result = 0;

    fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open file %s",filename);
        return 1;
    }


    // fgets read a line  fgets(str, n, stream)
    // str: pointer to an array of chars where the string is stored
    // n: max num of char to be read
    // stream: pointer to file
    // this block moves to the line before declarations
    
    while(fgets(str, MAXCHAR, fp) != NULL && foundDEC == 0) {
	char *ptr = strstr(str, "return_val");
	if(ptr != NULL) {

		printf("%s\n", ptr);
		//result = ptr+11 - '0';
		//printf("%lu\n", result);
		foundDEC = 1;
	}

    
    }
	//Close the file if still open.
	if(fp) {
		fclose(fp);
	}

    return 0;
}


