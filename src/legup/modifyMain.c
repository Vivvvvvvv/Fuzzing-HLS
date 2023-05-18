#include "helper.h"

// find the variables that send for hashing
// take in a number indicating which variable to comment out
int find_hash(int num)
{
    FILE *fp;
    char str[MAXCHAR];
    char *filename = "test.txt";
    int foundMain = 0;
    int needComment = 0;
    int line, i;
    char final_printOut[MAXCHAR];
    int count = 1;
    int commented = 0;

    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Could not open file %s", filename);
        return 1;
    }
    while (fgets(str, MAXCHAR, fp) != NULL)
    {
        commented = 0;
        if (foundMain == 1 && strstr(str, "return 0") != NULL)
            foundMain = 0;
        else if (foundMain == 1 && strstr(str, "return 0") == NULL)
            foundMain = 1;
        else
            foundMain = 0; // reset

        if (strstr(str, "main") != NULL && foundMain == 0)
            foundMain = 1;

        if (strstr(str, "transparent_crc") != NULL && foundMain == 1)
        {
            if (count == num)
            {
                printf("\t//%s", str);
                commented = 1;
                char *variable;
                variable = strtok(str, "(,.-)");
                for (i = 0; i < 1; i++)
                    variable = strtok(NULL, " ,.-");
                printf("//Removed variable is %s\n", variable);
            }
            count++;
        }

        if (commented == 0)
        {
            printf("%s", str);
        }
    }
    fclose(fp);
    return 0;
}

// comment one line out based on the parametess: functon name, lines number, and if want to print the message 
int comment_func(char *funcName, int num, int ifprint)
{
    char func[MAXCHAR];
    strcpy(func, funcName);
    //printf("function is %s", func);
    FILE *fp;
    char str[MAXCHAR], currLine[MAXCHAR], prevLine[MAXCHAR];
    char *filename = "test.txt";
    int foundfunc1 = 0;
    int enteredMainFunctions = 0;
    int func1Finished = 0;
    int i, j;
    char final_printOut[MAXCHAR];
    int forLoop = 0;
    int lineCount = 0, bracketF = 0, bracketB = 0;

    char var[MAXCHAR] = "for if else { } return continue; goto break;";
    char var3[MAXCHAR] = "uint int ";
    char varTemp[MAXCHAR], varTemp3[MAXCHAR];

    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Could not open file %s", filename);
        return 1;
    }
    while (fgets(str, MAXCHAR, fp) != NULL)
    {
        strcpy(currLine, str);
        if (strstr(str, "FUNCTIONS") != NULL)
            enteredMainFunctions = 1;
        foundfunc1 = 0;

        if (strstr(str, "static") != NULL && check_if_exist(func, str) != 0 && enteredMainFunctions == 1 && func1Finished == 0)
        { // get to func
            foundfunc1 = 1;
            if(ifprint) printf("%s", currLine);
            fgets(str, MAXCHAR, fp);
            strcpy(prevLine, currLine);
            do {                          //within func
                strcpy(currLine, str); // copy string to currLine so currLine preserves the original str
                strcpy(varTemp, var);
                strcpy(varTemp3, var3);
                if (check_if_exist(varTemp, str) != 0)
                { // contains if else for { } block return
                    if(ifprint) printf("%s", currLine);
                    strcpy(str, currLine);
                    for (i = 0; str[i] != 0; i++){ {
                        if (str[i] == '}')
                            bracketB++;  
                        else if (str[i] == '{')
                            bracketF++;
                        } 
                    }
                }
                else
                {
                    strcpy(str, currLine); // reset the str
                    if ((strstr(str, "uint") != NULL || strstr(str, "int") != NULL) && strstr(str, "safe") == NULL)
                    {
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
                        // contains uint/int but not in safe math function, is declaration, no comment
                        if((i+1) > (j+1)) {
                            if (ifprint) printf("%s", currLine);
                        } else {
                            lineCount++;
                        
                            if(ifprint) {
                                if (lineCount == num) {
                                    printf("//%s", currLine);
                                }else {
                                    printf("%s", currLine);
                                }
                            }
                        }
                    } else if (strstr(str, ":") != NULL) {
                        if(ifprint) printf("%s", currLine);
                    } else
                    {
                        // contains uint/int within safe math function, not declaration, need comment
                        // no uint/int, not declaration, need comment
                        //printf("comment 2\n");
                        if(strstr(prevLine, "for") != NULL || strstr(prevLine, "if") != NULL) {
                            if(ifprint) printf("%s", currLine);
                        }else {
                            lineCount++;
                            
                            if(ifprint) {
                                if (lineCount == num) {
                                    printf("//%s", currLine);
                                }else {
                                    printf("%s", currLine);
                                }
                            }
                        }
                        //printf("count %d", lineCount);
                    }
                }
                // printf("forward %d backward %d\n", bracketF, bracketB);
                strcpy(str, currLine); // reset the str
                strcpy(prevLine, str);
            } while (fgets(str, MAXCHAR, fp) != NULL && bracketF > bracketB);
            func1Finished = 1;
        }

        if (foundfunc1 == 0 && ifprint)
        {
            printf("%s", currLine);
        }
       
        
    }

    if(!ifprint) printf("//Possible comment out lines %d\n", lineCount);
    fclose(fp);
    return 0;
}


// find the random variables declared
// all the variable is first saved to hashVariable.txt
// the varaible declerad as array-type is then saved to arrayVariable.txt
int count_hash_var()
{
    FILE *fpread;
    FILE *fpwrite, *fpwrite2;
    char str[MAXCHAR], str1[MAXCHAR];
    char *filename1 = "test.txt";
    char *filename2 = "hashVariables.txt";
    char *filename3 = "arrayVariables.txt";
    fpread = fopen(filename1, "r");
    fpwrite = fopen(filename2, "w");
    fpwrite2=fopen(filename3, "w");

    if (fpread == NULL)
    {
        printf("Could not open file %s", filename1);
        return 1;
    }
    else if (fpwrite == NULL)
    {
        printf("Could not open file %s", filename2);
        return 1;
    } else if(fpwrite2 == NULL) {
        printf("Could not open file %s", filename3);
        return 1;
    }

    int foundMain = 0;
    int line, i;
    char final_printOut[MAXCHAR];
    int count = 0, arrayCount = 0;

    while (fgets(str, MAXCHAR, fpread) != NULL)
    {
        if (foundMain == 1 && strstr(str, "return 0") != NULL)
            foundMain = 0;
        else if (foundMain == 1 && strstr(str, "return 0") == NULL)
            foundMain = 1;
        else
            foundMain = 0; // reset

        if (strstr(str, "main") != NULL && foundMain == 0)
            foundMain = 1;

        if (strstr(str, "transparent_crc") != NULL && foundMain == 1)
        {
            count++;
            char *variable;
            variable = strtok(str, "(,.-)");
            for (i = 0; i < 1; i++)
                variable = strtok(NULL, " ,.-");
            fprintf(fpwrite, "%s\n", variable);

            if(strstr(variable, "[") != NULL) {
                arrayCount++;
                char *name;
                name = strtok(variable, "(,.-)[]");
                fprintf(fpwrite2, "%s\n", name);
            }      
        }
    }

    printf("//Total hash variables %d\n", count);
    printf("//Total array variables %d\n", arrayCount);
    fclose(fpread);
    fclose(fpwrite);
    fclose(fpwrite2);
    return 0;
}

int get_func_name()
{
    FILE *fpread;
    FILE *fpwrite;
    char str[MAXCHAR], str1[MAXCHAR];
    char *filename1 = "test.txt";
    char *filename2 = "functionName.txt";

    int foundLoop = 0;
    int line, i;
    char add_name[MAXCHAR];
    int count = 0;
    char convert[MAXCHAR];

    fpread = fopen(filename1, "r");
    fpwrite = fopen(filename2, "w");

    if (fpread == NULL)
    {
        printf("Could not open file %s", filename1);
        return 1;
    }
    else if (fpwrite == NULL)
    {
        printf("Could not open file %s", filename2);
        return 1;
    }

    while (fgets(str, MAXCHAR, fpread) != NULL)
    {
        if (strstr(str, "FORWARD") != NULL)
        {
            do
            {
                fgets(str, MAXCHAR, fpread);
                int j = 0;
                if (strstr(str, "func_") != NULL)
                {
                    char *start = strstr(str, "func_");
                    char *end = strstr(str, "(");
                    char *name = (char *)calloc(1, end - start + 1);
                    memcpy(name, start, end - start);
                    fprintf(fpwrite, "%s\n", name);
                }
            } while (strstr(str, "FUNCTIONS") == NULL);
        }
    }

    //Close the file if still open.
    fclose(fpread);
    fclose(fpwrite);
    return 0;
}

// replace the main with own declaration
// Also, count how many for loops in the random program
int modify()
{
    FILE *fp;
    FILE *fp_tmp;
    char str[MAXCHAR];
    char *filename = "test.txt";
    int foundDEC = 0;
    int foundFunc = 0;
    int foundInc = 0;
    int line, i;
    char final_printOut[MAXCHAR];
    char word[MAXCHAR];
    char atLine[MAXCHAR];
    char wordCount;
    int count1 = 0;
    int count2 = 0;
    int count0 = 0;
    int countFor = 0;
    int foundMain = 0;

    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Could not open file %s", filename);
        return 1;
    }

    // fgets read a line  fgets(str, n, stream)
    // str: pointer to an array oc chars where the string is stored
    // n: max num of char to be read
    // stream: pointer to file
    // this block moves to the line before declarations

    while (fgets(str, MAXCHAR, fp) != NULL)
    {
        //printf("%s\n", str);
        foundDEC = 0;
        foundFunc = 0;
        foundInc = 0;

        if (strstr(str, "include") != NULL && foundInc == 0 && count0 == 0)
        {

            strcpy(final_printOut, "#include \"/home/legup/legup-4.0/examples/tryCsmith/runtime/csmith.h\"");
            printf("%s\n", final_printOut);
            foundInc = 1;
            count0++;
        }

	if (strstr(str, "main") != NULL)
        {
            foundMain = 1;
        }
	if (strstr(str, "for (") != NULL && foundMain == 1)
        {
            countFor++;
        }

        if (strstr(str, "main") != NULL && foundDEC == 0 && count1 == 0)
        {
            strcpy(final_printOut, "int main() ");
            printf("%s\n", final_printOut);
            foundDEC = 1;
            count1++;
        }

        if (strstr(str, "end") != NULL && count1 != 0 && count2 == 0)
        {
            printf("%s", str);
            strcpy(final_printOut, "\tunsigned int str = crc32_context ^ 0xFFFFFFFFUL; ");
            printf("%s\n", final_printOut);
            // here %% is for printing out the percent sign
            // \\ is for printing out the \

            printf("\tprintf \(\"return_val= %%u\\n\" , str);\n");
            foundFunc = 1;
            count2++;
        }

        if (strstr(str, "return") != NULL && count2 != 0)
        {
            strcpy(final_printOut, "\treturn (int)str;");
            printf("%s\n", final_printOut);
            foundFunc = 1;
        }

        if (foundDEC == 0 && foundFunc == 0 && foundInc == 0)
        {
            printf("%s", str);
        }
        line++;
    }

    //Close the file if still open.
    if (fp)
    {
        fclose(fp);
    }

    printf("//Total number of for-loop is %d\n", countFor);
    return 0;
}

int main(int argc, char *argv[])
{
    // three parametes: 1. condition(modify or need to comment which)
    //                  2. which function
    //                  3. which line/variable count

    int condition = strtol(argv[1], NULL, 10);
    char* funcName = argv[2];
    int num = strtol(argv[3], NULL, 10); // specify which line or variable need to comment

    // int condition = 0;
    // char *funcName = "func_1";
    // int num = 1;
    if (condition == 0)
    {
        // regular modify
        modify();
        get_func_name();
        count_hash_var();
        comment_func(funcName, num, 0);
        //check_array();
    }
    else if (condition == 1)
    {
        // add comment to the func specified
        comment_func(funcName, num, 1);
    }
    else if (condition == 2)
    {
        // add comment to hash variable specified
        find_hash(num);
    }
}
