#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define BUFLEN 4096

typedef struct Node {
    int number;
    struct Node *next;
} Node;

Node* createNode(int number) {
    Node *newNode = malloc(sizeof(Node));
    // should probably check to see if this worked here
    newNode->number = number;
    newNode->next = NULL;
    return newNode;
}

void addNode(Node **head, int number) {
    Node *newNode = createNode(number);
    newNode->next = *head;
    *head = newNode;
}

int matchLongDigit(const char *str) {
    const char *digits[] = {"zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};
    for (int i = 0; i < 10; i++) {
        if (strncmp(str, digits[i], strlen(digits[i])) == 0) {
            return i;
        }
    }
    return -1; 
}

int main () {
    size_t n = 1;
    char buf[BUFLEN] = "";
    FILE *fp = fopen ("input.txt", "r");
    
    Node *num_list = NULL;

    while (fgets (buf, BUFLEN, fp)) {
        size_t len = strlen(buf);

        int num = 0;

        if (len > 1)  {
            for (int i = 0; i < len; i++) {
		if (isdigit(buf[i])) {
                    num  += 10*(buf[i] - '0');
                    break;
	        } else {	
                    int digit = matchLongDigit(&buf[i]);
                    if (digit != -1) {
                        num += 10 * digit;
                        break;
                    }
                } 
            }
            for (int i = len - 1; i >= 0; i--) {
		if (isdigit(buf[i])) {
                    num += buf[i] - '0';
                    break;
	        }  else {
                    int digit = matchLongDigit(&buf[i]);
                    if (digit != -1) {
                        num += digit;
                        break;
                    }
                }	
            }
	}
    
        addNode(&num_list, num);
    }

    int sum = 0;
    Node *current = num_list;
    while (current != NULL) {
        sum += current->number;
        current = current->next;
    }

    printf("Sum of all integers in the list: %d\n", sum);

    while (num_list != NULL) {
        Node *temp = num_list;
        num_list = num_list->next;
        free(temp);
    }

    fclose (fp);

    return 0;
}
