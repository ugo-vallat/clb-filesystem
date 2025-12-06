#include <string.h>


char* strcpy(char *dst, const char *src)
{
    int i = 0;
    while (src[i] != '\0') {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
    return dst;
}


char* strcat(char *dst, const char *src)
{
    int i = 0;
    int j = 0;


    while (dst[i] != '\0')
        i++;


    while (src[j] != '\0') {
        dst[i] = src[j];
        i++;
        j++;
    }

    dst[i] = '\0';
    return dst;
}


int strcmp(char *dst, const char *src)
{
    int i = 0;
    while (dst[i] != '\0' && src[i] != '\0') {
        if (dst[i] != src[i])
            return dst[i] - src[i];
        i++;
    }
    return dst[i] - src[i];
}


int strlen(const char *src)
{
    int i = 0;
    while (src[i] != '\0') {
        i++;
    }
    return i;
}

