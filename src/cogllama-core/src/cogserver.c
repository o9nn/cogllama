/**
 * @file cogserver.c
 * @brief Implementation of CogServer for cognitive orchestration
 */

#include "cogserver.h"
#include "ggml-cpu.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

/**
 * cogllama_cogserver_init - Initialize cognitive server
 */
struct cogllama_cogserver *cogllama_cogserver_init(
    const struct cogllama_server_params *params
) {
    if (!params) {
        fprintf(stderr, "cogllama_cogserver_init: invalid parameters\n");
        return NULL;
    }

    struct cogllama_cogserver *server = 
        (struct cogllama_cogserver *)calloc(1, sizeof(struct cogllama_cogserver));
    
    if (!server) {
        fprintf(stderr, "cogllama_cogserver_init: failed to allocate server\n");
        return NULL;
    }

    // Copy parameters
    server->params = *params;
    server->running = false;
    server->cycle_count = 0;
    server->n_agents = 1;  // Default to single agent

    // Initialize AtomSpace
    server->atomspace = cogllama_atomspace_init(
        params->max_atoms,
        params->embedding_dim
    );

    if (!server->atomspace) {
        fprintf(stderr, "cogllama_cogserver_init: failed to initialize atomspace\n");
        free(server);
        return NULL;
    }

    // Initialize attention bank
    server->attention = (struct cogllama_attention_bank *)
        calloc(1, sizeof(struct cogllama_attention_bank));
    
    if (!server->attention) {
        fprintf(stderr, "cogllama_cogserver_init: failed to allocate attention bank\n");
        cogllama_atomspace_free(server->atomspace);
        free(server);
        return NULL;
    }

    // Initialize attention funds (ECAN-style)
    server->attention->sti_funds = 100000.0f;  // Total STI in economy
    server->attention->lti_funds = 100000.0f;  // Total LTI in economy
    server->attention->attention_spread_rate = 0.1f;

    // Initialize thread pool if requested
    if (params->n_threads > 1) {
        struct ggml_threadpool_params tp_params = {
            .n_threads = params->n_threads,
        };
        server->pool = ggml_threadpool_new(&tp_params);
        if (!server->pool) {
            fprintf(stderr, "cogllama_cogserver_init: warning: failed to create thread pool\n");
        }
    }

    return server;
}

/**
 * cogllama_cogserver_free - Free CogServer and all resources
 */
void cogllama_cogserver_free(struct cogllama_cogserver *server) {
    if (!server) {
        return;
    }

    if (server->pool) {
        ggml_threadpool_free(server->pool);
    }

    if (server->attention) {
        free(server->attention);
    }

    if (server->atomspace) {
        cogllama_atomspace_free(server->atomspace);
    }

    free(server);
}

/**
 * cogllama_attention_allocate - Allocate attention across atoms
 */
int cogllama_attention_allocate(
    struct cogllama_cogserver *server
) {
    if (!server || !server->atomspace) {
        return -1;
    }

    // Simple attention allocation based on importance spreading
    size_t atom_count = cogllama_atomspace_get_atom_count(server->atomspace);
    
    if (atom_count == 0) {
        return 0;  // Nothing to allocate
    }

    // Apply attention decay to all atoms
    for (size_t i = 1; i <= atom_count; i++) {
        float attention[3];
        if (cogllama_atom_get_attention(server->atomspace, i, attention) == 0) {
            // Decay STI (short-term importance)
            attention[0] *= server->params.attention_decay;
            
            // Small drift from STI to LTI (long-term importance)
            float drift = attention[0] * 0.01f;
            attention[0] -= drift;
            attention[1] += drift;
            
            cogllama_atom_set_attention(server->atomspace, i, attention);
        }
    }

    return 0;
}

/**
 * cogllama_attention_spread - Spread attention through hypergraph
 */
int cogllama_attention_spread(
    struct cogllama_cogserver *server,
    uint64_t source_handle,
    float amount
) {
    if (!server || !server->atomspace || source_handle == 0) {
        return -1;
    }

    // Get source attention
    float source_attention[3];
    if (cogllama_atom_get_attention(server->atomspace, source_handle, source_attention) != 0) {
        return -1;
    }

    // Spread portion of attention to connected atoms
    // This is a simplified implementation
    // Real implementation would traverse links in the hypergraph
    
    float spread_amount = amount * server->attention->attention_spread_rate;
    source_attention[0] -= spread_amount;  // Reduce source STI
    
    cogllama_atom_set_attention(server->atomspace, source_handle, source_attention);

    return 0;
}

/**
 * cogllama_cognitive_cycle - Execute one cognitive cycle
 */
int cogllama_cognitive_cycle(
    struct cogllama_cogserver *server,
    float delta_time
) {
    if (!server) {
        return -1;
    }

    // Phase 1: Attention allocation
    if (cogllama_attention_allocate(server) != 0) {
        fprintf(stderr, "cogllama_cognitive_cycle: attention allocation failed\n");
        return -1;
    }

    // Phase 2: Pattern recognition
    // TODO: Implement pattern recognition using pattern matching

    // Phase 3: Inference
    // TODO: Implement PLN-style inference

    // Phase 4: Learning
    // TODO: Implement learning mechanisms

    // Phase 5: Action selection
    // TODO: Implement action selection based on goals and attention

    server->cycle_count++;

    return 0;
}

/**
 * cogllama_server_start - Start CogServer main loop
 */
int cogllama_server_start(struct cogllama_cogserver *server) {
    if (!server) {
        return -1;
    }

    server->running = true;

    // Main cognitive loop
    while (server->running) {
        // Execute one cognitive cycle
        if (cogllama_cognitive_cycle(server, 0.1f) != 0) {
            fprintf(stderr, "cogllama_server_start: cognitive cycle failed\n");
            break;
        }

        // In a real implementation, this would have proper timing control
        // For now, we rely on external control via cogllama_server_stop
    }

    return 0;
}

/**
 * cogllama_server_stop - Stop CogServer main loop
 */
void cogllama_server_stop(struct cogllama_cogserver *server) {
    if (server) {
        server->running = false;
    }
}

/**
 * cogllama_server_is_running - Check if server is running
 */
bool cogllama_server_is_running(const struct cogllama_cogserver *server) {
    return server ? server->running : false;
}

/**
 * cogllama_server_get_cycle_count - Get number of cycles executed
 */
size_t cogllama_server_get_cycle_count(const struct cogllama_cogserver *server) {
    return server ? server->cycle_count : 0;
}
