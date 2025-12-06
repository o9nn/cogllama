# Component Group 1: cogllama-core

**Main orchestration engine implementing cognitive infrastructure**

## Components
- **cogutil**: Core utilities and data structures
- **atomspace**: Hypergraph-based knowledge representation
- **cogserver**: Distributed cognitive server

## GGML Implementation

### AtomSpace as Tensor Hypergraph

```c
/**
 * cogllama_atomspace - AtomSpace hypergraph representation as GGML tensors
 * 
 * Implements OpenCog's AtomSpace using GGML tensor graphs where:
 * - Atoms are tensors (nodes in hypergraph)
 * - Links are tensor operations (edges in hypergraph)
 * - Truth values are probability tensors
 * - Attention values are weight tensors
 */
struct cogllama_atomspace {
    struct ggml_context *ctx;           // GGML context for allocations
    struct ggml_tensor *atoms;          // Atom storage tensor (N x embedding_dim)
    struct ggml_tensor *links;          // Link adjacency tensor (N x N)
    struct ggml_tensor *truth_values;   // Truth value tensor (N x 2) [strength, confidence]
    struct ggml_tensor *attention;      // Attention value tensor (N x 3) [STI, LTI, VLTI]
    size_t atom_count;
    size_t max_atoms;
};

/**
 * cogllama_atom_alloc - Allocate new atom in hypergraph
 * @atomspace: AtomSpace context
 * @type: Atom type (Node, Link, etc.)
 * @name: Atom name/identifier
 * @embedding: Initial embedding vector
 * 
 * Creates a new atom as a tensor node in the hypergraph.
 * Returns atom handle for further operations.
 */
uint64_t cogllama_atom_alloc(
    struct cogllama_atomspace *atomspace,
    enum cogllama_atom_type type,
    const char *name,
    const float *embedding
);

/**
 * cogllama_link_create - Create link between atoms
 * @atomspace: AtomSpace context
 * @link_type: Type of link (InheritanceLink, SimilarityLink, etc.)
 * @outgoing: Array of atom handles to link
 * @n_outgoing: Number of atoms in outgoing set
 * @truth: Truth value [strength, confidence]
 * 
 * Creates a directed hyperedge connecting atoms.
 * Implements as tensor operation in GGML graph.
 */
uint64_t cogllama_link_create(
    struct cogllama_atomspace *atomspace,
    enum cogllama_link_type link_type,
    const uint64_t *outgoing,
    size_t n_outgoing,
    const float truth[2]
);

/**
 * cogllama_pattern_match - Pattern matching on hypergraph
 * @atomspace: AtomSpace context
 * @pattern: Pattern to match (with variables)
 * @callback: Callback for each match
 * @user_data: User data for callback
 * 
 * Implements hypergraph pattern matching using tensor operations.
 * Efficient GPU-accelerated pattern recognition.
 */
int cogllama_pattern_match(
    struct cogllama_atomspace *atomspace,
    const struct cogllama_pattern *pattern,
    cogllama_match_callback callback,
    void *user_data
);
```

### CogServer as Tensor Orchestrator

```c
/**
 * cogllama_cogserver - Distributed cognitive orchestration
 * 
 * Manages cognitive cycles, attention allocation, and multi-agent
 * coordination using tensor-based message passing.
 */
struct cogllama_cogserver {
    struct cogllama_atomspace *atomspace;
    struct ggml_threadpool *pool;
    struct cogllama_attention_bank *attention;
    size_t n_agents;
    bool running;
};

/**
 * cogllama_cogserver_init - Initialize cognitive server
 * @params: Server configuration parameters
 * 
 * Initializes distributed cognitive processing infrastructure.
 */
struct cogllama_cogserver *cogllama_cogserver_init(
    const struct cogllama_server_params *params
);

/**
 * cogllama_cognitive_cycle - Execute one cognitive cycle
 * @server: CogServer instance
 * @delta_time: Time since last cycle (seconds)
 * 
 * Executes one complete cognitive cycle:
 * 1. Attention allocation
 * 2. Pattern recognition
 * 3. Inference
 * 4. Learning
 * 5. Action selection
 */
int cogllama_cognitive_cycle(
    struct cogllama_cogserver *server,
    float delta_time
);
```

## Build System

```cmake
# cogllama-core/CMakeLists.txt
cmake_minimum_required(VERSION 3.15)
project(cogllama-core C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

# GGML dependency
add_subdirectory(../ggml ggml)

# Core library
add_library(cogllama-core SHARED
    src/atomspace.c
    src/atomspace_tensor.c
    src/cogserver.c
    src/cognitive_cycle.c
    src/cogutil.c
)

target_link_libraries(cogllama-core
    ggml
)

target_include_directories(cogllama-core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

install(TARGETS cogllama-core
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
```
