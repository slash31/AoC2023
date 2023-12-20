#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#define BUFLEN 4096
#define NUMCARDS 5

enum DESC {
    NOTHING,
    ONE_PAIR,
    TWO_PAIR,
    THREE_OF_A_KIND,
    FULL_HOUSE,
    FOUR_OF_A_KIND,
    FIVE_OF_A_KIND
};

typedef struct Hand {
    char *cards;
    int bid;
    int label;
} Hand;

int getLabel (char *cards) {
    char counts[CHAR_MAX + 1] = {0};
    for (int i = 0; i < NUMCARDS; i++) {
        counts[(unsigned char)cards[i]]++;
        if (counts[(unsigned char)cards[i]] == FIVE_OF_A_KIND) {
            return 6;
        }
    }
    int pairs = 0;
    int triples = 0;
    for (int i = 0; i <= CHAR_MAX; i++) {
        if (counts[i] == 4) {
            return 5;
        } else if (counts[i] == 2) {
            pairs++;
            if (pairs == 2) {
                return TWO_PAIR;
            }
        } else if (counts[i] == 3) {
            triples++;
            if (pairs == 1 && triples == 1) {
                return FULL_HOUSE;
            }
        }
    }
    if (pairs == 1) {
        return ONE_PAIR;
    } else if (triples == 1) {
        return THREE_OF_A_KIND;
    }

    return NOTHING;
}

int getCardValue(char c) {
    switch(c) {
        case 'A': return 14;
        case 'K': return 13;
        case 'Q': return 12;
        case 'J': return 11;
        case 'T': return 10;
        default:  return c - '0';
    }
}

int compareHands(const void *a, const void *b) {
    const Hand *hand1 = (const Hand *)a;
    const Hand *hand2 = (const Hand *)b;

    char *cards1 = malloc(5*sizeof(char));
    char *cards2 = malloc(5*sizeof(char));
    if (hand1->label > hand2->label) {
        return 1;
    } else if (hand2->label > hand1->label) {
        return -1;
    }
    for (int i = 0; i < NUMCARDS; i++) {
        int delta = getCardValue(hand1->cards[i]) - getCardValue(hand2->cards[i]);
        //printf("comparing %c and %c...\n", hand1->cards[i], hand2->cards[i]);
        if (delta != 0) {
            //printf("returned %d\n", delta);
            return delta;
        }
    }
    return 0;
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

    int numLines = 0;
    while (fgets(buf, BUFLEN, fp) != NULL) {
        numLines++;
    }

    rewind(fp);

    Hand *hands;
    hands = malloc((numLines) * sizeof(Hand));
    int i = 0;
    while (fgets(buf, BUFLEN, fp) != NULL) {
        char *cards;
        int bid;
        cards = malloc((5 * sizeof(char)));
        if (sscanf(buf, "%5s %d", cards, &bid) == 2) {
            hands[i].cards = cards;
            hands[i].bid = bid;
            hands[i].label = getLabel(hands[i].cards);
            i++;
        }
    }
    qsort(hands, numLines, sizeof(Hand), compareHands);

    long total_winnings = 0;
    for (int i = 0; i < numLines; i++ ) {
        total_winnings += hands[i].bid * (i+1);
        printf("hand %s\n", hands[i].cards);
        printf("  bid %d, label %d, rank %d, prize %d, running total %ld\n", hands[i].bid, hands[i].label, i+1, hands[i].bid * (i+1), total_winnings);
    }
    printf("total winnings: %ld\n", total_winnings);

}