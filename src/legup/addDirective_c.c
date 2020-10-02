#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <ctype.h>
#define MAXCHAR 10000

// this function glance through Csmith's random test and find for-loops in the main
// then add a loop name to it named LOOP_x
// Loop pipeline only work within main
int add_Loop_Name(int num) {  
    FILE *fp;
    char str[MAXCHAR];
    char* filename = "test_Mod.txt";
    int foundMain = 0;
    int line, i;
    char add_name[MAXCHAR];
    int count = 0;
    char convert[MAXCHAR];


    fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open file %s",filename);
        return 1;
    }


    while(fgets(str, MAXCHAR, fp) != NULL) {
	if(strstr(str, "main") != NULL) {
	   foundMain = 1;
	}
        count = 0;
		if(strstr(str, "for (") != NULL && num > 0 && foundMain == 1) {
            strcpy(add_name, "\tLOOP_");
            sprintf(convert, "%d:", num);
            strcat(add_name, convert);
            printf("%s", add_name);
            printf("%s\n", str);
            count = 1;
            num--;
		}

        //don't need to print current line if find for and already modified in the
        //above if block
        if(count == 0) {
            printf("%s", str); 
        }
		line++;
	}
	
	//Close the file if still open.
	if(fp) {
		fclose(fp);
	}
    return 0;
}

int main(int argc, char* argv[]) {
    int num = strtol(argv[1], NULL, 10);
    add_Loop_Name(num); 
    return 0;
}
