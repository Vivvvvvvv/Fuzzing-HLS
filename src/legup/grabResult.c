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
    int simE[1000], noRC[1000], noRV[1000];
    char str[MAXCHAR];
    char *filename = "resultFile.txt";
    int simError = 0;
    int noResultCountC = 0, noResultCountV = 0;
    int i, j;
    int count = 0;

    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Could not open file %s", filename);
        return 1;
    }

    int cSimError = 0;
    while (fgets(str, MAXCHAR, fp) != NULL)
    {
        int outputLineCount = 0;
        cSimError = 0;
        if (strstr(str, "Test") != NULL)
        {
            count++;
            if (strstr(str, "No C output") != NULL)
            { // no c output found
                printf("Test %d: Unvalid test due to C gives no output.\n", count);
                noResultCountC++;
                noRC[noResultCountC] = count;
                cSimError = 1; // no c result found
            } else if (strstr(str, "Stimulation problem!") != NULL) {
                printf("Test %d: Assertion while synthesizing.\n", count);
                noResultCountV++;
                noRV[noResultCountV] = count;
            } else if (strstr(str, "No Verilog output") != NULL) {
                printf("Test %d: timeout.\n", count);
                noResultCountV++;
                noRV[noResultCountV] = count;
            }
            else
            { // c does produced an output
                printf("Test %d: ", count);
                do
                {
                    fgets(str, MAXCHAR, fp); // get to the result line
                    //printf("%s", str);
                    outputLineCount++;
                } while (strstr(str, "Result") == NULL && stringNullorEmpty(str) == 0 && outputLineCount <= 5);
		//printf("%d", outputLineCount);
                if (outputLineCount != 4)
                { // should reach the result line in four iter
                    if (cSimError == 1)
                    {
                        printf("C simulation error!\n");
                    }
                    else
                    {
                        printf("Something wrong happened here.. No output!\n");
                        noResultCountV++;
                        noRV[noResultCountV] = count;
                    }
                }
                else
                { //found result
                    if (strstr(str, "does not produce") != NULL)
                    { // verilog no result
                        printf("Verilog no result!\n");
                        noResultCountV++;
                        noRV[noResultCountV] = count;
                    }
                    else if (strstr(str, "does not match") != NULL)
                    { //fails
                        printf("Unmatch!\n");
                        simError++;
                        simE[simError] = count;
                    }
                    else
                    { // match
                        printf("Match!\n");
                    }
                }
            }
        }
    }
    printf("\n%d tests performed:\n%d tests failed, \n%d tests have no Verilog result,\n%d tests have no C result.\n", count, simError, noResultCountV, noResultCountC);
    if (simError != 0)
        printf("Failed tests are: ");
    else
        printf("Failed tests are: None!");
    for (i = 1; i <= simError; i++)
    {
        printf("Test %d;", simE[i]);
    }
    printf("\n");
    if (noResultCountV != 0)
        printf("Tests that didn't produce a Verilog output: ");
    else
        printf("Tests that didn't produce a Verilog output: None!");
    for (i = 1; i <= noResultCountV; i++)
    {
        printf("Test %d; ", noRV[i]);
    }
    printf("\n");
    if (noResultCountC != 0)
        printf("Tests that didn't produce an C output: ");
    else
        printf("Tests that didn't produce an C output: None!");
    for (i = 1; i <= noResultCountC; i++)
    {
        printf("Test %d; ", noRC[i]);
    }
    printf("\n");

    //Close the file if still open.
    if (fp)
    {
        fclose(fp);
    }
    return 0;
}
