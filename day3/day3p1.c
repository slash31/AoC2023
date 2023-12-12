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
    IntNode *intList = NULL;
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
                addIntNode(&intList, lineNum, num, ptr - line);
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

    IntNode *icurrent = intList;
    int pnSum = 0;
    while (icurrent != NULL) {
        int isPN = 0;
        for (int i = -1; i <= 1; i++) {
            int lineIndex = icurrent->line + i;

            if (lineIndex < 0 || lineIndex >= MAX_LINES || charLists[lineIndex] == NULL) {
                continue;
            }
            CharNode *ccurrent = charLists[lineIndex];
            while (ccurrent != NULL) {
                int lPos = icurrent->offset - 1;
                int hPos = icurrent->offset + countDigits(icurrent->val);                
                if (ccurrent->offset <= hPos && ccurrent->offset >= lPos) {
                    isPN = 1;
                    break;
                }
                ccurrent = ccurrent->next;
            }

            if (isPN) {
                break;
            }
        }
        if (isPN) {
            pnSum += icurrent->val; 
        }
        icurrent = icurrent->next;
    }
    printf("Sum of all part numbers is %d\n", pnSum);

    return 0;
}