#include <stdbool.h>
#include <stdlib.h>

typedef struct {
    char *key;
    char *value;
    Item *next;
} Item;

typedef Item *List;


