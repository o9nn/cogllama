/**
 * @file cogllama-demo.c
 * @brief Demonstration of CogLlama core functionality
 * 
 * This program demonstrates the basic AtomSpace and CogServer functionality
 * with tensor-based hypergraph representation.
 */

#include "atomspace.h"
#include "cogserver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_separator(const char *title) {
    printf("\n");
    printf("=================================================\n");
    printf(" %s\n", title);
    printf("=================================================\n");
}

int main(int argc, char **argv) {
    printf("CogLlama Core Demonstration\n");
    printf("Tensor-based OpenCog AtomSpace with GGML\n");

    // Initialize AtomSpace
    print_separator("Initializing AtomSpace");
    
    size_t max_atoms = 1000;
    size_t embedding_dim = 128;
    
    struct cogllama_atomspace *atomspace = cogllama_atomspace_init(max_atoms, embedding_dim);
    if (!atomspace) {
        fprintf(stderr, "Failed to initialize AtomSpace\n");
        return 1;
    }
    
    printf("AtomSpace initialized:\n");
    printf("  Max atoms: %zu\n", max_atoms);
    printf("  Embedding dimension: %zu\n", embedding_dim);
    printf("  Current atoms: %zu\n", cogllama_atomspace_get_atom_count(atomspace));

    // Create some concept nodes
    print_separator("Creating Concept Nodes");
    
    // Use dynamic allocation to avoid VLA stack issues
    float *cat_embedding = (float *)malloc(embedding_dim * sizeof(float));
    float *animal_embedding = (float *)malloc(embedding_dim * sizeof(float));
    float *mammal_embedding = (float *)malloc(embedding_dim * sizeof(float));
    
    if (!cat_embedding || !animal_embedding || !mammal_embedding) {
        fprintf(stderr, "Failed to allocate embeddings\n");
        cogllama_atomspace_free(atomspace);
        free(cat_embedding);
        free(animal_embedding);
        free(mammal_embedding);
        return 1;
    }
    
    // Initialize embeddings with simple values
    for (size_t i = 0; i < embedding_dim; i++) {
        cat_embedding[i] = 0.1f + (float)i / embedding_dim;
        animal_embedding[i] = 0.2f + (float)i / embedding_dim;
        mammal_embedding[i] = 0.15f + (float)i / embedding_dim;
    }
    
    uint64_t cat = cogllama_atom_alloc(atomspace, COGLLAMA_ATOM_CONCEPT_NODE, "Cat", cat_embedding);
    uint64_t animal = cogllama_atom_alloc(atomspace, COGLLAMA_ATOM_CONCEPT_NODE, "Animal", animal_embedding);
    uint64_t mammal = cogllama_atom_alloc(atomspace, COGLLAMA_ATOM_CONCEPT_NODE, "Mammal", mammal_embedding);
    
    printf("Created concept nodes:\n");
    printf("  Cat (handle: %lu)\n", (unsigned long)cat);
    printf("  Animal (handle: %lu)\n", (unsigned long)animal);
    printf("  Mammal (handle: %lu)\n", (unsigned long)mammal);
    printf("  Total atoms: %zu\n", cogllama_atomspace_get_atom_count(atomspace));

    // Create inheritance links
    print_separator("Creating Inheritance Links");
    
    float truth_high[2] = {0.95f, 0.90f};  // [strength, confidence]
    float truth_medium[2] = {0.85f, 0.85f};
    
    uint64_t outgoing1[2] = {cat, mammal};
    uint64_t link1 = cogllama_link_create(
        atomspace,
        COGLLAMA_LINK_INHERITANCE,
        outgoing1,
        2,
        truth_high
    );
    
    uint64_t outgoing2[2] = {mammal, animal};
    uint64_t link2 = cogllama_link_create(
        atomspace,
        COGLLAMA_LINK_INHERITANCE,
        outgoing2,
        2,
        truth_medium
    );
    
    printf("Created inheritance links:\n");
    printf("  Cat -> Mammal (handle: %lu)\n", (unsigned long)link1);
    printf("  Mammal -> Animal (handle: %lu)\n", (unsigned long)link2);
    printf("  Total atoms: %zu\n", cogllama_atomspace_get_atom_count(atomspace));

    // Query truth values
    print_separator("Querying Truth Values");
    
    float truth[2];
    if (cogllama_atom_get_truth(atomspace, link1, truth) == 0) {
        printf("Link1 truth value: [strength: %.2f, confidence: %.2f]\n", truth[0], truth[1]);
    }
    if (cogllama_atom_get_truth(atomspace, link2, truth) == 0) {
        printf("Link2 truth value: [strength: %.2f, confidence: %.2f]\n", truth[0], truth[1]);
    }

    // Test attention values
    print_separator("Testing Attention Values");
    
    float attention[3] = {100.0f, 50.0f, 0.0f};  // [STI, LTI, VLTI]
    cogllama_atom_set_attention(atomspace, cat, attention);
    
    float read_attention[3];
    if (cogllama_atom_get_attention(atomspace, cat, read_attention) == 0) {
        printf("Cat attention: [STI: %.1f, LTI: %.1f, VLTI: %.1f]\n",
               read_attention[0], read_attention[1], read_attention[2]);
    }

    // Initialize CogServer
    print_separator("Initializing CogServer");
    
    struct cogllama_server_params server_params = {
        .n_threads = 4,
        .max_atoms = max_atoms,
        .embedding_dim = embedding_dim,
        .attention_decay = 0.95f,
        .learning_rate = 0.01f,
    };
    
    struct cogllama_cogserver *server = cogllama_cogserver_init(&server_params);
    if (!server) {
        fprintf(stderr, "Failed to initialize CogServer\n");
        cogllama_atomspace_free(atomspace);
        return 1;
    }
    
    printf("CogServer initialized:\n");
    printf("  Threads: %zu\n", server_params.n_threads);
    printf("  Attention decay: %.2f\n", server_params.attention_decay);

    // Run a few cognitive cycles
    print_separator("Running Cognitive Cycles");
    
    for (int i = 0; i < 5; i++) {
        if (cogllama_cognitive_cycle(server, 0.1f) == 0) {
            printf("Cycle %d completed (total: %zu)\n", 
                   i + 1, cogllama_server_get_cycle_count(server));
        }
    }

    // Cleanup
    print_separator("Cleanup");
    
    printf("Freeing CogServer...\n");
    cogllama_cogserver_free(server);
    
    printf("Freeing AtomSpace...\n");
    cogllama_atomspace_free(atomspace);
    
    // Free embeddings
    free(cat_embedding);
    free(animal_embedding);
    free(mammal_embedding);
    
    printf("Demo completed successfully!\n\n");

    return 0;
}
