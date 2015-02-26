#ifndef HUFF_H
#define HUFF_H

typedef struct leaf leaf;
typedef struct node node;
typedef struct huff huff;
typedef struct huff_list huff_list;
typedef struct bin_table bin_table;

enum huff_tag { LEAF, NODE };

struct leaf {
  char c;
  int n;
};

struct node {
  int n;
  huff *lsub;
  huff *rsub;
};

union huff_union {
  leaf leaf;
  node node;
};

struct huff {
  enum huff_tag tag;
  union huff_union h;
};

struct huff_list {
  huff* h;
  huff_list* next;
};


/* Here follow prototypes for a selection of functions you will likely
   want. You will need to write more, but this is a start.
 */ 

/* Construct a singleton Huffman tree from a character and a count. */
huff *huff_singleton(char c, int n);

/* Return the weight of a Huffman tree. */
int huff_weight(huff *h);

/* Merge two Huffman trees. */
/* See step 2 of the algorithm on the web page for details. */
huff *huff_merge(huff *h1, huff *h2);

/* Display a Huffman tree at the console. */
/* Essential for development and debugging. */
void huff_show(huff *h);

// Checks if character exists in huff list
int huff_check(char c, huff_list* list);

// Finds the ammount of chars in a string
int string_search(char c, char* string);

//Creates a list of huff singletons with chars and their occurences
huff_list* build_list(char* string);

//Sorts the list of huffs
huff_list* sort_huffs(huff_list* h);

//inserts huff in the right place in a list
huff_list* insert_huff(huff_list* h, huff_list* l);

//Sorts the huff into huffman tree
huff_list* coalesce_huff(huff_list* h);

//Finds the binary coding of the char from a huffman tree
char* huff_find_bin(huff* h, char* bin, char c);

//Makes a string uppercase and removes special characters/numbers
void uppercase(char* string);

//returns huff list length
int huff_list_length(huff_list* h);

//Prints binary length
int bin_length(char* string, char** binarray, FILE* file);

// Prints the table of char to binary conversions
char** print_table(huff_list* hl, huff* h, FILE* file);

//Converts a chars representing binary bits into real bits stored
//in a char array.
unsigned char* str_to_bin(char** binarray, char* string, int binlength);

#endif /* HUFF_H */