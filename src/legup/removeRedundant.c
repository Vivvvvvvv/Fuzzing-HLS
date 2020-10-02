#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAXCHAR 10000


// removeRedundant should   1. remove lines without comment in the current function specified
//                          2. check if any function included in the problem line
//                          3. if included, save to a further reduce file



// check if the str contains the exact variable 
// return 0 if not included
// return greater than 0 if included
int check_if_exist(char *variable, char *str)
{
    char *token1, *token2, *array[MAXCHAR];
    int count = 0, i, j, array_len = 0;

    variable[strcspn(variable, "\r\n")] = '\0';
    token1 = strtok(variable, " ");
    while (token1 != NULL)
    {
        array[array_len] = token1;
        array_len++;
        token1 = strtok(NULL, " (,.=-)");
    }

    str[strcspn(str, "\r\n")] = '\0';
    token2 = strtok(str, " (,.=-)");
    while (token2 != NULL)
    {
        for (j = 0; j < array_len; j++)
        {
            // printf("%s\n", token2);
            if (strcmp(token2, array[j]) == 0)
            {
                // printf("matched %s %s\n", token2, array[j]);
                count++;
            }
        }
        token2 = strtok(NULL, " (,.=-)");
    }

    return count;
}

void remove_char(char * str, char c, int iter)
{
    int i = 0;
    int j;
    int len = strlen(str);
    for (j = 0; j < iter; j++) {
        i = 0;
        while(i<len && str[i]!=c)
            i++;
        while(i < len)
        {
            str[i] = str[i+1];
            i++;
        }
    }
}


int check_if_func(char *variable, int funcNum) {
    //printf("%s\n", variable);
    char* file = "functionName.txt";
    FILE *fp=fopen(file,"r");
    char str[MAXCHAR];
    //str is the FUNC names from txt
    // variable is the passed-in parameter: a line of string
    char *token1, *token2, *array[MAXCHAR]; 
    char *result[MAXCHAR];
    int count = 0, i, array_len = 0, j, result_len = 0;
    

    if (fp == NULL)
    {
        printf("Could not open file %s", file);
        return 1;
    }

    variable[strcspn(variable, "\r\n")] = '\0';
    token1 = strtok(variable, " (,.=-)");
    while(token1 != NULL) {
        if(strstr(token1, "func_") != NULL) {
            array[array_len] = token1;
            //printf("%s\n", token1);
            array_len++;
        }
        token1 = strtok(NULL, " (,.=-)");

    }

    for (i = 0; i < funcNum; i++) {
        fgets(str, MAXCHAR, fp);
        str[strcspn(str, "\r\n")] = '\0';
        token2 = strtok(str, " (,.=-)");
        while (token2 != NULL) {
            for(j = 0; j < array_len; j++) {
                if(strcmp(token2, array[j]) == 0) {
                    //printf("string is %s and token is %s\n", token2, array[j]);
                    //result_len++;
                    //result[result_len] = array[j];
                    //printf("result is %s\n", array[j]);
                    printf("%s\n", token2);
                    count++;
                }
            }
            token2 = strtok(NULL, " (,.=-)");
        }
    } 
    fclose(fp);
    return count;
}

int remove_in_func(char *funcName, int funcNum, int mode)
{
    char func[MAXCHAR], currLine[MAXCHAR], saveReturn[MAXCHAR], prevLine[MAXCHAR];
    strcpy (func, funcName);
    FILE *fp, *fp2;
    char str[MAXCHAR];
    char* filename = "test_reduced.txt";
    char* file2 = "test_removeRedundant.txt";
    int foundfunc1 = 0;
    int enteredMainFunctions = 0;
    int func1Finished = 0;
    int line, i, j;
    char final_printOut[MAXCHAR];
    int bracketF = 0, bracketB = 0, bracketBefore = 0;


    char var[MAXCHAR] = "for if else { } return continue; goto break;";
    char var2[MAXCHAR] = "}", var4[MAXCHAR] = "{";
    char varTemp2[MAXCHAR], varTemp4[MAXCHAR];
    char var3[MAXCHAR] = "uint int ";
    char varTemp[MAXCHAR], varTemp3[MAXCHAR];
    
    char includeFunc[MAXCHAR];
    int funcCount = 0, startRemove = 0;

    fp = fopen(filename, "r");
    fp2 = fopen(file2, "w");
    if (fp == NULL)
    {
        printf("Could not open file %s ", filename);
        return 1;
    } else if (fp2 == NULL) {
        printf("Could not open file %s", file2);
    }
    while (fgets(str, MAXCHAR, fp) != NULL)
    {
        strcpy(currLine, str);
        if (strstr(str, "FUNCTIONS") != NULL) enteredMainFunctions = 1;
        foundfunc1 = 0;
        if (strstr(str, "static") != NULL && check_if_exist(func, str) != 0 && enteredMainFunctions == 1 && func1Finished == 0)
        { // get to func
            foundfunc1 = 1;
            fprintf(fp2, "%s", currLine);
            fgets(str, MAXCHAR, fp);
            strcpy(prevLine, currLine);
            do { //within func
                strcpy(currLine, str); // copy string to currLine so currLine preserves the original str
                strcpy(varTemp, var);
                strcpy(varTemp3, var3);
                strcpy(varTemp2, var2);
                strcpy(varTemp4, var4);
                if (check_if_exist(varTemp, str) != 0)
                { // contains if else for { } block
                    if(mode == 1 ) {
                        fprintf(fp2, "%s", currLine);
                    } else{
                        if (startRemove == 0)fprintf(fp2, "%s", currLine);
                    }
                    strcpy(str, currLine);
                    for (i = 0; str[i] != 0; i++){ {
                        if (str[i] == '}')
                            bracketB++;  
                        else if (str[i] == '{')
                            bracketF++;
                        } 
                    }
    
                } else {
                    strcpy(str, currLine); // reset the str
                    if ((strstr(str, "uint") != NULL || strstr(str, "int") != NULL)&& strstr(str, "safe") == NULL) {
                        // contains uint/int but not in safe math function, also not cast, is declaration, no comment
                       
                        for(i = 0; i <= strlen(str); i++)
                        {
                            if((str[i] == 'g') || (str[i] == 'l'))  
                            {
                                if ((str[i+1] == '_')) break;    	
                            }
                        }
                        strcpy(str, currLine); // reset the str
                        for(j = 0; j <= strlen(str); j++)
                        {
                            if((str[j] == 'u'))  
                            {
                                if ((str[j+1] == 'i') && (str[j+2] == 'n') && (str[j+3] == 't')) break;	
                            } else if ((str[j] == 'i')) {
                                if ((str[j+1] == 'n') && (str[j+2] == 't')) break;
                            } 
                        }
                        // printf("position %d\n", (i+1));
                        // printf("position de %d\n", (j+1));
                        //  fprintf(fp2,"%s", str);
                      
                        if((strstr(str, "//") == NULL) && ((i+1) > (j+1))) {
                            // is declare
                            if(mode == 1) {
                                fprintf(fp2,"%s", currLine);
                            } else {
                                if (startRemove == 0)fprintf(fp2,"%s", currLine);
                            }
                         
                        } else {
                            if (strstr(str, "//") != NULL) {
                            remove_char(currLine, '/', 2);
                            if(mode == 1) {
                                fprintf(fp2,"%s", currLine);
                            } else {
                                strcpy(saveReturn, currLine);
                                fprintf(fp2,"return %s", currLine);
                                startRemove = 1;
                                break;
                            }
                            funcCount += check_if_func(currLine, funcNum);
                        } 
                           
                        }
                    } else if (strstr(str, ":") != NULL) {
                        if(mode == 1) {
                            fprintf(fp2,"%s", currLine);
                        } else {
                            if (startRemove == 0)fprintf(fp2,"%s", currLine);
                        }
                
                    } else {
                        // no uint/int, not declaration, need comment
                        if(strstr(prevLine, "for") != NULL || strstr(prevLine, "if") != NULL) {
                            // cannot remove, initialization
                            if(mode == 1) {
                                fprintf(fp2,"%s", currLine);
                            } else {
                                if (startRemove == 0)fprintf(fp2,"%s", currLine);
                            }

                        } else {
                            //fprintf(fp2,"%s", str);
                            if (strstr(str, "//") != NULL) {
                                remove_char(currLine, '/', 2);
                                if(mode == 1 ) {
                                    fprintf(fp2,"%s", currLine);
                                } else {
                                    strcpy(saveReturn, currLine);
                                    fprintf(fp2,"return %s", currLine);
                                    startRemove = 1;
                                    break;
                                }
                                funcCount += check_if_func(currLine, funcNum);
                            } 
                        }
                    }

                }
            strcpy(str, currLine); // reset the str
            strcpy(prevLine, str);
            }  while (fgets(str, MAXCHAR, fp) != NULL && bracketF > bracketB);
            if(mode == 1) {
                fprintf(fp2, "\n%s", str);
            } else {
                if (startRemove == 1) {
                    bracketBefore = bracketF;
                    while ((bracketF > bracketB) && fgets(str, MAXCHAR, fp) != NULL) {
                        strcpy(varTemp2, var2);
                        strcpy(varTemp3, var3);
                        strcpy(currLine, str);
                        if (check_if_exist(varTemp2, str) != 0) {
                            bracketB++;
                            if(bracketBefore > bracketB) fprintf(fp2,"}\n");
                        } 
                        if (check_if_exist(varTemp4, currLine) != 0) bracketF++;
                    }
                    fprintf(fp2,"}\n");
                    // fprintf(fp2,"return %s}\n", saveReturn);
                }
            }
            func1Finished = 1;
            if(mode == 2) fprintf(fp2, "\n");
        }

        
        if (foundfunc1 == 0)
        {
            fprintf(fp2,"%s", currLine);
        }
        
    }
    fclose(fp);
    fclose(fp2);
    return funcCount;
}

int main(int argc, char *argv[])
{

// two mode:
//         1. keep the commented line and remove other unneed functional lines
//         2. keep the commented line and remove all the lines following it
    // char *funcName = "func_1";
    // int funcNum = 1;
    // int mode = 2;
    
    char *funcName = argv[1];
    int funcNum = strtol(argv[2], NULL, 10);
    int mode = strtol(argv[3], NULL, 10);
        int funcIncluded = remove_in_func(funcName, funcNum, mode);
}