
#ifndef HELPER_H_
#define HELPER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAXCHAR 10000

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
            if (strcmp(token2, array[j]) == 0)
            {
                count++;
            }
        }
        token2 = strtok(NULL, " (,.=-)");
    }

    return count;
}

#endif //HELPER_H_