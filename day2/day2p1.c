#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFLEN 4096
#define INFILE "input.txt"
#define RED_MAX 12
#define GREEN_MAX 13
#define BLUE_MAX 14


int isValid(char * color, int count) {
    if (!strcmp(color,"red")) {
        if (count > RED_MAX) {
            return 0;
        }
    } else if (!strcmp(color,"green")) {
        if (count > GREEN_MAX) {
            return 0;
        }
    } else if (!strcmp(color,"blue")) {
        if (count > BLUE_MAX) {
            return 0;
        }
    }
    return 1;

}
int main() {
    char buf[BUFLEN] = "";
    FILE *fp = fopen(INFILE, "r");
    int gameTotal = 0;
    while (fgets(buf, BUFLEN, fp)) {
        char *gameTitle, *gameData, *plays;
        char *saveptr1, *saveptr2;
        int gameNum;
        gameTitle = strtok(buf,":");
        sscanf(gameTitle, "Game %d", &gameNum);
        gameData = strtok(NULL,":");
        int valid = 1;
        for (plays = strtok_r(gameData, ";", &saveptr1); plays != NULL; plays = strtok_r(NULL, ";", &saveptr1)) {
            char *result;
            for (result = strtok_r(plays, ",", &saveptr2); result != NULL; result = strtok_r(NULL, ",", &saveptr2)) {
                char color[5];
                int count;
                sscanf(result, "%d %s", &count, color);
                valid = isValid(color, count);
                if (!valid) {
                    break;
                }
            }
            if (!valid) {
                break;
            }
        }
        if (valid) {
            gameTotal += gameNum;
        }
    }
    printf("Sum of valid game numbers: %d\n", gameTotal);
}


