#include <iostream>
#include <vector>
#include <cassert>
#include "heap.h"

int main() {
    heap_t* h = heap_create();
    heap_key_t val;

    std::cout << "--- Test 1: Empty Heap Behavior ---" << std::endl;
    assert(heap_empty(h) == true);
    assert(heap_size(h) == 0);
    assert(heap_get_min(h, &val) != 0); // Should return non-zero for empty [cite: 69]

    std::cout << "--- Test 2: Basic Insertion & Min Property ---" << std::endl;
    heap_insert(h, 50);
    heap_insert(h, 10);
    heap_insert(h, 30);
    heap_insert(h, 5);
    
    assert(heap_size(h) == 4);
    heap_get_min(h, &val);
    assert(val == 5); 
    std::cout << "Min value is correctly: " << val << std::endl;

    std::cout << "--- Test 3: Sequential Deletion ---" << std::endl;
    // Extracting all should give: 5, 10, 30, 50
    heap_delete_min(h, &val); assert(val == 5);
    heap_delete_min(h, &val); assert(val == 10);
    heap_delete_min(h, &val); assert(val == 30);
    heap_delete_min(h, &val); assert(val == 50);
    assert(heap_empty(h) == true);

    std::cout << "--- Test 4: Comprehensive Stress Test (100 elements) ---" << std::endl;
    
    // 1. Insert 100 values in descending order (100 down to 1)
    // This forces the heap to sift-up the new minimum every single time. [cite: 47]
    for(int i = 100; i >= 1; i--) {
        int status = heap_insert(h, (heap_key_t)i);
        assert(status == 0); // Check for memory errors [cite: 70]
        
        // Verify get_min immediately after each insertion
        heap_get_min(h, &val);
        assert(val == (heap_key_t)i); 
    }
    
    assert(heap_size(h) == 100);
    std::cout << "All 100 insertions verified current min correctly." << std::endl;

    // 2. Delete all 100 values and verify they come out in order (1 up to 100)
    // This verifies the sift-down logic and the tiered array indexing. [cite: 48]
    for(int i = 1; i <= 100; i++) {
        // Check current min before deleting
        int get_status = heap_get_min(h, &val);
        assert(get_status == 0);
        assert(val == (heap_key_t)i);

        // Delete the min and verify the value returned via 'out' [cite: 65, 69]
        heap_key_t deleted_val;
        int del_status = heap_delete_min(h, &deleted_val);
        assert(del_status == 0);
        assert(deleted_val == (heap_key_t)i);
        
        // Verify size decreased
        assert(heap_size(h) == (size_t)(100 - i));
    }

    assert(heap_empty(h) == true);
    std::cout << "All 100 deletions verified sequential order correctly." << std::endl;

    heap_destroy(h);
    std::cout << "\nSUCCESS: All tests passed!" << std::endl;

    return 0;
}