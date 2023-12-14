#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFLEN 4096
#define PICKH 100


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

    int numLines = 0;
    while (fgets(buf, BUFLEN, fp)) {
        numLines++;
    }

    rewind(fp);

    int *cardsWon;
    cardsWon = (int*)malloc((numLines+1) * sizeof(int));
    if(cardsWon == NULL) {
        fprintf(stderr, "could not calloc\n");
        return 1;
    }
    for (int i = 0; i < numLines; i++) {
        cardsWon[i+1] = 1;
    }

    int total = 0;
    while (fgets(buf, BUFLEN, fp)) {
        int cardNum, matches= 0, winners[PICKH] = {0};
        char *cardTitle, *cardData, *chunk;

        cardTitle = strtok(buf,":");
        sscanf(cardTitle, "Card %d", &cardNum);
        cardData = strtok(NULL,":");

        chunk = strtok(cardData, "|");

        while (*chunk) {
            int num,n;
            if (sscanf(chunk, "%d%n", &num, &n) == 1) {
                winners[num] = 1;
                chunk += n;
            } else {
                chunk++;
            }
        }

        chunk = strtok(NULL, "|");
        while (*chunk) {
            int num,n;
            if (sscanf(chunk, "%d%n", &num, &n) == 1) {
                if (winners[num] == 1){
                    matches += 1;
                }
                chunk += n;
            } else {
                chunk++;
            }
        }
        for (int i = 1; i <= matches; i++) {
            if (cardNum > numLines) {
                break;
            }
            cardsWon[cardNum + i] += 1 * cardsWon[cardNum];
        }
    }
    for (int i = 0; i < numLines; i++) {
        total += cardsWon[i + 1];
    }
    printf("total cards: %d\n", total);
    free(cardsWon);
    return 0;
}