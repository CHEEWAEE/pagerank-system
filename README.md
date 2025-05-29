# C Search Engine with PageRank

A basic C-based search engine developed for UNSW COMP2521.  
It ranks URLs based on a combination of keyword relevance and the PageRank algorithm.

---

## 🧠 Overview

The project has 3 main components:

1. **Inverted Index Generator (`invertedIndex.c`)**  
   Builds a searchable inverted index from a collection of URL text files.  
   Output: `invertedIndex.txt`

2. **PageRank Calculator (`pagerank.c`)**  
   Computes PageRank values for each URL using an iterative algorithm.  
   Output: `pagerankList.txt`  
   Format: `<url>, <out-degree>, <PageRank>`

3. **Search Engine (`searchPagerank.c`)**  
   Accepts search terms via command line, ranks URLs by:
   - **Match count** across terms
   - **PageRank score**  
   Output: top matching URLs (max 30)

---

## 📁 File Summary

- `invertedIndex.c`: Reads `collection.txt`, normalizes and indexes words → `invertedIndex.txt`
- `pagerank.c`: Reads `collection.txt`, parses `.txt` files, computes PageRank → `pagerankList.txt`
- `searchPagerank.c`: Combines inverted index and PageRank data to return relevant results

---

## ⚙️ How to Run

### 1. Create your data files

- `collection.txt`: list of URL filenames (without `.txt`)
- Corresponding files: `url1.txt`, `url2.txt`, etc.
  - Must include `#start Section-1` and `#end Section-1` for links

### 2. Build and run

```bash
# Generate the inverted index
gcc -o invertedIndex invertedIndex.c
./invertedIndex

# Calculate PageRank
gcc -o pagerank pagerank.c -lm
./pagerank 0.85 0.0001 1000

# Run search engine with terms
gcc -o search searchPagerank.c
./search term1 term2
