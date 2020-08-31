
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <ctype.h>
#define MAXCHAR 10000

// Didn't include:
// Loop Initiation Interval: because this might casue compilation errors out
// Loop-Carried Dependencies: because loop might have dependencies, cannot be automatically ignored,
//                            Incorrect usage of the ivdep pragma might introduce functional errors in hardware.
// 
char* randomSelect() {
    char* prg;
    char c1[MAXCHAR], c2[MAXCHAR]; 
    int a = rand() & 2;
     //int a = 1;
    int b = 0;
    if (a == 0) {
        // coalesce nested loops into a single loop without affecting the loop functionality
        prg="#pragma loop_coalesce";
    } else if ( a == 1) {
        prg="#pragma unroll";
    } else {
        b = rand() & 5;
        strcpy(c1, "#pragma max_concurrency ");
        sprintf(c2, "%d", b);
        strcat(c1, c2);
        prg=c1;
    } 
    return prg;
}

// this function glance through Csmith's random test and find the for loop
// then add a loop name to it named LOOP_1
int add_Loop_Name(int num) {  
    FILE *fp, *fp2;
    char str[MAXCHAR];
    char* filename = "test.txt";
    char* file2 = "test_Mod.txt";
    char* selectedPrag;
    int line, i;
    char add_name[MAXCHAR];
    int find = 0;
    int count = num;
    char convert[MAXCHAR];

    fp = fopen(filename, "r");
    fp2 = fopen(file2, "w");
    if (fp == NULL){
        printf("Could not open file %s",filename);
        return 1;
    } else if (fp2 == NULL){
        printf("Could not open file %s",file2);
        return 1;
    }
    int foundFunc = 0;
    while(fgets(str, MAXCHAR, fp) != NULL) {
        find = 0;
        if(strstr(str, "component") != NULL) foundFunc = 1;
		if(strstr(str, "for (") != NULL && num > 0 && foundFunc == 1) {
            int a = rand() & 2;
            if(a == 1) {
                strcpy(add_name, "\tLOOP_");
                sprintf(convert, "%d:", num);
                strcat(add_name, convert);
                fprintf(fp2,"%s\n", add_name);
                selectedPrag=randomSelect();
                fprintf(fp2,"\t%s\n", selectedPrag);
                fprintf(fp2,"%s\n", str);
                find = 1;
                num--;
            } else {
                num--;
            }
		}

        //don't need to print current line if find for and already modified in the
        //above if block
        if(find == 0) {
            fprintf(fp2,"%s", str); 
        }
		line++;
	}
	
	//Close the file if still open.
		fclose(fp);
        fclose(fp2);

    return 0;
}

int getForNum() {
    FILE *fp;
    char str[MAXCHAR];
    char* filename = "numFor.txt";

    int max = 0;
    fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open file %s",filename);
        return 1;
    } 
    if(fgets(str, MAXCHAR, fp) != NULL) {
        max = strtol(str, NULL, 10);
    }
     fclose(fp);
    return max;
}

int main()
{  
    FILE *fp, *fp2;
    char str[MAXCHAR];
    char* filename = "test.txt";
    char* file2 = "test_Mod.txt";

    fp = fopen(filename, "r");
    fp2 = fopen(file2, "w");
    if (fp == NULL){
        printf("Could not open file %s",filename);
        return 1;
    } else if (fp2 == NULL){
        printf("Could not open file %s",file2);
        return 1;
    }

    int maxNum = getForNum();

    if(maxNum != 0) {
        int num = rand() % maxNum;
        add_Loop_Name(num);
    } else {
        while(fgets(str, MAXCHAR, fp) != NULL) fprintf(fp2,"%s", str); 
    }

    fclose(fp);
    fclose(fp2);
    return 0;
}