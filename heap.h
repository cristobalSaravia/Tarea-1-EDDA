#ifndef HEAP_H
#define HEAP_H
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
typedef uint32_t heap_key_t;
typedef struct heap heap_t; // heap es el struct que debe definir
heap_t *heap_create(); // constructor, crea un heap vac´ıo
void heap_destroy(heap_t *h); // destructor, libera el espacio de un heap
size_t heap_size(heap_t *h); // devuelve la cantidad de elementos del heap
bool heap_empty(heap_t *h); // retorna true si el heap es vac´ıo, false en otro caso
int heap_get_min(heap_t *h, heap_key_t *out); // m´ınimo del heap
int heap_insert(heap_t *h, heap_key_t x); // inserta elemento x al heap
int heap_delete_min(heap_t *h, heap_key_t *out); // elimina el m´ınimo
size_t heap_size_in_bytes(heap_t *h); // espacio total (bytes) del arreglo extendible
#endif