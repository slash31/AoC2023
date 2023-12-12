#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_LINES 250


typedef struct IntNode {
    int line;
    int val;
    int offset;
    struct IntNode *next;
} IntNode;

typedef struct CharNode {
    int line;
    int offset;
    char symbol;
    struct CharNode *next;
} CharNode;

void addIntNode(IntNode **head, int line, int val, int offset) {
    IntNode *newNode = (IntNode *)malloc(sizeof(IntNode));
    newNode->line = line;
    newNode->val = val;
    newNode->offset = offset;
    newNode->next = *head;
    *head = newNode;
}


void addCharNode(CharNode **head, int line, int offset, char symbol) {
    CharNode *newNode = (CharNode *)malloc(sizeof(CharNode));
    newNode->line = line;
    newNode->offset = offset;
    newNode->symbol = symbol;
    newNode->next = *head;
    *head = newNode;
}


int countDigits(int n) {
    if (n == 0) return 1;
    int digits = 0;
    while (n != 0) {
        n /= 10;
        digits++;
    }
    return digits;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    const char *filename = argv[1];
    IntNode *intLists[MAX_LINES] = {NULL};
    CharNode *charLists[MAX_LINES] = { NULL };
    
    FILE *file = fopen(filename, "r");
    char line[1024];

    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }
    int lineNum = 1;
    while (fgets(line, sizeof(line), file)) {
        int n = 0, num;
        char *ptr = line;

        while (*ptr) {
            if (*ptr == '+' || *ptr == '-') {
                addCharNode(&charLists[lineNum], lineNum, ptr - line, *ptr);
                ptr++;
            } else if (sscanf(ptr, "%d%n", &num, &n) == 1) {
                addIntNode(&intLists[lineNum], lineNum, num, ptr - line);
                ptr += n;
            } else {
                if (*ptr != '.' && !isspace(*ptr)) {
                    addCharNode(&charLists[lineNum], lineNum, ptr - line, *ptr);
                }
                ptr++;
            }
        }
        lineNum++;
    }

    fclose(file);


    int gearSum = 0;

    for (int i = 0; i < MAX_LINES; i++) {
        CharNode *ccurrent = charLists[i];
        while (ccurrent != NULL) {
            if (ccurrent->symbol == '*') {
                // see how many integers are adjacent
                int g1=0, g2=0;
                for (int j = -1; j <= 1; j++) {
                    int lineIndex = ccurrent->line + j;
                    if (lineIndex < 0 || lineIndex >= MAX_LINES || intLists[lineIndex] == NULL) {
                        continue;
                    }
                    IntNode *icurrent = intLists[lineIndex];
                    while (icurrent != NULL) {
                        int lPos = icurrent->offset - 1;
                        int hPos = icurrent->offset + countDigits(icurrent->val);
                        if (ccurrent->offset <= hPos && ccurrent->offset >= lPos) {
                            if (g1 == 0) {
                                g1 = icurrent->val;
                            } else if (g2 == 0) {
                                g2 = icurrent->val;
                            } else {
                                // too many integers, go to the next symbol
                                goto NEXT_SYMBOL;            
                            }
                        }
                        icurrent = icurrent->next;
                    }
                }
                if (g1 > 0 && g2 > 0) {
                    gearSum += g1*g2;
                }
            }
NEXT_SYMBOL:
            ccurrent = ccurrent->next;
        }
    }

    printf("Sum of all gears is %d\n", gearSum);

    return 0;
}