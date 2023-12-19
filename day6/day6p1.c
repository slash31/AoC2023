#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define BUFLEN 4096
#define NUMRACES 4

typedef struct race {
    int time;
    int distance;
    int nw2br;
} race;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr,"Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    const char *filename = argv[1];

    char buf[BUFLEN] = "";
    FILE *fp = fopen(filename, "r");

    if (fp == NULL) {
        perror("Error opening file");
        return 1;
    }
    race raceHx[NUMRACES];
    while (fgets(buf, BUFLEN, fp) != NULL) {
        int rowCount = 0;
        long sectionStart = 0;
        if (strncmp(buf, "Time:", 5) == 0) {
            int n = 0, count = 0, num;
            char *ptr = buf;
            while (*ptr) {
                if (sscanf(ptr, "%d%n", &num, &n) == 1) {
                    raceHx[count].time = num;
                    ptr += n;
                    count ++;
                } else {
                    ptr++;
                }
            }
        } else if (strncmp(buf, "Distance:", 9) == 0) {
            int n = 0, count = 0, num;
            char *ptr = buf;
            while (*ptr) {
                if (sscanf(ptr, "%d%n", &num, &n) == 1) {
                    raceHx[count].distance = num;
                    ptr += n;
                    count ++;
                } else {
                    ptr++;
                }
            }
        }
    }

    long result = 1;
    for (int i = 0; i < NUMRACES; i++) {
        int t = raceHx[i].time;
        int r = raceHx[i].distance;
        // not readable...application of quadratic formula to solve basic
        // algebraic inequality; everything between the roots is better than the
        // existing record
        int v1 = ceil((-t + sqrt(pow(t,2) - 4*r)) / (-2) +0.001);
        int v2 = floor((-t - sqrt(pow(t,2) - 4*r)) / (-2) -0.001);
        raceHx[i].nw2br = 0;
        for (int j = v1; j <= v2; j++) {
            raceHx[i].nw2br++;
        }
        //printf("%d/%d, %d\n", v1, v2, raceHx[i].nw2br);
        result *= raceHx[i].nw2br;
    }

    printf("result is %ld\n", result);
}