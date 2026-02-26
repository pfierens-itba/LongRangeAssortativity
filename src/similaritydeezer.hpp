// Save the current packing alignment setting onto an internal stack and set the new alignment to 1 byte
#pragma pack(push, 1) 
typedef struct {
    uint64_t lo;
    uint32_t hi;
} featdeezer;

// Restore the previous packing alignment setting from the stack
#pragma pack(pop) 

void cacheSimilarityDeezer(double* simRow, void* features, igraph_integer_t N, igraph_integer_t n);