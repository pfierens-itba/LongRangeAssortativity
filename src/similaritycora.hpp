// Save the current packing alignment setting onto an internal stack and set the new alignment to 1 byte
#pragma pack(push, 1) 
typedef struct {
    uint8_t f[1433];
} featcora;

// Restore the previous packing alignment setting from the stack
#pragma pack(pop) 

void cacheSimilarityCora(double* simRow, void* features, igraph_integer_t N, igraph_integer_t n);