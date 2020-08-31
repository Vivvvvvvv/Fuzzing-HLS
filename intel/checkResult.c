#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <ctype.h>
#define MAXCHAR 10000

// FORWARD DECLARETION
int stringNullorEmpty(char *str);


// Functions
// This function checks if string is null or is empty
// return 0 if not null or empty
// return 1 if is null or empty 
int stringNullorEmpty(char* str) {
    if(str != NULL && str[0] != '\0') return 0; //not null or empty
    return 1;
}


int main() {
    FILE *fp;
    char str[MAXCHAR];
    char* filename = "resultFile.txt";
    int line, i, j;
    int sim[MAXCHAR], noC[MAXCHAR], noV[MAXCHAR];
    fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open file %s",filename);
        return 1;
    }
 
    int timeout = 0;
    int tim[MAXCHAR];

    unsigned long int synRes = 0, cosimRes = 0;
    int error = 0, count = 0, noResC = 0, noResV = 0;
    while(fgets(str, MAXCHAR, fp) != NULL) {
		if(strstr(str, "Test") != NULL) {
        NEXT:
            count++;
            // printf("Test %d: \n", count);
            // get to synthesis result
            for (i=0; i <=1; i++) {
                    if (fgets(str, MAXCHAR, fp)!= NULL){
                        if(strstr(str, "Test") != NULL){
                            timeout++;
                            tim[timeout] = count;
                            printf("Test %d: TIM\n", count);
                            goto NEXT;
                         }
                    } else {
                        timeout++;
                            tim[timeout] = count;
                        printf("Test %d: TIM\n", count);
                        goto END;
                    }
                } 
            if (strstr(str, "Test") != NULL) {
                // test-x86-64 no result, so test-fpga ignored
                timeout++;
                tim[timeout] = count;
                printf("Test %d: TIM\n", count);
                goto NEXT;
            } else {
                synRes = strtol(str, NULL, 16);
                for (i=0; i <=1; i++) {
                    if (fgets(str, MAXCHAR, fp)!= NULL){
                         if(strstr(str, "Test") != NULL){
                             timeout++;
                            tim[timeout] = count;
                             printf("Test %d: TIM\n", count);
                            goto NEXT;
                         }
                        
                    } else {
                        timeout++;
                        tim[timeout] = count;
                        printf("Test %d: TIM\n", count);
                        goto END;
                    }
                } 
                if(strstr(str, "Test") != NULL){
                    timeout++;
                    tim[timeout] = count;
                    printf("Test %d: TIM\n", count);
                    goto NEXT;
                } else {
                    cosimRes = strtol(str, NULL, 16);
                    if(cosimRes != synRes) {
                        error++;
                        sim[error] = count;
                        printf("Test %d: UN\n", count);
                    } else {
                        printf("Test %d: PA\n", count);
                    }

                }
                

            }
		}
		line++;
	}
END:
    printf("\n%d tests performed: \n%d tests have C/RTL cosimulation error.\n%d tests have timedOut. \n", count, error, timeout);
	if(error != 0) printf("Cosimulation error tests are: ");
    else printf("C simulation error tests are: None!");
    for (i = 1; i <= error; i++) {
        printf("Test %d;", sim[i]);
    }
    printf("\n");

    if(timeout != 0) printf("Timed out tests are: ");
    else printf("Timed out tests are: None!");
    for (i = 1; i <= timeout; i++) {
        printf("Test %d;", tim[i]);
    }
    printf("\n");



	//Close the file if still open.
	if(fp) {
		fclose(fp);
	}
    return 0;
}
