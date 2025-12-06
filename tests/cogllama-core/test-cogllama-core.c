/**
 * @file test-cogllama-core.c
 * @brief Unit tests for CogLlama Core components
 */

#include "atomspace.h"
#include "cogserver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define TEST_MAX_ATOMS 100
#define TEST_EMBEDDING_DIM 64

// Test counter
static int tests_passed = 0;
static int tests_failed = 0;

#define TEST_ASSERT(condition, message) \
    do { \
        if (condition) { \
            tests_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            tests_failed++; \
            printf("  ✗ %s\n", message); \
        } \
    } while (0)

void test_atomspace_init() {
    printf("\nTest: AtomSpace Initialization\n");
    
    struct cogllama_atomspace *as = cogllama_atomspace_init(TEST_MAX_ATOMS, TEST_EMBEDDING_DIM);
    TEST_ASSERT(as != NULL, "AtomSpace initialized successfully");
    TEST_ASSERT(cogllama_atomspace_get_atom_count(as) == 0, "Initial atom count is zero");
    
    cogllama_atomspace_free(as);
}

void test_atom_creation() {
    printf("\nTest: Atom Creation\n");
    
    struct cogllama_atomspace *as = cogllama_atomspace_init(TEST_MAX_ATOMS, TEST_EMBEDDING_DIM);
    
    float embedding[TEST_EMBEDDING_DIM];
    for (size_t i = 0; i < TEST_EMBEDDING_DIM; i++) {
        embedding[i] = (float)i / TEST_EMBEDDING_DIM;
    }
    
    uint64_t atom1 = cogllama_atom_alloc(as, COGLLAMA_ATOM_CONCEPT_NODE, "Test1", embedding);
    TEST_ASSERT(atom1 == 1, "First atom has handle 1");
    TEST_ASSERT(cogllama_atomspace_get_atom_count(as) == 1, "Atom count is 1");
    
    uint64_t atom2 = cogllama_atom_alloc(as, COGLLAMA_ATOM_CONCEPT_NODE, "Test2", embedding);
    TEST_ASSERT(atom2 == 2, "Second atom has handle 2");
    TEST_ASSERT(cogllama_atomspace_get_atom_count(as) == 2, "Atom count is 2");
    
    cogllama_atomspace_free(as);
}

void test_truth_values() {
    printf("\nTest: Truth Values\n");
    
    struct cogllama_atomspace *as = cogllama_atomspace_init(TEST_MAX_ATOMS, TEST_EMBEDDING_DIM);
    
    float embedding[TEST_EMBEDDING_DIM] = {0};
    uint64_t atom = cogllama_atom_alloc(as, COGLLAMA_ATOM_CONCEPT_NODE, "Test", embedding);
    
    // Get default truth value
    float truth[2];
    int result = cogllama_atom_get_truth(as, atom, truth);
    TEST_ASSERT(result == 0, "Truth value query successful");
    TEST_ASSERT(truth[0] == 1.0f && truth[1] == 1.0f, "Default truth is [1.0, 1.0]");
    
    // Set custom truth value
    float new_truth[2] = {0.85f, 0.90f};
    result = cogllama_atom_set_truth(as, atom, new_truth);
    TEST_ASSERT(result == 0, "Truth value set successful");
    
    // Verify it was set
    result = cogllama_atom_get_truth(as, atom, truth);
    TEST_ASSERT(result == 0, "Truth value query successful");
    TEST_ASSERT(truth[0] == 0.85f && truth[1] == 0.90f, "Truth value correctly set");
    
    cogllama_atomspace_free(as);
}

void test_attention_values() {
    printf("\nTest: Attention Values\n");
    
    struct cogllama_atomspace *as = cogllama_atomspace_init(TEST_MAX_ATOMS, TEST_EMBEDDING_DIM);
    
    float embedding[TEST_EMBEDDING_DIM] = {0};
    uint64_t atom = cogllama_atom_alloc(as, COGLLAMA_ATOM_CONCEPT_NODE, "Test", embedding);
    
    // Get default attention
    float attention[3];
    int result = cogllama_atom_get_attention(as, atom, attention);
    TEST_ASSERT(result == 0, "Attention query successful");
    TEST_ASSERT(attention[0] == 0.0f && attention[1] == 0.0f && attention[2] == 0.0f, 
                "Default attention is [0, 0, 0]");
    
    // Set custom attention
    float new_attention[3] = {100.0f, 50.0f, 25.0f};
    result = cogllama_atom_set_attention(as, atom, new_attention);
    TEST_ASSERT(result == 0, "Attention set successful");
    
    // Verify it was set
    result = cogllama_atom_get_attention(as, atom, attention);
    TEST_ASSERT(result == 0, "Attention query successful");
    TEST_ASSERT(attention[0] == 100.0f && attention[1] == 50.0f && attention[2] == 25.0f,
                "Attention values correctly set");
    
    cogllama_atomspace_free(as);
}

void test_link_creation() {
    printf("\nTest: Link Creation\n");
    
    struct cogllama_atomspace *as = cogllama_atomspace_init(TEST_MAX_ATOMS, TEST_EMBEDDING_DIM);
    
    float embedding[TEST_EMBEDDING_DIM] = {0};
    uint64_t atom1 = cogllama_atom_alloc(as, COGLLAMA_ATOM_CONCEPT_NODE, "Cat", embedding);
    uint64_t atom2 = cogllama_atom_alloc(as, COGLLAMA_ATOM_CONCEPT_NODE, "Animal", embedding);
    
    uint64_t outgoing[2] = {atom1, atom2};
    float truth[2] = {0.95f, 0.90f};
    
    uint64_t link = cogllama_link_create(as, COGLLAMA_LINK_INHERITANCE, outgoing, 2, truth);
    TEST_ASSERT(link == 3, "Link created with handle 3");
    TEST_ASSERT(cogllama_atomspace_get_atom_count(as) == 3, "Atom count includes link");
    
    // Verify link truth value
    float link_truth[2];
    cogllama_atom_get_truth(as, link, link_truth);
    TEST_ASSERT(link_truth[0] == 0.95f && link_truth[1] == 0.90f, "Link truth value correct");
    
    cogllama_atomspace_free(as);
}

void test_cogserver_init() {
    printf("\nTest: CogServer Initialization\n");
    
    struct cogllama_server_params params = {
        .n_threads = 2,
        .max_atoms = TEST_MAX_ATOMS,
        .embedding_dim = TEST_EMBEDDING_DIM,
        .attention_decay = 0.95f,
        .learning_rate = 0.01f,
    };
    
    struct cogllama_cogserver *server = cogllama_cogserver_init(&params);
    TEST_ASSERT(server != NULL, "CogServer initialized successfully");
    TEST_ASSERT(!cogllama_server_is_running(server), "Server not running initially");
    TEST_ASSERT(cogllama_server_get_cycle_count(server) == 0, "Initial cycle count is 0");
    
    cogllama_cogserver_free(server);
}

void test_cognitive_cycles() {
    printf("\nTest: Cognitive Cycles\n");
    
    struct cogllama_server_params params = {
        .n_threads = 2,
        .max_atoms = TEST_MAX_ATOMS,
        .embedding_dim = TEST_EMBEDDING_DIM,
        .attention_decay = 0.95f,
        .learning_rate = 0.01f,
    };
    
    struct cogllama_cogserver *server = cogllama_cogserver_init(&params);
    
    int result = cogllama_cognitive_cycle(server, 0.1f);
    TEST_ASSERT(result == 0, "First cognitive cycle successful");
    TEST_ASSERT(cogllama_server_get_cycle_count(server) == 1, "Cycle count is 1");
    
    result = cogllama_cognitive_cycle(server, 0.1f);
    TEST_ASSERT(result == 0, "Second cognitive cycle successful");
    TEST_ASSERT(cogllama_server_get_cycle_count(server) == 2, "Cycle count is 2");
    
    cogllama_cogserver_free(server);
}

int main() {
    printf("=================================================\n");
    printf("CogLlama Core Unit Tests\n");
    printf("=================================================\n");
    
    test_atomspace_init();
    test_atom_creation();
    test_truth_values();
    test_attention_values();
    test_link_creation();
    test_cogserver_init();
    test_cognitive_cycles();
    
    printf("\n=================================================\n");
    printf("Test Results\n");
    printf("=================================================\n");
    printf("Passed: %d\n", tests_passed);
    printf("Failed: %d\n", tests_failed);
    printf("Total:  %d\n", tests_passed + tests_failed);
    
    if (tests_failed == 0) {
        printf("\n✅ All tests passed!\n\n");
        return 0;
    } else {
        printf("\n❌ Some tests failed!\n\n");
        return 1;
    }
}
