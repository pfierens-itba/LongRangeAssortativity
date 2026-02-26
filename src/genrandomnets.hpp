
enum randomtypes {
    CONFIG_HEUR,    // 0
    CHUNG_LU,    // 1
    REWIRE,      // 2
};


int genrandomnet(igraph_t* graph, int type, igraph_integer_t no_of_nodes, igraph_integer_t nrewires, void* vector);

