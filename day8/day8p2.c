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

long getGCD(long a, long b) {
    if (b == 0) 
        return a;
    return getGCD(b, a % b);
}


long getLCM(long a, long b) {
    long gcd = getGCD(a, b);
    long lcm = (a / gcd) * b;
    //printf("gcd of %ld and %ld is %ld; lcm is %ld\n", a, b, gcd, lcm);
    return lcm;
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

    int startingElementCounter = 0;
    char *currentElementName[6];
    while (fgets(buf, BUFLEN, fp) != NULL) {
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
        if (elementName[2] == 'A') {
            //printf("adding %s\n", elementName);
            currentElementName[startingElementCounter] = elementName;
            startingElementCounter++;
        }
    }
    // for (int i=0; i < 6; i++) {
    //     printf("currentElementName %d is %s\n", i, currentElementName[i]);
    // }
    
    int stepCounter = 0, directionCounter = 0;
    Element *currentElement = malloc(sizeof(Element));
    int resCount = 0;
    long lcms[6];
    memset(lcms, 0, sizeof(lcms));
    while (resCount < 6) {
        if (directions[directionCounter] == '\0') {
            directionCounter = 0;
        }
        for (int i=0; i < 6; i++) {
            char * thisElementName = currentElementName[i];
            currentElement = map_get(&elementMap, thisElementName);
            if (!currentElement) {
                printf("value for %s not found\n", thisElementName);
            } else {
                if (directions[directionCounter] == 'L') {
                    currentElementName[i] = currentElement->left;
                } else if (directions[directionCounter] == 'R') {
                    currentElementName[i] = currentElement->right;
                }
            }
            // if (i == 2) {
            //     printf("%s\n", currentElementName[i]);
            // }
        }
        // for (int i=0; i < 6; i++) {
        //     printf("currentElementName %d is %s\n", i, currentElementName[i]);
        // }
        stepCounter++;
        directionCounter++;
        int zcounter = 0;
        for (int i=0; i < 6; i++) {
            if (currentElementName[i][2] == 'Z' && lcms[i] == 0) {
                lcms[i] = stepCounter;
                //printf("set lcms index %d to %ld\n", i, lcms[i]);
                resCount++;
            }
        }
    }

    long lcm = lcms[0];
    for (int i = 1; i < 6; i++) {
        lcm = getLCM(lcm,lcms[i]);
    }
    
    printf ("All current elements end with z at step %ld\n", lcm);
}