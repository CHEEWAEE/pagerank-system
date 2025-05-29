// searchPagerank.c
// Written by William Chhour z5585071
// on 29/1/2025
// 
// This program searches for relevant URLs based on a set of given terms,
// ranking them using PageRank values.
//
// It reads search terms from the command line and looks them up in
// `invertedIndex.txt` to find matching URLs. The retrieved URLs are then 
// sorted based on their PageRank scores, which are read from `pagerankList.txt`. 
//
// The final output is a ranked list of URLs, ordered by relevance 
// (matching terms first) and PageRank score second.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_WORD_LENGTH 1000
#define MAX_URLS 1000

// Structures
typedef struct {
    char url[MAX_WORD_LENGTH];
    int matchCount;
    double pageRank;
} URL;

typedef struct {
    char word[MAX_WORD_LENGTH];
    char *urls[MAX_URLS];
    int urlCount;
} WordEntry;

typedef struct {
    URL urls[MAX_URLS];
    int urlCount;
} PageRankList;

// Function prototypes
void parseInvertedIndex(const char *filename, WordEntry **wordEntries, int *wordCount);
void parsePageRankList(const char *filename, PageRankList *pageRankList);
void findMatchingURLs(WordEntry *wordEntries, int wordCount, PageRankList *pageRankList, char **searchTerms, int termCount, URL *results, int *resultCount);
void rankAndPrintResults(URL *results, int resultCount);
void freeWordEntries(WordEntry *wordEntries, int wordCount);

// Main function
int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <search terms>\n", argv[0]);
        return 1;
    }

    char **searchTerms = argv + 1;
    int termCount = argc - 1;

    // Parse invertedIndex.txt
    WordEntry *wordEntries = NULL;
    int wordCount = 0;
    parseInvertedIndex("invertedIndex.txt", &wordEntries, &wordCount);

    // Parse pagerankList.txt
    PageRankList pageRankList;
    parsePageRankList("pagerankList.txt", &pageRankList);

    // Find matching URLs
    URL results[MAX_URLS];
    int resultCount = 0;
    findMatchingURLs(wordEntries, wordCount, &pageRankList, searchTerms, termCount, results, &resultCount);

    // Rank and print results
    rankAndPrintResults(results, resultCount);

    // Cleanup
    freeWordEntries(wordEntries, wordCount);

    return 0;
}

// Function to parse invertedIndex.txt
void parseInvertedIndex(const char *filename, WordEntry **wordEntries, int *wordCount) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening invertedIndex.txt");
        exit(1);
    }

    *wordEntries = malloc(sizeof(WordEntry) * MAX_URLS);
    *wordCount = 0;
    char line[MAX_WORD_LENGTH * 2];

    while (fgets(line, sizeof(line), file)) {
        char *word = strtok(line, " ");
        WordEntry *entry = &(*wordEntries)[(*wordCount)++];
        strcpy(entry->word, word);
        entry->urlCount = 0;

        char *url = strtok(NULL, " \n");
        while (url) {
            entry->urls[entry->urlCount++] = strdup(url);
            url = strtok(NULL, " \n");
        }
    }

    fclose(file);
}

// Function to parse pagerankList.txt
void parsePageRankList(const char *filename, PageRankList *pageRankList) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening pagerankList.txt");
        exit(1);
    }

    pageRankList->urlCount = 0;
    while (!feof(file)) {
        URL *url = &pageRankList->urls[pageRankList->urlCount];
        if (fscanf(file, "%[^,], %*d, %lf\n", url->url, &url->pageRank) == 2) {
            url->matchCount = 0;
            pageRankList->urlCount++;
        }
    }

    fclose(file);
}

// Function to find matching URLs
void findMatchingURLs(
    WordEntry *wordEntries, int wordCount, PageRankList *pageRankList, 
    char **searchTerms, int termCount, URL *results, int *resultCount) {
    
    *resultCount = 0;

    for (int i = 0; i < termCount; i++) {
        for (int j = 0; j < wordCount; j++) {
            if (strcmp(searchTerms[i], wordEntries[j].word) == 0) {
                for (int k = 0; k < wordEntries[j].urlCount; k++) {
                    for (int l = 0; l < pageRankList->urlCount; l++) {
                        if (strcmp(wordEntries[j].urls[k], 
                                  pageRankList->urls[l].url) == 0) {
                            pageRankList->urls[l].matchCount++;
                        }
                    }
                }
            }
        }
    }

    for (int i = 0; i < pageRankList->urlCount; i++) {
        if (pageRankList->urls[i].matchCount > 0) {
            results[(*resultCount)++] = pageRankList->urls[i];
        }
    }
}

// Function to compare URLs for sorting
int compareURLs(const void *a, const void *b) {
    const URL *urlA = (const URL *)a;
    const URL *urlB = (const URL *)b;

    if (urlA->matchCount != urlB->matchCount) {
        return urlB->matchCount - urlA->matchCount;
    }
    if (urlA->pageRank != urlB->pageRank) {
        return (urlB->pageRank > urlA->pageRank) - (urlB->pageRank < urlA->pageRank);
    }
    return strcmp(urlA->url, urlB->url);
}

// Function to rank and print results
void rankAndPrintResults(URL *results, int resultCount) {
    qsort(results, resultCount, sizeof(URL), compareURLs);

    for (int i = 0; i < resultCount && i < 30; i++) {
        printf("%s\n", results[i].url);
    }
}

// Function to free WordEntry memory
void freeWordEntries(WordEntry *wordEntries, int wordCount) {
    for (int i = 0; i < wordCount; i++) {
        for (int j = 0; j < wordEntries[i].urlCount; j++) {
            free(wordEntries[i].urls[j]);
        }
    }
    free(wordEntries);
}
