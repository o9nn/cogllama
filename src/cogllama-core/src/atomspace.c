/**
 * @file atomspace.c
 * @brief Implementation of AtomSpace hypergraph using GGML tensors
 */

#include "atomspace.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**
 * cogllama_atomspace_init - Initialize AtomSpace
 */
struct cogllama_atomspace *cogllama_atomspace_init(
    size_t max_atoms,
    size_t embedding_dim
) {
    if (max_atoms == 0 || embedding_dim == 0) {
        fprintf(stderr, "cogllama_atomspace_init: invalid parameters\n");
        return NULL;
    }

    struct cogllama_atomspace *atomspace = 
        (struct cogllama_atomspace *)calloc(1, sizeof(struct cogllama_atomspace));
    
    if (!atomspace) {
        fprintf(stderr, "cogllama_atomspace_init: failed to allocate atomspace\n");
        return NULL;
    }

    atomspace->max_atoms = max_atoms;
    atomspace->embedding_dim = embedding_dim;
    atomspace->atom_count = 0;

    // Calculate memory requirements
    // NOTE: Current implementation uses dense O(n²) adjacency matrix for links
    // which is suitable for small-scale demonstrations (< 10K atoms).
    // For production with larger atom spaces, use sparse representation or
    // separate adjacency lists to avoid memory issues.
    size_t mem_size = 0;
    mem_size += max_atoms * embedding_dim * sizeof(float);  // atoms
    mem_size += max_atoms * max_atoms * sizeof(float);      // links (O(n²) - see note above)
    mem_size += max_atoms * 2 * sizeof(float);              // truth_values
    mem_size += max_atoms * 3 * sizeof(float);              // attention
    mem_size += 1024 * 1024;                                // overhead

    // Initialize GGML context
    struct ggml_init_params params = {
        .mem_size   = mem_size,
        .mem_buffer = NULL,
        .no_alloc   = false,
    };

    atomspace->ctx = ggml_init(params);
    if (!atomspace->ctx) {
        fprintf(stderr, "cogllama_atomspace_init: failed to initialize GGML context\n");
        free(atomspace);
        return NULL;
    }

    // Allocate tensors
    atomspace->atoms = ggml_new_tensor_2d(
        atomspace->ctx,
        GGML_TYPE_F32,
        embedding_dim,
        max_atoms
    );
    ggml_set_name(atomspace->atoms, "atoms");

    atomspace->links = ggml_new_tensor_2d(
        atomspace->ctx,
        GGML_TYPE_F32,
        max_atoms,
        max_atoms
    );
    ggml_set_name(atomspace->links, "links");

    atomspace->truth_values = ggml_new_tensor_2d(
        atomspace->ctx,
        GGML_TYPE_F32,
        2,  // [strength, confidence]
        max_atoms
    );
    ggml_set_name(atomspace->truth_values, "truth_values");

    atomspace->attention = ggml_new_tensor_2d(
        atomspace->ctx,
        GGML_TYPE_F32,
        3,  // [STI, LTI, VLTI]
        max_atoms
    );
    ggml_set_name(atomspace->attention, "attention");

    // Initialize tensors to zero
    memset(atomspace->atoms->data, 0, ggml_nbytes(atomspace->atoms));
    memset(atomspace->links->data, 0, ggml_nbytes(atomspace->links));
    memset(atomspace->truth_values->data, 0, ggml_nbytes(atomspace->truth_values));
    memset(atomspace->attention->data, 0, ggml_nbytes(atomspace->attention));

    return atomspace;
}

/**
 * cogllama_atomspace_free - Free AtomSpace and all resources
 */
void cogllama_atomspace_free(struct cogllama_atomspace *atomspace) {
    if (!atomspace) {
        return;
    }

    if (atomspace->ctx) {
        ggml_free(atomspace->ctx);
    }

    free(atomspace);
}

/**
 * cogllama_atom_alloc - Allocate new atom in hypergraph
 */
uint64_t cogllama_atom_alloc(
    struct cogllama_atomspace *atomspace,
    enum cogllama_atom_type type,
    const char *name,
    const float *embedding
) {
    if (!atomspace || !embedding) {
        fprintf(stderr, "cogllama_atom_alloc: invalid parameters\n");
        return 0;
    }

    if (atomspace->atom_count >= atomspace->max_atoms) {
        fprintf(stderr, "cogllama_atom_alloc: atomspace is full\n");
        return 0;
    }

    uint64_t atom_handle = atomspace->atom_count + 1;  // 1-indexed
    size_t index = atomspace->atom_count;

    // Copy embedding data to tensor
    float *atom_data = (float *)atomspace->atoms->data;
    size_t offset = index * atomspace->embedding_dim;
    memcpy(atom_data + offset, embedding, atomspace->embedding_dim * sizeof(float));

    // Initialize truth value to [1.0, 1.0] (full strength, full confidence)
    float *truth_data = (float *)atomspace->truth_values->data;
    truth_data[index * 2 + 0] = 1.0f;  // strength
    truth_data[index * 2 + 1] = 1.0f;  // confidence

    // Initialize attention to [0.0, 0.0, 0.0]
    float *attention_data = (float *)atomspace->attention->data;
    attention_data[index * 3 + 0] = 0.0f;  // STI
    attention_data[index * 3 + 1] = 0.0f;  // LTI
    attention_data[index * 3 + 2] = 0.0f;  // VLTI

    atomspace->atom_count++;

    return atom_handle;
}

/**
 * cogllama_link_create - Create link between atoms
 */
uint64_t cogllama_link_create(
    struct cogllama_atomspace *atomspace,
    enum cogllama_link_type link_type,
    const uint64_t *outgoing,
    size_t n_outgoing,
    const float truth[2]
) {
    if (!atomspace || !outgoing || n_outgoing == 0) {
        fprintf(stderr, "cogllama_link_create: invalid parameters\n");
        return 0;
    }

    // Create link atom
    float link_embedding[atomspace->embedding_dim];
    memset(link_embedding, 0, atomspace->embedding_dim * sizeof(float));
    
    // Encode link type in first element
    link_embedding[0] = (float)link_type;

    uint64_t link_handle = cogllama_atom_alloc(
        atomspace,
        COGLLAMA_ATOM_LINK,
        NULL,
        link_embedding
    );

    if (link_handle == 0) {
        return 0;
    }

    // Set truth value if provided
    if (truth) {
        cogllama_atom_set_truth(atomspace, link_handle, truth);
    }

    // Create adjacency entries in links tensor
    float *links_data = (float *)atomspace->links->data;
    size_t link_index = link_handle - 1;

    for (size_t i = 0; i < n_outgoing; i++) {
        uint64_t target_handle = outgoing[i];
        if (target_handle == 0 || target_handle > atomspace->atom_count) {
            fprintf(stderr, "cogllama_link_create: invalid target handle\n");
            continue;
        }

        size_t target_index = target_handle - 1;
        size_t offset = link_index * atomspace->max_atoms + target_index;
        links_data[offset] = 1.0f;  // Mark connection
    }

    return link_handle;
}

/**
 * cogllama_atom_get_truth - Get truth value of an atom
 */
int cogllama_atom_get_truth(
    struct cogllama_atomspace *atomspace,
    uint64_t atom_handle,
    float truth[2]
) {
    if (!atomspace || atom_handle == 0 || atom_handle > atomspace->atom_count || !truth) {
        return -1;
    }

    size_t index = atom_handle - 1;
    float *truth_data = (float *)atomspace->truth_values->data;
    
    truth[0] = truth_data[index * 2 + 0];  // strength
    truth[1] = truth_data[index * 2 + 1];  // confidence

    return 0;
}

/**
 * cogllama_atom_set_truth - Set truth value of an atom
 */
int cogllama_atom_set_truth(
    struct cogllama_atomspace *atomspace,
    uint64_t atom_handle,
    const float truth[2]
) {
    if (!atomspace || atom_handle == 0 || atom_handle > atomspace->atom_count || !truth) {
        return -1;
    }

    size_t index = atom_handle - 1;
    float *truth_data = (float *)atomspace->truth_values->data;
    
    truth_data[index * 2 + 0] = truth[0];  // strength
    truth_data[index * 2 + 1] = truth[1];  // confidence

    return 0;
}

/**
 * cogllama_atom_get_attention - Get attention values of an atom
 */
int cogllama_atom_get_attention(
    struct cogllama_atomspace *atomspace,
    uint64_t atom_handle,
    float attention[3]
) {
    if (!atomspace || atom_handle == 0 || atom_handle > atomspace->atom_count || !attention) {
        return -1;
    }

    size_t index = atom_handle - 1;
    float *attention_data = (float *)atomspace->attention->data;
    
    attention[0] = attention_data[index * 3 + 0];  // STI
    attention[1] = attention_data[index * 3 + 1];  // LTI
    attention[2] = attention_data[index * 3 + 2];  // VLTI

    return 0;
}

/**
 * cogllama_atom_set_attention - Set attention values of an atom
 */
int cogllama_atom_set_attention(
    struct cogllama_atomspace *atomspace,
    uint64_t atom_handle,
    const float attention[3]
) {
    if (!atomspace || atom_handle == 0 || atom_handle > atomspace->atom_count || !attention) {
        return -1;
    }

    size_t index = atom_handle - 1;
    float *attention_data = (float *)atomspace->attention->data;
    
    attention_data[index * 3 + 0] = attention[0];  // STI
    attention_data[index * 3 + 1] = attention[1];  // LTI
    attention_data[index * 3 + 2] = attention[2];  // VLTI

    return 0;
}

/**
 * cogllama_pattern_match - Pattern matching on hypergraph
 */
int cogllama_pattern_match(
    struct cogllama_atomspace *atomspace,
    const struct cogllama_pattern *pattern,
    cogllama_match_callback callback,
    void *user_data
) {
    if (!atomspace || !pattern || !callback) {
        return -1;
    }

    // Simplified pattern matching implementation
    // In a full implementation, this would use tensor operations for efficient matching
    
    int match_count = 0;
    
    // For now, return a placeholder implementation
    // Real implementation would use GGML operations for GPU-accelerated matching
    
    return match_count;
}

/**
 * cogllama_atomspace_get_atom_count - Get current number of atoms
 */
size_t cogllama_atomspace_get_atom_count(
    const struct cogllama_atomspace *atomspace
) {
    if (!atomspace) {
        return 0;
    }
    
    return atomspace->atom_count;
}
