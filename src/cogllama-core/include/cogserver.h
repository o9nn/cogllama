/**
 * @file cogserver.h
 * @brief CogServer for distributed cognitive orchestration
 * 
 * Manages cognitive cycles, attention allocation, and multi-agent
 * coordination using tensor-based message passing.
 */

#ifndef COGLLAMA_COGSERVER_H
#define COGLLAMA_COGSERVER_H

#include "atomspace.h"
#include "ggml.h"
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Server configuration parameters
 */
struct cogllama_server_params {
    size_t n_threads;           // Number of threads for parallel processing
    size_t max_atoms;           // Maximum atoms in AtomSpace
    size_t embedding_dim;       // Dimensionality of embeddings
    float attention_decay;      // Decay rate for attention values
    float learning_rate;        // Learning rate for adaptation
};

/**
 * Attention bank for ECAN-style attention allocation
 */
struct cogllama_attention_bank {
    float sti_funds;            // Short-term importance funds
    float lti_funds;            // Long-term importance funds
    float attention_spread_rate; // Rate of attention spreading
};

/**
 * CogServer structure - main cognitive orchestrator
 */
struct cogllama_cogserver {
    struct cogllama_atomspace *atomspace;
    struct ggml_threadpool *pool;
    struct cogllama_attention_bank *attention;
    struct cogllama_server_params params;
    size_t n_agents;
    size_t cycle_count;
    bool running;
};

/**
 * cogllama_cogserver_init - Initialize cognitive server
 * @params: Server configuration parameters
 * 
 * Initializes distributed cognitive processing infrastructure.
 * 
 * Returns: Pointer to initialized CogServer, or NULL on failure
 */
struct cogllama_cogserver *cogllama_cogserver_init(
    const struct cogllama_server_params *params
);

/**
 * cogllama_cogserver_free - Free CogServer and all resources
 * @server: CogServer to free
 */
void cogllama_cogserver_free(struct cogllama_cogserver *server);

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
 * 
 * Returns: 0 on success, -1 on failure
 */
int cogllama_cognitive_cycle(
    struct cogllama_cogserver *server,
    float delta_time
);

/**
 * cogllama_attention_allocate - Allocate attention across atoms
 * @server: CogServer instance
 * 
 * Implements ECAN-style economic attention allocation.
 * 
 * Returns: 0 on success, -1 on failure
 */
int cogllama_attention_allocate(
    struct cogllama_cogserver *server
);

/**
 * cogllama_attention_spread - Spread attention through hypergraph
 * @server: CogServer instance
 * @source_handle: Source atom for attention spreading
 * @amount: Amount of attention to spread
 * 
 * Spreads attention from source atom to connected atoms.
 * 
 * Returns: 0 on success, -1 on failure
 */
int cogllama_attention_spread(
    struct cogllama_cogserver *server,
    uint64_t source_handle,
    float amount
);

/**
 * cogllama_server_start - Start CogServer main loop
 * @server: CogServer instance
 * 
 * Starts the main cognitive processing loop.
 * Runs until cogllama_server_stop is called.
 * 
 * Returns: 0 on success, -1 on failure
 */
int cogllama_server_start(struct cogllama_cogserver *server);

/**
 * cogllama_server_stop - Stop CogServer main loop
 * @server: CogServer instance
 * 
 * Signals the server to stop processing.
 */
void cogllama_server_stop(struct cogllama_cogserver *server);

/**
 * cogllama_server_is_running - Check if server is running
 * @server: CogServer instance
 * 
 * Returns: true if running, false otherwise
 */
bool cogllama_server_is_running(const struct cogllama_cogserver *server);

/**
 * cogllama_server_get_cycle_count - Get number of cycles executed
 * @server: CogServer instance
 * 
 * Returns: Number of cognitive cycles executed
 */
size_t cogllama_server_get_cycle_count(const struct cogllama_cogserver *server);

#ifdef __cplusplus
}
#endif

#endif // COGLLAMA_COGSERVER_H
