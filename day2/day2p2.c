#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFLEN 4096
#define INFILE "input.txt"


int main() {

    char buf[BUFLEN] = "";
    FILE *fp = fopen(INFILE, "r");
    int gameTotal = 0;
    while (fgets(buf, BUFLEN, fp)) {
        char *gameTitle, *gameData, *plays;
        char *saveptr1, *saveptr2;
        int rMax = 0, gMax = 0, bMax = 0, gamePow = 0;
        gameTitle = strtok(buf,":");
        gameData = strtok(NULL,":");
        for (plays = strtok_r(gameData, ";", &saveptr1); plays != NULL; plays = strtok_r(NULL, ";", &saveptr1)) {
            char *result;
            for (result = strtok_r(plays, ",", &saveptr2); result != NULL; result = strtok_r(NULL, ",", &saveptr2)) {
                char color[5];
                int count;
                sscanf(result, "%d %s", &count, color);
                if (!strcmp(color,"red")) {
                    if (count > rMax) {
                        rMax = count;
                    }
                } else if (!strcmp(color,"green")) {
                    if (count > gMax) {
                        gMax = count;
                    }
                } else if (!strcmp(color,"blue")) {
                    if (count > bMax) {
                        bMax = count;
                    }
                }
            }
        }
        gamePow = rMax * gMax * bMax;
        gameTotal += gamePow;
        
    }
    printf("Sum of game powers: %d\n", gameTotal);
}


