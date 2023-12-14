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
        perror("Error opening file");
        return 1;
    }
    int total = 0;
    while (fgets(buf, BUFLEN, fp)) {
        int matches= 0, prize = 1, winners[PICKH] = {0};
        char *cardTitle, *cardData, *chunk;

        cardTitle = strtok(buf,":");
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
        for (int i = 0; i < matches; i++) {
            prize <<= 1;
        }
        prize >>= 1;
        total += prize;
    }
    printf("total is: %d\n", total);
}