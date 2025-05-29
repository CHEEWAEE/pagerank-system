// invertedIndex.c
// Written by William Chhour z5585071
// on 29/1/2025
// 
// This program constructs an **inverted index** from a collection of text files.
// It processes each file, extracts words, and builds a searchable data structure
// that maps words to the URLs where they appear. The index is then used for
// efficient term-based searching.
//
// The program reads input from `collection.txt`, normalizes words (removing 
// punctuation and converting to lowercase), and stores them in a binary search tree. 
// The output is written to `invertedIndex.txt`
// in **alphabetical order**, showing each word followed by the list of URLs 
// where it appears.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORD_LENGTH 1000
#define MAX_FILENAME_LENGTH 100

// Linked list for filenames
typedef struct FileNode {
    char filename[MAX_FILENAME_LENGTH];
    struct FileNode *next;
} FileNode;

// Node for the binary search tree
typedef struct TreeNode {
    char word[MAX_WORD_LENGTH];
    FileNode *fileList;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

// Function prototypes
TreeNode *insertWord(TreeNode *root, const char *word, const char *filename);
TreeNode *createTreeNode(const char *word, const char *filename);
FileNode *createFileNode(const char *filename);
void addFilename(FileNode **head, const char *filename);
void normalizeWord(char *word);
void printInvertedIndex(TreeNode *root, FILE *outputFile);
void freeTree(TreeNode *root);
void parseFile(const char *filename, TreeNode **root);

int main() {
    // Open collection.txt
    FILE *collectionFile = fopen("collection.txt", "r");
    if (!collectionFile) {
        perror("Error opening collection.txt");
        return 1;
    }

    TreeNode *root = NULL;
    char filename[MAX_FILENAME_LENGTH];

    // Read filenames from collection.txt
    while (fscanf(collectionFile, "%s", filename) != EOF) {
        printf("Processing file: %s\n", filename);
        parseFile(filename, &root);
    }

    fclose(collectionFile);

    // Write inverted index to a file
    FILE *outputFile = fopen("invertedIndex.txt", "w");
    if (!outputFile) {
        perror("Error opening invertedIndex.txt");
        freeTree(root);
        return 1;
    }

    printInvertedIndex(root, outputFile);
    fclose(outputFile);

    freeTree(root);
    return 0;
}

// Function to parse a file and add its words to the inverted index
void parseFile(const char *filename, TreeNode **root) {
    char fullFilename[MAX_FILENAME_LENGTH + 5];
    snprintf(fullFilename, sizeof(fullFilename), "%s.txt", filename);

    FILE *file = fopen(fullFilename, "r");
    if (!file) {
        fprintf(stderr, "Error opening input file: %s\n", fullFilename);
        return;
    }

    char word[MAX_WORD_LENGTH];
    while (fscanf(file, "%s", word) != EOF) {
        // Skip metadata like "#start", "#end", "section-1", "section-2"
        if (strncmp(word, "#start", 6) == 0 || strncmp(word, "#end", 4) == 0 ||
            strcmp(word, "Section-1") == 0 || strcmp(word, "Section-2") == 0) {
            continue;
        }

        // Skip words that look like filenames (e.g., "url11", "url21", etc.)
        if (strncmp(word, "url", 3) == 0 && isdigit(word[3])) {
            continue;
        }

        // Normalize the word and add to the inverted index if valid
        normalizeWord(word);
        if (strlen(word) > 0) {
            *root = insertWord(*root, word, filename);
        }
    }

    fclose(file);
}


// Function to normalize a word
void normalizeWord(char *word) {
    size_t len = strlen(word);

    // Convert to lowercase
    for (size_t i = 0; i < len; i++) {
        word[i] = tolower(word[i]);
    }

    // Remove trailing punctuation
    while (len > 0 && (word[len - 1] == '.' || word[len - 1] == ',' ||
                        word[len - 1] == ':' || word[len - 1] == ';' ||
                        word[len - 1] == '?' || word[len - 1] == '*')) {
        word[len - 1] = '\0';
        len--;
    }

    // Filter out invalid words that are empty or metadata
    if (len == 0 || !isalpha(word[0])) {
        word[0] = '\0'; // Set invalid word to empty
    }
}


// Function to insert a word into the binary search tree
TreeNode *insertWord(TreeNode *root, const char *word, const char *filename) {
    if (root == NULL) {
        return createTreeNode(word, filename);
    }

    int cmp = strcmp(word, root->word);
    if (cmp < 0) {
        root->left = insertWord(root->left, word, filename);
    } else if (cmp > 0) {
        root->right = insertWord(root->right, word, filename);
    } else {
        addFilename(&(root->fileList), filename);
    }

    return root;
}

// Function to create a new tree node
TreeNode *createTreeNode(const char *word, const char *filename) {
    TreeNode *newNode = malloc(sizeof(TreeNode));
    if (!newNode) {
        perror("Error allocating memory for tree node");
        exit(1);
    }

    strcpy(newNode->word, word);
    newNode->fileList = createFileNode(filename);
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

// Function to create a new file node
FileNode *createFileNode(const char *filename) {
    FileNode *newNode = malloc(sizeof(FileNode));
    if (!newNode) {
        perror("Error allocating memory for file node");
        exit(1);
    }

    strcpy(newNode->filename, filename);
    newNode->next = NULL;
    return newNode;
}

// Function to add a filename to the file list
// Function to add a filename to the file list in sorted order
void addFilename(FileNode **head, const char *filename) {
    FileNode *current = *head;
    FileNode *prev = NULL;

    // Create a new file node
    FileNode *newNode = createFileNode(filename);

    // Find the correct position for insertion (sorted order)
    while (current != NULL && strcmp(current->filename, filename) < 0) {
        prev = current;
        current = current->next;
    }

    // Check if the filename already exists
    if (current != NULL && strcmp(current->filename, filename) == 0) {
        free(newNode); // Filename already exists, no need to add it again
        return;
    }

    // Insert the new file node at the correct position
    if (prev == NULL) {
        // Insert at the head
        newNode->next = *head;
        *head = newNode;
    } else {
        // Insert in the middle or at the end
        newNode->next = current;
        prev->next = newNode;
    }
}


// Function to print the inverted index to a file
void printInvertedIndex(TreeNode *root, FILE *outputFile) {
    if (root == NULL) {
        return;
    }

    printInvertedIndex(root->left, outputFile);

    fprintf(outputFile, "%s", root->word);
    FileNode *current = root->fileList;
    while (current != NULL) {
        fprintf(outputFile, " %s", current->filename);
        current = current->next;
    }
    fprintf(outputFile, "\n");

    printInvertedIndex(root->right, outputFile);
}

// Function to free the tree
void freeTree(TreeNode *root) {
    if (root == NULL) {
        return;
    }

    freeTree(root->left);
    freeTree(root->right);

    FileNode *current = root->fileList;
    while (current != NULL) {
        FileNode *temp = current;
        current = current->next;
        free(temp);
    }

    free(root);
}
