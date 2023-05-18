#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAXCHAR 10000

// FORWARD DECLARETION
int stringNullorEmpty(char *str);

// Functions
// This function checks if string is null or is empty
// return 0 if not null or empty
// return 1 if is null or empty
int stringNullorEmpty(char *str)
{
    if (str != NULL && str[0] != '\0')
        return 0; //not null or empty
    return 1;
}

int main()
{
    FILE *fp;
    int simE[1000], cosimE[1000], noR[1000], prg[1000];
    char str[MAXCHAR];
    char *filename = "resultFile.txt";
    int simError = 0;
    int cosimError = 0;
    int noResultCount = 0, prgError = 0;
    int line, i, j;
    int count = 0;
    int checked = 0;

    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Could not open file %s", filename);
        return 1;
    }

    // fgets read a line  fgets(str, n, stream)
    // str: pointer to an array of chars where the string is stored
    // n: max num of char to be read
    // stream: pointer to file
    // this block moves to the line before declarations
    int cSimError = 0;
L2:
    while (fgets(str, MAXCHAR, fp) != NULL)
    {
        int outputLineCount = 0;
        cSimError = 0;

        if (strstr(str, "Test") != NULL)
        {
            count++;
            if (strstr(str, "No C output") != NULL)
            {
                // printf("Test %d: Unvalid test due to C gives no output.\n", count);
                //  means no c output
                printf("Test %d: NOC\n", count);
                noResultCount++;
                noR[noResultCount] = count;
            }
            else
            {
                if (fgets(str, MAXCHAR, fp) != NULL)
                    // printf("%s\n", str);

                if (strstr(str, "Wrong Pragma") != NULL || strstr(str, "C exit") != NULL)
                {
                    // printf("Test %d: Unvalid test due to wrong pragma usage. \n", count);
                    // PRG means pragma
                    printf("Test %d: PRG\n", count);
                    prgError++;
                    prg[prgError] = count;
                    goto L2;
                }
                else
                {
                    checked = 0;
                    for (i = 0; i < 2; i++)
                    {
                        if (i == 0)
                        {
                            //printf("C simulation result %d: ", count);
                            outputLineCount = -1;
                        }
                        else
                        {
                            //printf("Cosimulation result %d: ", count);
                            outputLineCount = -3;
                        }
                        do
                        {
                            fgets(str, MAXCHAR, fp); // get to the result line
                            outputLineCount++;

                        } while (strstr(str, "golden") == NULL && stringNullorEmpty(str) == 0 && outputLineCount <= 3 && strstr(str, "C exit status") == NULL);
                        // printf("outputLine count %d\n", outputLineCount);
                        if (outputLineCount != 2)
                        { // should reach the result line in three iter
                            if (i == 0)
                                printf("Something wrong happened here.. No C simulation result!\n");
                            else
                            {
                                if (cSimError == 1)
                                {
                                    // CSIM
                                    printf("Test %d: CSIM\n", count);
                                    //printf("C simulation error!\n");
                                }
                                else
                                {
                                    // TIM
                                    printf("Test %d: TIM\n", count);
                                    //printf("Something wrong happened here.. Vivado HLS timeout. No cosimulation result!\n");
                                    noResultCount++;
                                    noR[noResultCount] = count;
                                }
                            }
                        }
                        else
                        { //found result
                            if (strstr(str, "PASS") != NULL)
                            {
                                //printf("Passed!\n");
                                if (i == 0)
                                    checked = 1;
                                if (i == 1 && checked == 1)
                                {
                                    // PA means passed
                                    printf("Test %d: PA\n", count);
                                }
                            }
                            else
                            {
                                if (i == 0)
                                { //c simulation error
                                    simError++;
                                    cSimError = 1;
                                    simE[simError] = count;
                                }
                                else
                                {
                                    cosimError++;
                                    cosimE[cosimError] = count;
                                }
                                // UN means unmatched result
                                if (i == 1)
                                    printf("Test %d: UN\n", count);
                                // printf("Unmatch!\n");
                            }
                        }
                    }
                }
            

        }
            line++;
        }
    }
        printf("\n%d tests performed:\n%d tests have C simulation error, \n%d tests have C/RTL cosimulation error, \n%d tests do not produce results, \n%d tests have pragma error.\n", count, simError, cosimError, noResultCount, prgError);
        if (simError != 0)
            printf("C simulation error tests are: ");
        else
            printf("C simulation error tests are: None!");
        for (i = 1; i <= simError; i++)
        {
            printf("Test %d;", simE[i]);
        }
        printf("\n");
        if (cosimError != 0)
            printf("Cosimulation error tests are: ");
        else
            printf("Cosimulation error tests are: None!");
        for (i = 1; i <= cosimError; i++)
        {
            printf("Test %d; ", cosimE[i]);
        }
        printf("\n");
        if (noResultCount != 0)
            printf("Tests that didn't produce an output: ");
        else
            printf("Tests that didn't produce an output: None!\n");
        for (i = 1; i <= noResultCount; i++)
        {
            printf("Test %d; ", noR[i]);
        }
        printf("\n");
        if (prgError != 0)
            printf("Tests that have pragma error: ");
        else
            printf("Tests that have pragma error: None!\n");
        for (i = 1; i <= prgError; i++)
        {
            printf("Test %d; ", prg[i]);
        }
        printf("\n");
        //Close the file if still open.
        if (fp)
        {
            fclose(fp);
        }
        return 0;
    }
