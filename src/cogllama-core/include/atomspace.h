/**
 * @file atomspace.h
 * @brief AtomSpace hypergraph representation using GGML tensors
 * 
 * This implements OpenCog's AtomSpace as a tensor-based hypergraph where:
 * - Atoms are tensors (nodes in hypergraph)
 * - Links are tensor operations (edges in hypergraph)  
 * - Truth values are probability tensors
 * - Attention values are weight tensors
 */

#ifndef COGLLAMA_ATOMSPACE_H
#define COGLLAMA_ATOMSPACE_H

#include "ggml.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Atom types - representing different kinds of nodes and links
 */
enum cogllama_atom_type {
    // Node types
    COGLLAMA_ATOM_NODE = 0,
    COGLLAMA_ATOM_CONCEPT_NODE,
    COGLLAMA_ATOM_PREDICATE_NODE,
    COGLLAMA_ATOM_VARIABLE_NODE,
    COGLLAMA_ATOM_NUMBER_NODE,
    COGLLAMA_ATOM_SCHEMA_NODE,          // For procedures/functions
    
    // Link types
    COGLLAMA_ATOM_LINK = 100,
    COGLLAMA_ATOM_INHERITANCE_LINK,
    COGLLAMA_ATOM_SIMILARITY_LINK,
    COGLLAMA_ATOM_EVALUATION_LINK,
    COGLLAMA_ATOM_IMPLICATION_LINK,
    COGLLAMA_ATOM_AND_LINK,
    COGLLAMA_ATOM_OR_LINK,
    COGLLAMA_ATOM_NOT_LINK,
    
    // Expression link types
    COGLLAMA_ATOM_EXECUTION_LINK,       // ExecutionLink - execute procedures
    COGLLAMA_ATOM_LAMBDA_LINK,          // LambdaLink - lambda expressions
    COGLLAMA_ATOM_GET_LINK,             // GetLink - pattern matching queries
    COGLLAMA_ATOM_PUT_LINK,             // PutLink - variable substitution
    COGLLAMA_ATOM_DEFINE_LINK,          // DefineLink - name definitions
    COGLLAMA_ATOM_QUOTE_LINK,           // QuoteLink - prevent evaluation
    COGLLAMA_ATOM_UNQUOTE_LINK,         // UnquoteLink - force evaluation
};

/**
 * Link types for different relationships
 */
enum cogllama_link_type {
    COGLLAMA_LINK_INHERITANCE = 0,
    COGLLAMA_LINK_SIMILARITY,
    COGLLAMA_LINK_EVALUATION,
    COGLLAMA_LINK_IMPLICATION,
    COGLLAMA_LINK_AND,
    COGLLAMA_LINK_OR,
    COGLLAMA_LINK_NOT,
    COGLLAMA_LINK_EXECUTION,            // ExecutionLink
    COGLLAMA_LINK_LAMBDA,               // LambdaLink
    COGLLAMA_LINK_GET,                  // GetLink
    COGLLAMA_LINK_PUT,                  // PutLink
};

/**
 * AtomSpace structure - the main hypergraph knowledge representation
 */
struct cogllama_atomspace {
    struct ggml_context *ctx;           // GGML context for allocations
    struct ggml_tensor *atoms;          // Atom storage tensor (N x embedding_dim)
    struct ggml_tensor *links;          // Link adjacency tensor (N x N)
    struct ggml_tensor *truth_values;   // Truth value tensor (N x 2) [strength, confidence]
    struct ggml_tensor *attention;      // Attention value tensor (N x 3) [STI, LTI, VLTI]
    size_t atom_count;
    size_t max_atoms;
    size_t embedding_dim;
};

/**
 * Pattern structure for pattern matching
 */
struct cogllama_pattern {
    struct ggml_tensor *pattern_atoms;
    struct ggml_tensor *variables;
    size_t n_atoms;
    size_t n_variables;
};

/**
 * Match callback function type
 */
typedef void (*cogllama_match_callback)(
    const uint64_t *matches,
    size_t n_matches,
    void *user_data
);

/**
 * cogllama_atomspace_init - Initialize AtomSpace
 * @max_atoms: Maximum number of atoms to support
 * @embedding_dim: Dimensionality of atom embeddings
 * 
 * Creates a new AtomSpace with tensor-based hypergraph representation.
 * 
 * Returns: Pointer to initialized AtomSpace, or NULL on failure
 */
struct cogllama_atomspace *cogllama_atomspace_init(
    size_t max_atoms,
    size_t embedding_dim
);

/**
 * cogllama_atomspace_free - Free AtomSpace and all resources
 * @atomspace: AtomSpace to free
 */
void cogllama_atomspace_free(struct cogllama_atomspace *atomspace);

/**
 * cogllama_atom_alloc - Allocate new atom in hypergraph
 * @atomspace: AtomSpace context
 * @type: Atom type (Node, Link, etc.)
 * @name: Atom name/identifier
 * @embedding: Initial embedding vector (embedding_dim floats)
 * 
 * Creates a new atom as a tensor node in the hypergraph.
 * 
 * Returns: Atom handle (index) for further operations, or 0 on failure
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
 * 
 * Returns: Link handle (index) for further operations, or 0 on failure
 */
uint64_t cogllama_link_create(
    struct cogllama_atomspace *atomspace,
    enum cogllama_link_type link_type,
    const uint64_t *outgoing,
    size_t n_outgoing,
    const float truth[2]
);

/**
 * cogllama_atom_get_truth - Get truth value of an atom
 * @atomspace: AtomSpace context
 * @atom_handle: Handle of atom to query
 * @truth: Output array [strength, confidence]
 * 
 * Returns: 0 on success, -1 on failure
 */
int cogllama_atom_get_truth(
    struct cogllama_atomspace *atomspace,
    uint64_t atom_handle,
    float truth[2]
);

/**
 * cogllama_atom_set_truth - Set truth value of an atom
 * @atomspace: AtomSpace context
 * @atom_handle: Handle of atom to modify
 * @truth: New truth value [strength, confidence]
 * 
 * Returns: 0 on success, -1 on failure
 */
int cogllama_atom_set_truth(
    struct cogllama_atomspace *atomspace,
    uint64_t atom_handle,
    const float truth[2]
);

/**
 * cogllama_atom_get_attention - Get attention values of an atom
 * @atomspace: AtomSpace context
 * @atom_handle: Handle of atom to query
 * @attention: Output array [STI, LTI, VLTI]
 * 
 * Returns: 0 on success, -1 on failure
 */
int cogllama_atom_get_attention(
    struct cogllama_atomspace *atomspace,
    uint64_t atom_handle,
    float attention[3]
);

/**
 * cogllama_atom_set_attention - Set attention values of an atom
 * @atomspace: AtomSpace context
 * @atom_handle: Handle of atom to modify
 * @attention: New attention values [STI, LTI, VLTI]
 * 
 * Returns: 0 on success, -1 on failure
 */
int cogllama_atom_set_attention(
    struct cogllama_atomspace *atomspace,
    uint64_t atom_handle,
    const float attention[3]
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
 * 
 * Returns: Number of matches found, or -1 on failure
 */
int cogllama_pattern_match(
    struct cogllama_atomspace *atomspace,
    const struct cogllama_pattern *pattern,
    cogllama_match_callback callback,
    void *user_data
);

/**
 * cogllama_atomspace_get_atom_count - Get current number of atoms
 * @atomspace: AtomSpace context
 * 
 * Returns: Current number of atoms in the AtomSpace
 */
size_t cogllama_atomspace_get_atom_count(
    const struct cogllama_atomspace *atomspace
);

#ifdef __cplusplus
}
#endif

#endif // COGLLAMA_ATOMSPACE_H
