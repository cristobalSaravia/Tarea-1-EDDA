#include <heap.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>

struct heap {
    std::vector<heap_key_t> array;     
    size_t count;         
    size_t capacity;       
};

heap_t *heap_create() {

    heap_t *h = new heap_t();
    h->array.resize(1);
    h->count = 0;
    h->capacity = 1;
    return h;
}

void heap_destroy(heap_t *h) {    
    delete h;
}

size_t heap_size(heap_t *h) {
    if (h == nullptr) return 0;
    return h->count;
}

bool heap_empty(heap_t *h) {
    if (h == nullptr) return true;
    if (h->count == 0) return true;
    return false;
}

int heap_get_min(heap_t *h, heap_key_t *out) {
    if (h == nullptr || heap_empty(h)) return -1;
    
    *out = h->array.at(0);
    return 0;
}

int heap_insert(heap_t *h, heap_key_t x) {
    if (h == nullptr) return -1;
    int count = h->count;
    if (count == h->capacity) {
        h->capacity *= 2;
        h->array.resize(h->capacity);
    }
    h->array.at(count) = x;
    h->count++;

    if (h->count == 1) return 0;

    bool sift_up_complete = false;
    int child_index = count;
    while (!sift_up_complete)
    {
        
        if (child_index == 0) break;


        int parent_index = (child_index - 1) / 2;
        heap_key_t child_value = h->array.at(child_index);
        heap_key_t parent_value = h->array.at(parent_index);

        if (h->array.at(child_index) < h->array.at(parent_index))
        {
            h->array.at(child_index) = parent_value;
            h->array.at(parent_index) = child_value;
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
    
    *out = h->array.at(0);
    int last_index = h->count - 1;
    
    h->array.at(0) = h->array.at(last_index);


    h->count--;

    if (h->count == 0) return 0;

    bool sift_down_complete = false;
    int parent_index = 0;

    while (!sift_down_complete)
    {
        int left_child_index = 2 * parent_index + 1;
        int right_child_index = 2 * parent_index + 2;

        if (left_child_index >= h->count) break;

        heap_key_t parent_value = h->array.at(parent_index);
        heap_key_t left_child_value = h->array.at(left_child_index);
        
        if (right_child_index >= h->count){
            if (left_child_value < parent_value)
            {
                h->array.at(parent_index) = left_child_value;
                h->array.at(left_child_index) = parent_value;  
                parent_index = left_child_index;
            }
            else
            {
                sift_down_complete = true;
            }
        }
        else
        {
            heap_key_t right_child_value = h->array.at(right_child_index);
            if (left_child_value < parent_value && left_child_value <= right_child_value){
                h->array.at(parent_index) = left_child_value;
                h->array.at(left_child_index) = parent_value;  
                parent_index = left_child_index;
            }
            else if (right_child_value < parent_value && right_child_value < left_child_value){
                h->array.at(parent_index) = right_child_value;
                h->array.at(right_child_index) = parent_value;  
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





