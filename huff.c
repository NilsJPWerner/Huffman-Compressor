#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "huff.h"


huff *huff_singleton(char c, int n)
{
	huff* a = malloc(sizeof(huff));
	a->tag = LEAF;
	a->h.leaf.c = c;
	a->h.leaf.n = n;
	return a;
}

int huff_weight(huff *h)
{
	if (h->tag == LEAF)
	{
		return h->h.leaf.n;
	}
	else 
	{
		return h->h.node.n;
	}
}

huff *huff_merge(huff *h1, huff *h2)
{
	huff* a = malloc(sizeof(huff));
	a->tag = NODE;
	a->h.node.lsub = h1;
	a->h.node.rsub = h2;
	a->h.node.n = huff_weight(h1) + huff_weight(h2);
	return a;
}

void huff_show(huff *h)
{
	if (h == NULL)
	{
		return;
	}
	if (h->tag == LEAF)
	{
		printf("%c:%d", h->h.leaf.c, h->h.leaf.n);
	}
	else
	{
		printf("%d(", h->h.node.n);
		huff_show(h->h.node.lsub);
		printf(",");
		huff_show(h->h.node.rsub);
		printf(")");
	}
}

int huff_check(char c, huff_list* list) 
{
	if (list == NULL) return 0; //it doesn't
	if (c == list->h->h.leaf.c)
	{
	return 1; // It exists in the list
	}
	else return huff_check(c, list->next);
}

int string_search(char c, char* string)
{
	int n = 0;
	int i;
	for (i=0; string[i]!=0; i++)
	{
		if (string[i] == c)
		{
			n++;
		}
	}
	return n;
}

huff_list* build_list(char* string)
{
	int i;
	huff_list* list = NULL;
	for (i = 0; string[i]!=0; ++i)
	{
		if (huff_check(string[i], list));
		else
		{
			huff_list* newl = malloc(sizeof(huff_list));
			newl->h = huff_singleton(string[i], 
				string_search(string[i], string));
			newl->next = list;
			list = newl;
		}
	}
	return list;
}

huff_list* sort_huffs(huff_list* h)
{
	huff_list* new = NULL;
	while (h != NULL)
	{
		huff_list* next = h->next;
		new = insert_huff(h, new);
		h = next;
	}
	return new;
}

huff_list* insert_huff(huff_list* h, huff_list* l)
{
	if (l == NULL)
	{
		h->next = NULL;
		return h;
	}
	if (huff_weight(h->h) <= huff_weight(l->h))
	{
		h->next = l;
		return h;
	}
	else
	{
		l->next = insert_huff(h, l->next);
		return l;
	}
}

huff_list* coalesce_huff(huff_list* h)
{
	if (h->next == NULL)
	{
		return h;
	}
	huff_list* merged = malloc(sizeof(huff_list));
	merged->h = huff_merge(h->h, h->next->h);
	huff_list* new = insert_huff(merged, h->next->next);
	free(h->next);
	free(h);
	return coalesce_huff(new);
}

char* huff_find_bin(huff* h, char* bin, char c)
{
	if (h->tag == NODE)
	{
		char* l_bin = malloc(sizeof(char)*(strlen(bin)+1));
		char* r_bin = malloc(sizeof(char)*(strlen(bin)+1));
		strcpy(l_bin, bin);
		strcpy(r_bin, bin);
		strcat(l_bin, "0");
		strcat(r_bin, "1");
		if (strlen(bin) > 0) free(bin);
		char* left = huff_find_bin(h->h.node.lsub, l_bin, c);
		char* right = huff_find_bin(h->h.node.rsub, r_bin, c);
		return (left == NULL)? right : left;
	}
	else
	{
		if (h->h.leaf.c == c)
		{
			return bin;
		}
		else 
		{
			free(bin);
			return NULL;
		}
	}
}

char** print_table(huff_list* hl, huff* h, FILE* file)
{
	fprintf(file, "%d\n", huff_list_length(hl));
	char** binarray = malloc(sizeof(char*)*27);
	char c;
	while (hl != NULL)
	{
		c = hl->h->h.leaf.c;
		char* bin = huff_find_bin(h, "", c);
		binarray[(c == 32)? 0 : c-64] = bin;
		fprintf(file, "%c=%s\n", c, bin);
		hl = hl->next;
	}
	return binarray;
}

void uppercase(char* string)
{
	int i;
	int pos = 0;
	for (i=0; string[i]!=0; i++)
	{
		if ((string[i] > 64 && string[i] < 91) ||
			(string[i] >= 96 && string[i] <= 123) || string[i] == 32)
		{
			string[pos] = toupper(string[i]);
			pos++;
		}
	}
	string[pos] = 0;
}

int huff_list_length(huff_list* h)
{
	int i = 0;
	while (h != NULL)
	{
		i++;
		h = h->next;
	}
	return i;
}

int bin_length(char* string, char** binarray, FILE* file)
{
	int i;
	int count = 0;
	for (i=0; string[i]!=0; i++)
	{
		count += strlen(binarray[(string[i] == 32)? 0 : string[i]-64]);
	}
	return count;
}

unsigned char* str_to_bin(char** binarray, char* string, int binlength)
{
	int i, j;
	int index = 0;
	int pos = 1;
	unsigned char* bincode = malloc(sizeof(char)*ceil(binlength/8.0));
	for (i = 0; i<ceil(binlength/8.0); i++) bincode[i]=0;
	for (i = 0; string[i]!=0; i++)
	{
		char* binary = binarray[(string[i] == 32)? 0 : string[i]-64];
		for (j=0; binary[j]!=0; j++)
		{
			if (pos > 8)
			{
				index++;
				pos -= 8;
			}
			if (binary[j]==49)
			{
				bincode[index] = bincode[index] | (1<<(8-pos));
			}
			pos++;
		}
	}
	return bincode;
}

int main(int argc, char* argv[])
{
	
	uppercase(argv[2]);
	
	huff_list* list = build_list(argv[2]);
	list = sort_huffs(list);
	list = coalesce_huff(list);

	FILE* file = fopen(argv[1], "w");
	huff_list* clist = build_list(argv[2]);
	char** binarray = print_table(clist, list->h, file);
	int len = bin_length(argv[2], binarray, file);
	fprintf(file, "%d\n", len);
	unsigned char* bitcode = str_to_bin(binarray, argv[2], len);
	int i;
	for (i=0; i<ceil(len/8.0); i++)
	{
		fputc(bitcode[i], file);
	}
}







