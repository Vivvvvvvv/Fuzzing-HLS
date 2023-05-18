#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <ctype.h>
#define MAXCHAR 10000


// this function glance through Csmith's random test and find the for loop
// then add a loop name to it named LOOP_1
int add_Loop_Name(int num) {  
    FILE *fp;
    char str[MAXCHAR];
    char* filename = "test_modify_main.txt";
    //char* filename = argv[1];
    int line, i;
    char add_name[MAXCHAR];
    int find = 0;
    int count = num;
    char convert[MAXCHAR];
    //int num = strtol(argv[2], NULL, 10);

    fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open file %s",filename);
        return 1;
    }

    while(fgets(str, MAXCHAR, fp) != NULL) {
        find = 0;
		if(strstr(str, "for (") != NULL && num > 0) {
            strcpy(add_name, "\tLOOP_");
            sprintf(convert, "%d:", num);
            strcat(add_name, convert);
            printf("%s", add_name);
            printf("%s\n", str);
            find = 1;
            num--;
		}

        //don't need to print current line if find for and already modified in the
        //above if block
        if(find == 0) {
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

int main(int argc, char* argv[])
{   

    int num = strtol(argv[1], NULL, 10);
    add_Loop_Name(num);
    return 0;
}