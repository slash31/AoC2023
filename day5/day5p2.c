#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <limits.h>

#define BUFLEN 4096
#define MAXSEEDS 24 // make this an even number
#define MAXTHREADS MAXSEEDS / 2

pthread_mutex_t mutex;
long globalMinLocation = LONG_MAX;

long globalMapMin = LONG_MAX;
long globalMapMax = 0;

typedef struct MapRange {
    long start;
    long end;
    long offset;
} MapRange;

typedef struct Map {
    MapRange *ranges;
    int rangeCount;
    struct Map *nextMap;
} Map;

typedef struct {
    long start;
    long end;
    Map *map;
} ThreadData;


void getVals(long *dest, long *source, long *range, char *row) {
    *dest = atoi(strtok(row, " "));
    *source = atoi(strtok(NULL, " "));
    *range = atoi(strtok(NULL, " "));
}

int countRanges(FILE *fp) {
    long sectionStart = ftell(fp);
    int count = 0;
    char buf[BUFLEN] = "";
    while (fgets(buf, BUFLEN, fp) != NULL) {
        if (strcmp(buf, "\n") == 0) {
            fseek(fp, sectionStart, SEEK_SET);
            return count;
        }
        count++;
    }
    fseek(fp, sectionStart, SEEK_SET);
    return count;
}

void populateRanges(FILE *fp, Map *map) {
    map->rangeCount = countRanges(fp);
    map->ranges = malloc(map->rangeCount * sizeof(MapRange));
    if (map->ranges == NULL) {
        perror("Failed to allocate memory for MapRanges");
    }
    char buf[BUFLEN] = "";
    int i = 0;
    while (fgets(buf, BUFLEN, fp) != NULL) {
        long dest, source, range;
        if (strcmp(buf, "\n") == 0)  {
            break;
        }
        getVals(&dest, &source, &range, buf);
        map->ranges[i].start = source;
        if (source < globalMapMin)
            globalMapMin = source;
        map->ranges[i].end = source + range - 1;
        if (source + range -1 > globalMapMax)
            globalMapMax = source + range - 1;
        map->ranges[i].offset = dest - source;
        //printf("start: %ld, end: %ld; offset: %ld\n", map->ranges[i].start, map->ranges[i].end, map->ranges[i].offset);
        i++;
    }
}

long translate(Map *map, long source) {
    Map *nextMap = map->nextMap;
    long translated = source;
    for (int i = 0; i < map->rangeCount; i++) {
        if (map->ranges[i].start <= source && map->ranges[i].end >= source) {
            translated = source + map->ranges[i].offset;
            break;
        }
    }
    //printf("translated: %ld\n", translated);
    return translated;
}

long getLocation(long sval, Map *sMap) {
    Map *nextMap = sMap->nextMap;
    while (nextMap != NULL) {
        sval = translate(sMap, sval);
        sMap = nextMap;
        nextMap = sMap->nextMap;
    }
    sval = translate(sMap, sval);
    return sval;
}

void *processSeedPair(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    long localMin = LONG_MAX;

    for (long j = data->start; j < data->start + data->end; j++) {
        long result = j;
        if (j >= globalMapMin && j <= globalMapMax) {
            result = getLocation(j, data->map);
        }
        if (labs(result) < localMin) {
            localMin = labs(result);
        }
    }

    pthread_mutex_lock(&mutex);
    if (localMin < globalMinLocation) {
        globalMinLocation = localMin;
    }
    pthread_mutex_unlock(&mutex);
    //printf("localmin for %ld to %ld is %ld\n", data->start, data->end, localMin);
    free(data);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr,"Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    const char *filename = argv[1];

    char buf[BUFLEN] = "";
    FILE *fp = fopen(filename, "r");

    if (fp == NULL) {
        perror("Error opening file");
        return 1;
    }
    char *sectionName = strtok(buf,":");
    long *seeds = NULL;
    Map *seedToSoilMap = NULL,
        *soilToFertilizerMap = NULL, 
        *fertilizerToWaterMap = NULL,
        *waterToLightMap = NULL,
        *lightToTemperatureMap = NULL,
        *temperatureToHumidityMap = NULL,
        *humidityToLocationMap = NULL;
    // process the file and build the maps
    while (fgets(buf, BUFLEN, fp) != NULL) {
        int rowCount = 0;
        long sectionStart = 0;
        if (strncmp(buf, "seeds:", 6) == 0) {
            char * chunk;
            chunk = strtok(buf, ":");
            chunk = strtok(NULL, ":");
            seeds = (long*)malloc((MAXSEEDS) * sizeof(long));
            if (seeds == NULL) {
                perror("Failed to allocate memory for seeds");
                break;
            }
            int idx = 0;
            while (*chunk) {
                long num;
                int n;
                if (sscanf(chunk, "%ld%n", &num, &n) == 1) {
                    seeds[idx] = num;
                    chunk += n;
                    idx++;
                } else {
                    chunk++;
                }
            }
        } else if (strncmp(buf, "seed-to-soil map:", 17) == 0) {
            // build the seed-to-soil map
            seedToSoilMap = calloc(1, sizeof(Map));
            if (seedToSoilMap == NULL) {
                perror("Failed to allocate memory for Map");
                break;
            }
            populateRanges(fp, seedToSoilMap);
        } else if (strncmp(buf, "soil-to-fertilizer map:", 23) == 0) {
            // build the soil-to-fertilizer map
            soilToFertilizerMap = calloc(1, sizeof(Map));
            seedToSoilMap->nextMap = soilToFertilizerMap;
            if (soilToFertilizerMap == NULL) {
                perror("Failed to allocate memory for Map");
                break;
            }
            populateRanges(fp, soilToFertilizerMap);
        } else if (strncmp(buf, "fertilizer-to-water map:", 24) == 0) {
            // build the fertilizer-to-water map
            fertilizerToWaterMap = calloc(1, sizeof(Map));
            soilToFertilizerMap->nextMap = fertilizerToWaterMap;
            if (fertilizerToWaterMap == NULL) {
                perror("Failed to allocate memory for Map");
                break;
            }
            populateRanges(fp, fertilizerToWaterMap);
        } else if (strncmp(buf, "water-to-light map:", 19) == 0) {
            // build the water-to-light map
            waterToLightMap = calloc(1, sizeof(Map));
            fertilizerToWaterMap->nextMap = waterToLightMap;
            if (waterToLightMap == NULL) {
                perror("Failed to allocate memory for Map");
                break;
            }
            populateRanges(fp, waterToLightMap);
        } else if (strncmp(buf, "light-to-temperature map:", 25) == 0) {
            // build the light-to-temperature map
            lightToTemperatureMap = calloc(1, sizeof(Map));
            waterToLightMap->nextMap = lightToTemperatureMap;
            if (lightToTemperatureMap == NULL) {
                perror("Failed to allocate memory for Map");
                break;
            }
            populateRanges(fp, lightToTemperatureMap);
        } else if (strncmp(buf, "temperature-to-humidity map:", 28) == 0) {
            // build the temperature-to-humidity map
            temperatureToHumidityMap = calloc(1, sizeof(Map));
            lightToTemperatureMap->nextMap = temperatureToHumidityMap;
            if (temperatureToHumidityMap == NULL) {
                perror("Failed to allocate memory for Map");
                break;
            }
            populateRanges(fp, temperatureToHumidityMap);
        } else if (strncmp(buf, "humidity-to-location map:", 25) == 0) {
            // build the humidity-to-location map
            humidityToLocationMap = calloc(1, sizeof(Map));
            temperatureToHumidityMap->nextMap = humidityToLocationMap;
            if (humidityToLocationMap == NULL) {
                perror("Failed to allocate memory for h2l map");
                break;
            }
            populateRanges(fp, humidityToLocationMap);
            humidityToLocationMap->nextMap = NULL;
        }
    }

    pthread_t threads[MAXTHREADS];
    pthread_mutex_init(&mutex, NULL);
    int threadIndex = 0;

    for (int i = 0; seeds[i] != 0 && threadIndex < MAXTHREADS; i += 2) {
        ThreadData *data = malloc(sizeof(ThreadData));
        data->start = seeds[i];
        data->end = seeds[i + 1];
        data->map = seedToSoilMap;

        if (pthread_create(&threads[threadIndex], NULL, processSeedPair, data) != 0) {
            perror("Failed to create thread");
            free(data);
            continue;
        }
        threadIndex++;
    }

    for (int i = 0; i < threadIndex; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("min is %ld\n", globalMinLocation);

    pthread_mutex_destroy(&mutex);
    
    if (seeds != NULL)
        free(seeds);
    if (seedToSoilMap != NULL)
        free(seedToSoilMap);
}