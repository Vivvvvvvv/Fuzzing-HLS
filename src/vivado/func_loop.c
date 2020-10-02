#include "helper.h"

int get_func(int funcNum)
{
    FILE *fpread, *fpread2;
    FILE *fpwrite;
    char str[MAXCHAR], str1[MAXCHAR];
    char *filename1 = ".\\test_modify_main.txt";
    char *filename2 = ".\\loopInFunc.txt";
    char *filename3 = ".\\functionName.txt";

    fpread = fopen(filename1, "r");
    fpread2 = fopen(filename3, "r");
    fpwrite = fopen(filename2, "w");

    if (fpread == NULL)
    {
        printf("Could not open file %s", filename1);
        return 1;
    }
    else if (fpread2 == NULL)
    {
        printf("Could not open file %s", filename3);
        return 1;
    }
    else if (fpwrite == NULL)
    {
        printf("Could not open file %s", filename2);
        return 1;
    }

    int foundFunc = 0, inFunc = 0, exitFunc = 0;
    int i;
    char currLine[MAXCHAR];
    int count = 0;

    for(i = 0; i <= funcNum; i++) {
        if(i < funcNum) {
            fgets(str, MAXCHAR, fpread2); // get the func name
        } 
        else if(i == funcNum) strcpy(str, "result\n");
        fprintf(fpwrite, "%s", str);
        fseek(fpread, 0, SEEK_SET);
        if(i < funcNum) foundFunc = -1;
        else foundFunc = 0;
        while(fgets(str1, MAXCHAR, fpread) != NULL && exitFunc == 0) {
            strcpy(currLine, str1);
            if(strstr(str1, "reads") != NULL) foundFunc++;
            if(check_if_exist(str, str1) != 0 && foundFunc == i) {
                inFunc = 1;
            } 


            strcpy(str1, currLine);
            if(inFunc == 1 && (strstr(str1, "reads") != NULL || strstr(str1, "found") != NULL)) {
                //printf("%s\n", str1);
                exitFunc = 1;
            }
        
            if(inFunc == 1 && strstr(currLine, "for (") != NULL) {
                count++;
                //printf("for loop:%s", currLine);
                //fprintf(fpwrite, "LOOP_%d\n", count);   
            }

        }
        fprintf(fpwrite, "%d\n", count);   

        inFunc = 0;
        exitFunc = 0;
        count = 0;

    }

    fclose(fpread);
    fclose(fpwrite);
    fclose(fpread2);
    return 0;
}


int main(int argc, char *argv[])
{
    int funcNum = strtol(argv[1], NULL, 10);
    get_func(funcNum);
    return 0;
}