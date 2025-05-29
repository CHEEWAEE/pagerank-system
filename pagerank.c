// pagerank.c
// Written by William Chhour z5585071
// on 29/1/2025
// 
// This program calculates the PageRank of URLs based on their link structure
// using the iterative PageRank algorithm.
//
// It reads a list of URLs from `collection.txt`, builds a directed graph 
// where nodes represent URLs and edges represent outgoing links, and then
// applies the PageRank formula iteratively until convergence.
//
// The computed PageRank values are written to `pagerankList.txt`, sorted in 
// descending order of PageRank. The output format is:
// 
//    <URL>, <out-degree>, <PageRank>
//
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#define MAX_URL_LENGTH 100
#define MAX_URLS 1000

typedef struct {
    char url[MAX_URL_LENGTH];
    int outDegree;
    double pageRank;
    int links[MAX_URLS];
} Page;

// Function Prototypes
int readCollection(Page pages[], int maxPages);
void calculatePageRank(Page pages[], int N, double d, double diffPR, int maxIterations);
void writePageRankToFile(Page pages[], int N);
int findPageIndex(Page pages[], int N, const char *url);
int comparePageRank(const void *a, const void *b);

// Helper Functions for PageRank Calculation
void initializePreviousRanks(Page pages[], double prevPR[], int N);
void calculateNewRanks(Page pages[], double prevPR[], int N, double d);
double computePageRankDiff(Page pages[], double prevPR[], int N);

// Function Definitions
int findPageIndex(Page pages[], int N, const char *url) {
    for (int i = 0; i < N; i++) {
        if (strcmp(pages[i].url, url) == 0) {
            return i;
        }
    }
    return -1;
}

int comparePageRank(const void *a, const void *b) {
    double diff = ((Page *)b)->pageRank - ((Page *)a)->pageRank;
    return (diff > 0) - (diff < 0);
}

void processSection1(FILE *urlFile, Page *pages, int N, int i) {
    char line[1000];
    int inSection1 = 0;

    while (fgets(line, sizeof(line), urlFile)) {
        if (strcmp(line, "#start Section-1\n") == 0) {
            inSection1 = 1;
        } else if (strcmp(line, "#end Section-1\n") == 0) {
            inSection1 = 0;
        } else if (inSection1) {
            char *token = strtok(line, " \n");
            while (token != NULL) {
                int linkedIndex = findPageIndex(pages, N, token);
                if (linkedIndex != -1 && linkedIndex != i) {
                    pages[i].links[linkedIndex] = 1;
                    pages[i].outDegree++;
                }
                token = strtok(NULL, " \n");
            }
        }
    }
}

int readCollection(Page pages[], int maxPages) {
    int N = 0;
    FILE *file = fopen("collection.txt", "r");
    if (!file) {
        perror("Error opening collection.txt");
        exit(1);
    }

    while (1) {
        if (N >= maxPages) {
            break;
        }
        if (fscanf(file, "%s", pages[N].url) == EOF) {
            break;
        }
        N++;
    }
    fclose(file);


    // Initialize each page's outDegree, PageRank, and adjacency matrix
    for (int i = 0; i < N; i++) {
        pages[i].outDegree = 0;
        pages[i].pageRank = 1.0 / N;
        memset(pages[i].links, 0, sizeof(pages[i].links));
    }

    // Parse each URL's corresponding .txt file
    for (int i = 0; i < N; i++) {
        char filename[MAX_URL_LENGTH + 5];
        snprintf(filename, sizeof(filename), "%s.txt", pages[i].url);

        FILE *urlFile = fopen(filename, "r");
        if (!urlFile) {
            perror("Error opening URL file");
            exit(1);
        }

        processSection1(urlFile, pages, N, i);

        fclose(urlFile);
    }
    return N;
}

void initializePreviousRanks(Page pages[], double prevPR[], int N) {
    for (int i = 0; i < N; i++) {
        prevPR[i] = pages[i].pageRank;
    }
}

void calculateNewRanks(Page pages[], double prevPR[], int N, double d) {
    for (int i = 0; i < N; i++) {
        double sum = 0.0;
        for (int j = 0; j < N; j++) {
            if (pages[j].links[i]) {
                if (pages[j].outDegree != 0) {
                    sum += prevPR[j] / pages[j].outDegree;
                } else {
                    sum += prevPR[j] / N;
                }
            }
        }
        pages[i].pageRank = ((1 - d) / N) + (d * sum);
    }
}

double computePageRankDiff(Page pages[], double prevPR[], int N) {
    double diff = 0.0;
    for (int i = 0; i < N; i++) {
        diff += fabs(pages[i].pageRank - prevPR[i]);
    }
    return diff;
}

void calculatePageRank(Page pages[], int N, double d, double diffPR, int maxIterations) {
    double prevPR[MAX_URLS];
    int iteration = 0;
    double diff;

    do {
        initializePreviousRanks(pages, prevPR, N);
        calculateNewRanks(pages, prevPR, N, d);
        diff = computePageRankDiff(pages, prevPR, N);
        iteration++;
    } while (iteration < maxIterations && diff >= diffPR);
}

void writePageRankToFile(Page pages[], int N) {
    FILE *file = fopen("pagerankList.txt", "w");
    if (!file) {
        perror("Error opening pagerankList.txt");
        exit(1);
    }

    qsort(pages, N, sizeof(Page), comparePageRank);

    for (int i = 0; i < N; i++) {
        fprintf(file, "%s, %d, %.7f\n", pages[i].url, pages[i].outDegree, pages[i].pageRank);
    }
    fclose(file);
}

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s d diffPR maxIterations\n", argv[0]);
        return 1;
    }

    double d = atof(argv[1]);
    double diffPR = atof(argv[2]);
    int maxIterations = atoi(argv[3]);

    Page pages[MAX_URLS];
    int N = readCollection(pages, MAX_URLS);
    calculatePageRank(pages, N, d, diffPR, maxIterations);
    writePageRankToFile(pages, N);

    return 0;
}
