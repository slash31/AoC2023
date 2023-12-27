#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "map.h"

#define BUFLEN 4096


typedef struct Element {
    char *left;
    char *right;
} Element;

typedef map_t(Element) map_element_t;

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
    map_element_t elementMap;
    map_init(&elementMap);
    char directions[1000];
    memset(directions,'\0',1000);
    while (fgets(buf, BUFLEN, fp) != NULL) { 
        if (buf[0] == '\n') {
            break;
        }
        char *ptr = buf;
        int i = 0;
        while (*ptr != '\n') {
            directions[i] = *ptr;
            ptr++;
            i++;
        }
    }

    while (fgets(buf, BUFLEN, fp) != NULL) {
        //printf("buf is %s\n", buf);
        if (buf[0] == '\n') {
            continue;
        }
        Element *element = malloc(sizeof(Element));
        char *elementName = malloc(3 * sizeof(char));
        element->left = malloc(3 * sizeof(char));
        element->right = malloc(3 * sizeof(char));
        if (sscanf(buf, "%3s = (%3s, %3s)", elementName, element->left, element->right) == 3) {
            map_set(&elementMap, elementName, *element);
        }
    }
    
    int stepCounter = 1, directionCounter = 0;
    char *currentElementName = "AAA";
    Element *currentElement = malloc(sizeof(Element));
    while (strncmp(currentElementName,"ZZZ",3) != 0) {
        printf("%s\n", currentElementName);
        if (directions[directionCounter] == '\0') {
            directionCounter = 0;
        }
        currentElement = map_get(&elementMap, currentElementName);
        if (!currentElement) {
            //printf("value for %s not found\n", currentElementName);
        } else {
            if (directions[directionCounter] == 'L') {
                currentElementName = currentElement->left;
            } else if (directions[directionCounter] == 'R') {
                currentElementName = currentElement->right;
            }
            //printf("direction is %c, that element is %s, step counter is %d\n", directions[directionCounter], currentElementName, stepCounter);
            if (strncmp(currentElementName,"ZZZ",3) == 0) {
                printf("Reached ZZZ at step %d\n", stepCounter);
            }
        }
        stepCounter++;
        directionCounter++;
    }
}