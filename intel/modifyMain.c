#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAXCHAR 10000

// find the random variables declared
// all the variable is first saved to hashVariable.txt
// the varaible declerad as array-type is then saved to arrayVariable.txt
// different with Vivado HLS, this variable is extracted from Global portion
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
    fpwrite2 = fopen(filename3, "w");

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
    else if (fpwrite2 == NULL)
    {
        printf("Could not open file %s", filename3);
        return 1;
    }

    int line, i;
    char final_printOut[MAXCHAR];
    int count = 0, arrayCount = 0, foundVar = 0;

    char *token1, *token2, *array[MAXCHAR];
    int j, array_len = 0;
    char *variable;

    while (fgets(str, MAXCHAR, fpread) != NULL && strstr(str, "FORWARD") == NULL)
    {
        if (strstr(str, "GLOBAL VARIABLES") != NULL && foundVar == 0)
        {
            foundVar = 1;
        }
        if (foundVar == 1 && strstr(str, "g_") != NULL)
        {
            printf("%s", str);
            str[strcspn(str, "\r\n")] = '\0';
            token1 = strtok(str, " ");

            while (token1 != NULL && strstr(token1, ";") == NULL)
            {
                array[array_len] = token1;
                if (strstr(token1, "g_") != NULL)
                {
                    fprintf(fpwrite, "%s\n", token1);
                    count++;
                }
                array_len++;
                token1 = strtok(NULL, " (,.=-)");
            }
        }
    }
    printf("//Total hash variables %d\n", count);
    //printf("//Total array variables %d\n", arrayCount);
    fclose(fpread);
    fclose(fpwrite);
    fclose(fpwrite2);
    return 0;
}

/*
int create_hash(){
    FILE *fpread;
    char str[MAXCHAR], str1[MAXCHAR];
    char *filename1 = "C:\\intelFPGA_lite\\18.1\\hls\\examples\\csmith_test\\hashVariables.txt";
    fpread = fopen(filename1, "r");
    int i, j, index = 0;
    int array[MAXCHAR];
    if (fpread == NULL)
    {
        printf("Could not open file %s", filename1);
        return 1;
    }

    while(fgets(str, MAXCHAR, fpread) != NULL) {
        if(strstr(str, "[") != NULL) {
            char *variable, *arrayVar;
            variable = strtok(str, "[]");
            arrayVar = variable;
            index=0;
            while(variable != NULL) {
                variable = strtok(NULL, "[]");
                printf("%s\n", variable);
                int digit = atoi(variable);
                array[index]=(digit);
                index++;
            }

            for(i=0; i<=index; i++) { 
                printf("%s", arrayVar);
                for(j=0; j<=index; j++) {
                    printf("[%d]", j);
                }

            }

        } else {
            //printf("%s", str);
        }
    }
    
    fclose(fpread);
    return 0;

}
*/

void create_main()
{
    printf("}\n\n");
    printf("int main(void) \n{\n");
    printf("\tunsigned int resultOut = result();\n");
    printf("\tprintf(\"%%X\\n\", resultOut);\n");
    printf("\treturn 0;\n");
}

// replace the main with own declaration
// Also, count how many for loops in the random program
int modify()
{
    FILE *fp, *fp2;
    FILE *fp_tmp;
    char str[MAXCHAR];
    char *filename = "test.txt";
    char *filename2 = "numfor.txt";
    int foundDEC = 0, foundFunc = 0, foundInc = 0, changed = 0, callFun = 0;
    int line, i;
    char final_printOut[MAXCHAR];
    char word[MAXCHAR];
    char atLine[MAXCHAR];
    char wordCount;
    int count1 = 0;
    int count2 = 0;
    int countFor = 0, countDec = 0;

    fp = fopen(filename, "r");
    fp2 = fopen(filename2, "w");
    if (fp == NULL)
    {
        printf("Could not open file %s", filename);
        return 1;
    } else if (fp2 == NULL) {
        printf("Could not open file %s", filename2);
        return 1;
    }

    //printf("#include <stdint.h>\n");
    //printf("#include <inttypes.h>\n");

    // fgets read a line  fgets(str, n, stream)
    // str: pointer to an array oc chars where the string is stored
    // n: max num of char to be read
    // stream: pointer to file
    // this block moves to the line before declarations

    while (fgets(str, MAXCHAR, fp) != NULL)
    {
        foundDEC = 0;
        foundFunc = 0;
        changed = 0;

        if (strstr(str, "include") != NULL && foundInc == 0)
        {
            strcpy(final_printOut, "#include \"D:\\csmith-2.3.0\\runtime\\csmith.h\"");
            printf("%s\n", final_printOut);
            changed = 1;
        }
        if (strstr(str, "for (") != NULL)
        {
            countFor++;
        }
        if (strstr(str, "main") != NULL && foundDEC == 0 && count1 == 0)
        {

            strcpy(final_printOut, "unsigned int result() ");
            printf("%s\n", final_printOut);
            foundDEC = 1;
            count1++;
            countDec++;

        }
        if(countDec != 0) countDec++;

        //save the declare i j k line
        if(countDec == 4 && strstr(str, "int") != NULL) {
            printf("%s",str);
            changed=1;
          
        }

        if (strstr(str, "transparent_crc") != NULL && count1 != 0)
        {

            char *variable;
            variable = strtok(str, "(,-)");
            for (i = 0; i < 1; i++)
                variable = strtok(NULL, " ,-");
            printf("\ttmp ^=%s;\n", variable);
            changed = 1;
        }

        if (strstr(str, "platform") == NULL && (strstr(str, "for") != NULL || strstr(str, "{") != NULL || strstr(str, "}") != NULL) && count1 != 0)
        {

            printf("%s", str);
            changed = 1;
            if (callFun == 0)
            {
                printf("\tfunc_1();\n");
                printf("\tunsigned int tmp = 1;\n");
                callFun = 1;
            }
        }

        if (strstr(str, "end") != NULL && count1 != 0 && count2 == 0)
        {

            strcpy(final_printOut, "\tunsigned int str = tmp ^ 0xFFFFFFFFUL; ");
            printf("%s\n", final_printOut);
            foundFunc = 1;
            count2++;
        }

        if (strstr(str, "return") != NULL && count2 != 0)
        {

            strcpy(final_printOut, "\treturn str;");
            printf("%s\n", final_printOut);
            foundFunc = 1;
            count1 = 0;
            create_main();
             
        }


        if (foundDEC == 0 && foundFunc == 0 && changed == 0 && count1 == 0)
        {
            printf("%s", str);
        }

        line++;
    }
    fprintf(fp2, "%d\n", countFor);
    //Close the file if still open.
    fclose(fp);
    fclose(fp2);
    return 0;
}


int add_component() {
    FILE *fp, *fp2;
    char str[MAXCHAR], final_printOut[MAXCHAR];
    char *filename = "test.txt";
    char *filename2 = "numfor.txt";

    fp = fopen(filename, "r");
    fp2 = fopen(filename2, "w");
    if (fp == NULL)
    {
        printf("Could not open file %s", filename);
        return 1;
    }else if (fp2 == NULL) {
        printf("Could not open file %s", filename2);
        return 1;
    }
    int foundDEC = 0, count = 0, countFor = 0;

    while(fgets(str, MAXCHAR, fp) != NULL) {
        foundDEC = 0;
        if(strstr(str, "result") != NULL && count == 0) {
            strcpy(final_printOut, "component unsigned int result() ");
            printf("%s\n", final_printOut);
            foundDEC = 1;
            count++;
        }

        if (strstr(str, "for (") != NULL && count != 0)
        {
            countFor++;
        }

        if (foundDEC == 0)
        {
            printf("%s", str);
        }
    }
    fprintf(fp2, "%d\n", countFor);
    fclose(fp);
    fclose(fp2);
    return 0;
}

int main(int argc, char *argv[])
{

    int condition = strtol(argv[1], NULL, 10);
    //int condition = 1;
    if(condition == 0) {
        modify();
    } else if(condition == 1) {
        add_component();
    }


    
    return 0;
}