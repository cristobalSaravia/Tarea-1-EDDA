#include "heap.h"
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <cmath>

struct heap {
    heap_key_t *data;
    heap_key_t **index_array;     
    heap_key_t **shadow_index_array;
    size_t count; 
    size_t index_array_tail;        
    size_t capacity;   
    size_t index_array_capacity;    
    size_t current_super_block_id;
    size_t current_data_block_size;
    size_t remaining_data_blocks_in_super_block;
    size_t current_datablock_count;
    size_t copy_index;
};


static size_t get_superblock_index(size_t index);
static size_t get_index_block_index(size_t index, size_t superblock_index);
static size_t get_datablock_index(size_t index, size_t superblock_index);
static int get_msb_pos(size_t n);

static heap_key_t& heap_at(heap_t *h, size_t index) {


    size_t superblock_index = get_superblock_index(index);
    size_t index_block_index = get_index_block_index(index, superblock_index);
    size_t datablock_index = get_datablock_index(index, superblock_index);

    return h->index_array[index_block_index][datablock_index];


    return h->data[index];
}

static int array_insert(heap_t *h, heap_key_t x, size_t index) {
    h->data[index] = x;


    size_t superblock_index = get_superblock_index(index);
    size_t index_block_index = get_index_block_index(index, superblock_index);
    size_t datablock_index = get_datablock_index(index, superblock_index);

    h->index_array[index_block_index][datablock_index] = x;

    return 0;
}


static size_t get_index_block_index(size_t index, size_t superblock_index) {
    int base_offset;

    if (superblock_index % 2 == 0){
        int m = superblock_index / 2;
        base_offset = std::pow(2, m + 1)-2;
    }
    else {
        int m = (superblock_index - 1) / 2;
        base_offset = 3*std::pow(2, m) - 2; 
    }
       
    int high_count = superblock_index / 2;              
    int low_count = (superblock_index + 1) / 2;

    int x = index + 1;

    uint64_t high_part = (x >> low_count) & ((1ULL << high_count) - 1);

    return base_offset + high_part;
}

static size_t get_datablock_index(size_t index, size_t superblock_index) {
    int low_count = (superblock_index + 1) / 2;
    size_t x = index + 1;
    
    return x & ((1ULL << low_count) - 1);

}


static size_t get_superblock_index(size_t index) {

    return (size_t) get_msb_pos(index + 1);

}

int get_msb_pos(size_t n) {
    int pos = -1;
    while (n > 0) {
        n >>= 1;
        pos++;
    }
    return pos;
}



static int heap_resize(heap_t *h, size_t new_capacity) {
    heap_key_t *new_data = (heap_key_t*)realloc(h->data, new_capacity * sizeof(heap_key_t));
    
    h->data = new_data;
    h->capacity = new_capacity;
    return 0;
}

static int index_array_resize(heap_t *h, size_t new_capacity) {
    heap_key_t **new_index_array = (heap_key_t**)realloc(h->index_array, new_capacity * sizeof(heap_key_t*));
    
    h->index_array = new_index_array;
    h->index_array_capacity = new_capacity;
    return 0;
}

static int index_array_increase_size(heap_t *h) {
    if (h->index_array == nullptr) {
        index_array_resize(h, h->index_array_capacity);
        return 0;
    }
    index_array_resize(h, h->index_array_capacity * 2);
    return 0;
}

static int new_data_block(heap_t *h) {


    if (h->index_array == nullptr){
        index_array_resize(h, h->index_array_capacity);
        h->shadow_index_array = (heap_key_t**)malloc(2*h->index_array_capacity * sizeof(heap_key_t*));
    }

    if (h->index_array_tail == h->index_array_capacity){
        index_array_increase_size(h);
        }

    if (h->remaining_data_blocks_in_super_block == 0){
        h->current_super_block_id++;
        size_t k = h->current_super_block_id;
        h->current_data_block_size = 1ULL << ((k + 1) / 2);
        h->remaining_data_blocks_in_super_block = 1ULL << (k / 2);
    }

    h->index_array[h->index_array_tail] = (heap_key_t*)malloc(h->current_data_block_size * sizeof(heap_key_t));
    
    h->index_array_tail++;
    h->remaining_data_blocks_in_super_block--;
    h->current_datablock_count = 0;
    return 0;
    
}


static int increase_size(heap_t *h) {

    if (h->data == nullptr) {
        heap_resize(h, h->capacity);
        return 0;
    }

    heap_resize(h, h->capacity * 2);
    return 0;
}

heap_t *heap_create() {

    heap_t *h = new heap_t();
    h->data = nullptr;
    h->index_array = nullptr;
    h->shadow_index_array = nullptr;
    h->count = 0;
    h->capacity = 1;
    h->index_array_tail = 0;
    h->index_array_capacity = 1;
    h->current_super_block_id = 0;
    h->current_data_block_size = 1;
    h->remaining_data_blocks_in_super_block = 1;
    h->current_datablock_count = 0;
    h->copy_index = 0;

    increase_size(h);
    new_data_block(h);
    return h;
}

void heap_destroy(heap_t *h) {
    if (h == nullptr) return;

    if (h->index_array != nullptr) {
        for (size_t i = 0; i < h->index_array_tail; i++) {
            if (h->index_array[i] != nullptr) {
                free(h->index_array[i]); 
            }
        }
        free(h->index_array);
    }

    if (h->data != nullptr) {
        free(h->data);
    }

    delete h;
}
size_t heap_size(heap_t *h) {
    return h->count;
}

bool heap_empty(heap_t *h) {
    if (h->count == 0) return true;
    return false;
}

int heap_get_min(heap_t *h, heap_key_t *out) {
    if (heap_empty(h)) return -1;
    
    *out = heap_at(h, 0);
    return 0;
}

int heap_insert(heap_t *h, heap_key_t x) {
    if (h == nullptr) return -1;
    size_t count = h->count;
    if (count == h->capacity) {
        increase_size(h);
    }

    if (h->current_datablock_count == h->current_data_block_size){
        new_data_block(h);
    }


    array_insert(h, x, count);
    h->count++;
    h->current_datablock_count++;

    if (h->count == 1) return 0;

    bool sift_up_complete = false;
    size_t child_index = count;
    while (!sift_up_complete)
    {
        
        if (child_index == 0) break;


        int parent_index = (child_index - 1) / 2;
        heap_key_t child_value = heap_at(h, child_index);
        heap_key_t parent_value = heap_at(h, parent_index);

        if (child_value < parent_value)
        {
            array_insert(h, parent_value, child_index);
            array_insert(h, child_value, parent_index);
            child_index = parent_index;
        }
        else
        {
            sift_up_complete = true;
        }    
    }
    
    return 0;
}


int heap_delete_min(heap_t *h, heap_key_t *out) {
    if (h == nullptr || heap_empty(h)) return -1;
    
    *out = heap_at(h, 0);
    size_t last_index = h->count - 1;
    
    heap_key_t last_value = heap_at(h, last_index);
    array_insert(h, last_value, 0);
    array_insert(h, 0, last_index);

    h->count--;
    h->current_datablock_count--;

    if (h->count == 0) return 0;

    bool sift_down_complete = false;
    size_t parent_index = 0;

    while (!sift_down_complete)
    {
        size_t left_child_index = 2 * parent_index + 1;
        size_t right_child_index = 2 * parent_index + 2;

        if (left_child_index >= h->count) break;

        heap_key_t parent_value = heap_at(h, parent_index);
        heap_key_t left_child_value = heap_at(h, left_child_index);
        
        if (right_child_index >= h->count){
            if (left_child_value < parent_value)
            {
                array_insert(h, left_child_value, parent_index);
                
                array_insert(h, parent_value, left_child_index);
                
                parent_index = left_child_index;
            }
            else
            {
                sift_down_complete = true;
            }
        }
        else
        {
            heap_key_t right_child_value = heap_at(h, right_child_index);
            if (left_child_value < parent_value && left_child_value <= right_child_value){

                array_insert(h, left_child_value, parent_index);
                array_insert(h, parent_value, left_child_index);
                parent_index = left_child_index;
            }
            else if (right_child_value < parent_value && right_child_value < left_child_value){

                array_insert(h, right_child_value, parent_index);
                array_insert(h, parent_value, right_child_index);
                parent_index = right_child_index;
            }
            else
            {
                sift_down_complete = true;
            }
        }
    }
    
    return 0;
}





