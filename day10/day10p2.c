#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


#define BUFLEN 4096
#define GRIDSZ 140

typedef struct Tile {
    int col;
    int row;
    int distance;
    struct Tile *head;
    struct Tile *tail;
    char pipe; 
} Tile;

int validNeighbor (char sourcePipe, char destPipe, char dir) {
    switch(dir) {
        case 'N':
            if ((sourcePipe == '|' || sourcePipe == 'J' || sourcePipe == 'L' || sourcePipe == 'S') && (destPipe == '|' || destPipe == 'F' || destPipe == '7'))
                return 1;
            break;
        case 'E':
            if ((sourcePipe == '-' || sourcePipe == 'F' || sourcePipe == 'L' || sourcePipe == 'S') && (destPipe == '-' || destPipe == '7' || destPipe == 'J'))
                return 1;
            break;
        case 'S':
            if ((sourcePipe == '|' || sourcePipe == 'F' || sourcePipe == '7' || sourcePipe == 'S') && (destPipe == '|' || destPipe == 'J' || destPipe == 'L'))
                return 1;
            break;
        case 'W':
            if ((sourcePipe == '-' || sourcePipe == '7' || sourcePipe == 'J' || sourcePipe == 'S') && (destPipe == '-' || destPipe == 'F' || destPipe == 'L'))
                return 1;
            break;
    }
    return 0;
}

int oob(int pos) {
    if (pos < 0 || pos >= GRIDSZ) {
        return 1;
    }
    return 0;
}

void printViz(char viz[GRIDSZ][GRIDSZ]) {
    for (int i=0; i< GRIDSZ; i++) {
        for (int j=0; j<GRIDSZ; j++) {
            printf("%c", viz[i][j]);
        }
        printf("\n");
    }
    printf("\n\n");
}

void getNeighbors (Tile *tile, char field[GRIDSZ][GRIDSZ], char viz[GRIDSZ][GRIDSZ]) {
    //printf("getting neighbors for %c\n", tile->pipe);
    int rpos[4] = {-1,0,1,0}, cpos[4] = {0,1,0,-1};
    char dir[4] = {'N', 'E', 'S', 'W'};
    for (int i = 0; i < 4; i++) {
        if (tile->head != NULL && tile->tail != NULL) {
            continue;
        }
        if (oob(tile->row + rpos[i]) || oob(tile->col + cpos[i])) {
            continue;
        }
        if ( (tile->head != NULL && tile->head->row == tile->row + rpos[i] && tile->head->col == tile->col + cpos[i]) || (tile->tail != NULL && tile->tail->row == tile->row + rpos[i] && tile->tail->col == tile->col + cpos[i]) ) {
            continue;
        }
        char nPipe = field[tile->row + rpos[i]][tile->col + cpos[i]];
        if (validNeighbor(tile->pipe, nPipe, dir[i])) {
            //printf("valid pipe %c in dir %c\n", nPipe, dir[i]);
            Tile *neighbor = calloc(1, sizeof(Tile));
            neighbor->pipe = nPipe;
            neighbor->row = tile->row + rpos[i];
            neighbor->col = tile->col + cpos[i];
            neighbor->distance = tile->distance + 1;
            if (tile->head == NULL) {
                tile->head = neighbor;
                neighbor->tail = tile;
                viz[tile->row + rpos[i]][tile->col + cpos[i]] = nPipe;
            } else if (tile->tail == NULL) {
                tile->tail = neighbor;
                neighbor->head = tile;
                viz[tile->row + rpos[i]][tile->col + cpos[i]] = nPipe;
            } else {
                free(neighbor);
            }
            //printViz(viz);
        } 
    }
}

int explore(Tile *head, Tile *tail, char field[GRIDSZ][GRIDSZ],char viz[GRIDSZ][GRIDSZ]) {
    if (head->row == tail->row && head->col == tail->col) {
        return head->distance;
    } else {
         getNeighbors(head, field, viz);
         getNeighbors(tail, field, viz);
         //printf("got neighbors %p, %p, go explore\n", head->head, tail->tail);
         return explore(head->head, tail->tail, field, viz);
    }
}

int getNestSize(char viz[GRIDSZ][GRIDSZ]) {
    int nestSize = 0;
    int bLeft[GRIDSZ], bRight[GRIDSZ], bTop[GRIDSZ], bBottom[GRIDSZ];
    for (int i = 0; i < GRIDSZ; i++) {
        bLeft[i] = 0;
        bTop[i] = 0;
        bRight[i] = GRIDSZ - 1;
        bBottom[i] = GRIDSZ - 1;
    }
    
    for (int row = 0; row < GRIDSZ; row++) { 
        for (int col = 0; col < GRIDSZ; col++) {
            if (bLeft[row] != 0 && bRight[row] != GRIDSZ-1) {
                continue;
            }
            if (viz[row][col] != '.' && bLeft[row] == 0) {
                bLeft[row] = col;
            }
            if (viz[row][(GRIDSZ-1)-col] != '.' && bRight[row] == GRIDSZ-1) {
                bRight[row] = (GRIDSZ-1)-col;
            } 
        }
    }
    for (int col = 0; col < GRIDSZ; col++) { 
        for (int row = 0; row < GRIDSZ; row++) {
            if (bTop[col] != 0 && bBottom[col] != GRIDSZ-1) {
                continue;
            }
            if (viz[row][col] != '.' && bTop[col] == 0) {
                bTop[col] = row;
            }
            if (viz[(GRIDSZ-1)-row][col] != '.' && bBottom[col] ==  GRIDSZ-1) {
                bBottom[col] = (GRIDSZ-1)-row;
            } 
        }
    }
    for (int row = 0; row < GRIDSZ; row++) { 
        for (int col = 0; col < GRIDSZ; col++) {
            if (row > bTop[col] && row < bBottom[col] && col > bLeft[row] && col < bRight[row]) {
                if (viz[row][col] == '.') {
                    int walls = 0;
                    int rayRow = row, rayCol = col;
                    while (rayRow < GRIDSZ && rayCol < GRIDSZ) {
                        if (viz[rayRow][rayCol] != '.' && viz[rayRow][rayCol] != 'L' &&  viz[rayRow][rayCol] != '7') {
                            walls++;
                        }
                        rayRow++;
                        rayCol++;
                    }
                    if (walls % 2 == 1) {
                        //printf("counting %d, %d\n", row, col);
                        nestSize++;
                    }
                } 
            }
        }
    }
    return nestSize;
}

char getStartPipe(Tile *start) {
    char hPipe = start->head->pipe;
    char tPipe = start->tail->pipe;
    char dirs[2];

    if (start->head->col == start->col) {
       return '|';
    } else if (start->head->row == start->row) {
        return '_';
    } else if ( (start->head->row < start->row && start->tail->col > start->col) || (start->tail->row < start->row && start->head->col > start->col) ) {
        return 'L';
    } else if ( (start->head->row > start->row && start->tail->col < start->col) || (start->tail->row > start->row && start->head->col < start->col) ) {
        return '7';
    } else if ( (start->head->row < start->row && start->tail->col < start->col) || (start->tail->row < start->row && start->head->col < start->col) ) {
        return 'J';
    } else {
        return 'F';
    }
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
    char field[GRIDSZ][GRIDSZ];
    char viz[GRIDSZ][GRIDSZ];
    bool enclosed;

    //printf("setting up viz\n");
    for (int i=0; i< GRIDSZ; i++) {
        for (int j=0; j<GRIDSZ; j++) {
            viz[i][j] = '.';
        }
        //printf("\n");
    }
    int row = 0;
    Tile *start = calloc(1, sizeof(Tile));
    while (fgets(buf, sizeof(buf) , fp)) {
        for (int col = 0; col < GRIDSZ; col++) {
            char tile = buf[col];
            field[row][col] = tile;
            if (tile == 'S') {
                start->col = col;
                start->row = row;
                start->distance = 0;
                start->pipe = 'S';
            }
        }
        row++;
    }
    
    getNeighbors(start, field, viz);
    viz[start->row][start->col] = getStartPipe(start);
    printf("start pipe is %c\n", viz[start->row][start->col]);
    int distance = explore(start->head, start->tail, field, viz);    
    //printViz(viz);
    printf("nest size is %d\n", getNestSize(viz));
    
    return 0;
}