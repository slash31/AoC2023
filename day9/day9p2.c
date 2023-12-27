#include <stdio.h>
#include <stdlib.h>


#define SERIES_LEN 21
#define BUFLEN 4096


int getPreviousElement(int series[], int numElements) {
    int newSeries[numElements-1];
    int allZeroes = 1, diff = 0;
    for (int i = 0; i < numElements - 1; i++) {
        diff = series[i+1] - series[i];
        //printf("diff of %d and %d is %d\n", series[i], series[i-1], diff);
        newSeries[i] = diff;
        if (diff != 0) {
            allZeroes = 0;
        }
    }
    if (allZeroes == 1) {
        return series[0];
    } else {
        return series[0] - getPreviousElement(newSeries, numElements-1);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    
    char buf[BUFLEN] = "";
    FILE *fp = fopen(filename, "r");

    if (fp == NULL) {
        fprintf(stderr, "Error opening file");
        return 1;
    }
    long total = 0;
    while (fgets(buf, sizeof(buf), fp)) {
        int n = 0, num, count = 0;
        int series[SERIES_LEN];
        char *ptr = buf;
        while (*ptr) {
            if (sscanf(ptr, "%d%n", &num, &n) == 1) {
                //printf("adding %d\n", num);
                series[count] = num;
                ptr += n;
                count++;
            } else {
                ptr ++;
            }
        }
        int nextElement = getPreviousElement(series, SERIES_LEN);
        //printf("adding next num %d\n", nextElement);
        total += nextElement;
        //printf("%ld\n", total);
    }
   printf("total is %ld\n", total);
}
