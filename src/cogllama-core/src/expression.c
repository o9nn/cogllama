/**
 * @file expression.c
 * @brief Implementation of CogLlama expressions
 */

#include "expression.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

/**
 * Internal structure for procedure registry
 */
struct procedure_entry {
    char *name;
    cogllama_procedure_fn procedure;
    struct procedure_entry *next;
};

/**
 * Extended expression context with internal data
 */
struct cogllama_expression_context_internal {
    struct cogllama_expression_context base;
    struct procedure_entry *procedures;
    clock_t start_time;
};

/**
 * cogllama_expression_context_init - Initialize expression context
 */
struct cogllama_expression_context *cogllama_expression_context_init(
    struct cogllama_atomspace *atomspace,
    size_t max_recursion_depth,
    float timeout_seconds
) {
    if (!atomspace) {
        fprintf(stderr, "cogllama_expression_context_init: invalid atomspace\n");
        return NULL;
    }

    struct cogllama_expression_context_internal *internal =
        (struct cogllama_expression_context_internal *)calloc(
            1, sizeof(struct cogllama_expression_context_internal));
    
    if (!internal) {
        fprintf(stderr, "cogllama_expression_context_init: allocation failed\n");
        return NULL;
    }

    internal->base.atomspace = atomspace;
    internal->base.ctx = atomspace->ctx;
    internal->base.max_recursion_depth = max_recursion_depth;
    internal->base.current_depth = 0;
    internal->base.timeout_seconds = timeout_seconds;
    internal->base.user_data = NULL;
    internal->procedures = NULL;
    internal->start_time = clock();

    return (struct cogllama_expression_context *)internal;
}

/**
 * cogllama_expression_context_free - Free expression context
 */
void cogllama_expression_context_free(
    struct cogllama_expression_context *context
) {
    if (!context) {
        return;
    }

    struct cogllama_expression_context_internal *internal =
        (struct cogllama_expression_context_internal *)context;

    // Free procedure registry
    struct procedure_entry *entry = internal->procedures;
    while (entry) {
        struct procedure_entry *next = entry->next;
        free(entry->name);
        free(entry);
        entry = next;
    }

    free(internal);
}

/**
 * cogllama_execution_link_create - Create ExecutionLink
 */
uint64_t cogllama_execution_link_create(
    struct cogllama_atomspace *atomspace,
    uint64_t procedure_handle,
    const uint64_t *args,
    size_t n_args
) {
    if (!atomspace || procedure_handle == 0) {
        fprintf(stderr, "cogllama_execution_link_create: invalid parameters\n");
        return 0;
    }

    // Create outgoing set: [procedure, arg1, arg2, ...]
    size_t outgoing_size = n_args + 1;
    uint64_t *outgoing = (uint64_t *)malloc(outgoing_size * sizeof(uint64_t));
    if (!outgoing) {
        fprintf(stderr, "cogllama_execution_link_create: allocation failed\n");
        return 0;
    }

    outgoing[0] = procedure_handle;
    for (size_t i = 0; i < n_args; i++) {
        outgoing[i + 1] = args[i];
    }

    // Create link with EXECUTION type
    float truth[2] = {1.0f, 1.0f};
    uint64_t link_handle = cogllama_link_create(
        atomspace,
        COGLLAMA_LINK_EXECUTION,  // Using EXECUTION for execution
        outgoing,
        outgoing_size,
        truth
    );

    free(outgoing);
    return link_handle;
}

/**
 * Check if execution has timed out
 */
static bool check_timeout(struct cogllama_expression_context_internal *internal) {
    if (internal->base.timeout_seconds <= 0.0f) {
        return false;
    }

    clock_t current = clock();
    double elapsed = (double)(current - internal->start_time) / CLOCKS_PER_SEC;
    return elapsed > internal->base.timeout_seconds;
}

/**
 * cogllama_execution_link_eval - Evaluate ExecutionLink
 */
struct cogllama_execution_result cogllama_execution_link_eval(
    struct cogllama_expression_context *context,
    uint64_t execution_link_handle
) {
    struct cogllama_execution_result result = {
        .status = COGLLAMA_EXEC_ERROR,
        .result_handle = 0,
        .execution_time = 0.0f,
        .error_message = NULL
    };

    if (!context || execution_link_handle == 0) {
        result.error_message = strdup("Invalid context or handle");
        return result;
    }

    struct cogllama_expression_context_internal *internal =
        (struct cogllama_expression_context_internal *)context;

    // Check timeout
    if (check_timeout(internal)) {
        result.status = COGLLAMA_EXEC_TIMEOUT;
        result.error_message = strdup("Execution timeout");
        return result;
    }

    // Check recursion depth
    if (internal->base.max_recursion_depth > 0 &&
        internal->base.current_depth >= internal->base.max_recursion_depth) {
        result.error_message = strdup("Maximum recursion depth exceeded");
        return result;
    }

    clock_t start = clock();

    // For now, return a placeholder successful result
    // In a full implementation, this would:
    // 1. Extract procedure and arguments from the link
    // 2. Look up procedure in registry
    // 3. Execute procedure with arguments
    // 4. Return result atom
    
    result.status = COGLLAMA_EXEC_SUCCESS;
    result.result_handle = execution_link_handle;  // Placeholder
    
    clock_t end = clock();
    result.execution_time = (float)(end - start) / CLOCKS_PER_SEC;

    return result;
}

/**
 * cogllama_lambda_link_create - Create LambdaLink
 */
uint64_t cogllama_lambda_link_create(
    struct cogllama_atomspace *atomspace,
    const uint64_t *variable_handles,
    size_t n_variables,
    uint64_t body_handle
) {
    if (!atomspace || !variable_handles || n_variables == 0 || body_handle == 0) {
        fprintf(stderr, "cogllama_lambda_link_create: invalid parameters\n");
        return 0;
    }

    // Create outgoing set: [variables..., body]
    size_t outgoing_size = n_variables + 1;
    uint64_t *outgoing = (uint64_t *)malloc(outgoing_size * sizeof(uint64_t));
    if (!outgoing) {
        fprintf(stderr, "cogllama_lambda_link_create: allocation failed\n");
        return 0;
    }

    for (size_t i = 0; i < n_variables; i++) {
        outgoing[i] = variable_handles[i];
    }
    outgoing[n_variables] = body_handle;

    float truth[2] = {1.0f, 1.0f};
    uint64_t link_handle = cogllama_link_create(
        atomspace,
        COGLLAMA_LINK_LAMBDA,  // Using LAMBDA for lambda
        outgoing,
        outgoing_size,
        truth
    );

    free(outgoing);
    return link_handle;
}

/**
 * cogllama_lambda_link_apply - Apply lambda to arguments
 */
struct cogllama_execution_result cogllama_lambda_link_apply(
    struct cogllama_expression_context *context,
    uint64_t lambda_handle,
    const uint64_t *args,
    size_t n_args
) {
    struct cogllama_execution_result result = {
        .status = COGLLAMA_EXEC_ERROR,
        .result_handle = 0,
        .execution_time = 0.0f,
        .error_message = NULL
    };

    if (!context || lambda_handle == 0 || !args) {
        result.error_message = strdup("Invalid parameters");
        return result;
    }

    struct cogllama_expression_context_internal *internal =
        (struct cogllama_expression_context_internal *)context;

    // Check timeout
    if (check_timeout(internal)) {
        result.status = COGLLAMA_EXEC_TIMEOUT;
        result.error_message = strdup("Execution timeout");
        return result;
    }

    clock_t start = clock();

    // Increment recursion depth
    internal->base.current_depth++;

    // Placeholder implementation
    // Full implementation would:
    // 1. Extract variables and body from lambda
    // 2. Create variable bindings with arguments
    // 3. Substitute variables in body
    // 4. Evaluate substituted body
    
    result.status = COGLLAMA_EXEC_SUCCESS;
    result.result_handle = lambda_handle;  // Placeholder

    // Decrement recursion depth
    internal->base.current_depth--;

    clock_t end = clock();
    result.execution_time = (float)(end - start) / CLOCKS_PER_SEC;

    return result;
}

/**
 * cogllama_get_link_create - Create GetLink
 */
uint64_t cogllama_get_link_create(
    struct cogllama_atomspace *atomspace,
    uint64_t pattern_handle,
    const uint64_t *variable_handles,
    size_t n_variables
) {
    if (!atomspace || pattern_handle == 0) {
        fprintf(stderr, "cogllama_get_link_create: invalid parameters\n");
        return 0;
    }

    // Create outgoing set: [pattern, variables...]
    size_t outgoing_size = n_variables + 1;
    uint64_t *outgoing = (uint64_t *)malloc(outgoing_size * sizeof(uint64_t));
    if (!outgoing) {
        fprintf(stderr, "cogllama_get_link_create: allocation failed\n");
        return 0;
    }

    outgoing[0] = pattern_handle;
    for (size_t i = 0; i < n_variables; i++) {
        outgoing[i + 1] = variable_handles[i];
    }

    float truth[2] = {1.0f, 1.0f};
    uint64_t link_handle = cogllama_link_create(
        atomspace,
        COGLLAMA_LINK_GET,  // Using GET for get
        outgoing,
        outgoing_size,
        truth
    );

    free(outgoing);
    return link_handle;
}

/**
 * cogllama_get_link_eval - Evaluate GetLink
 */
struct cogllama_query_result cogllama_get_link_eval(
    struct cogllama_expression_context *context,
    uint64_t get_link_handle
) {
    struct cogllama_query_result result = {
        .bindings = NULL,
        .n_results = 0,
        .n_bindings_per_result = 0
    };

    if (!context || get_link_handle == 0) {
        return result;
    }

    // Placeholder implementation
    // Full implementation would:
    // 1. Extract pattern and variables from GetLink
    // 2. Use pattern matching on atomspace
    // 3. Return all matching variable bindings
    
    return result;
}

/**
 * cogllama_put_link_create - Create PutLink
 */
uint64_t cogllama_put_link_create(
    struct cogllama_atomspace *atomspace,
    uint64_t template_handle,
    const struct cogllama_variable_binding *bindings,
    size_t n_bindings
) {
    if (!atomspace || template_handle == 0 || !bindings) {
        fprintf(stderr, "cogllama_put_link_create: invalid parameters\n");
        return 0;
    }

    // Create outgoing set: [template, binding_atoms...]
    size_t outgoing_size = n_bindings + 1;
    uint64_t *outgoing = (uint64_t *)malloc(outgoing_size * sizeof(uint64_t));
    if (!outgoing) {
        fprintf(stderr, "cogllama_put_link_create: allocation failed\n");
        return 0;
    }

    outgoing[0] = template_handle;
    for (size_t i = 0; i < n_bindings; i++) {
        outgoing[i + 1] = bindings[i].atom_handle;
    }

    float truth[2] = {1.0f, 1.0f};
    uint64_t link_handle = cogllama_link_create(
        atomspace,
        COGLLAMA_LINK_PUT,  // Using PUT for put
        outgoing,
        outgoing_size,
        truth
    );

    free(outgoing);
    return link_handle;
}

/**
 * cogllama_put_link_eval - Evaluate PutLink
 */
struct cogllama_execution_result cogllama_put_link_eval(
    struct cogllama_expression_context *context,
    uint64_t put_link_handle
) {
    struct cogllama_execution_result result = {
        .status = COGLLAMA_EXEC_ERROR,
        .result_handle = 0,
        .execution_time = 0.0f,
        .error_message = NULL
    };

    if (!context || put_link_handle == 0) {
        result.error_message = strdup("Invalid parameters");
        return result;
    }

    clock_t start = clock();

    // Placeholder implementation
    // Full implementation would:
    // 1. Extract template and bindings from PutLink
    // 2. Substitute all variables in template with bound values
    // 3. Return instantiated template
    
    result.status = COGLLAMA_EXEC_SUCCESS;
    result.result_handle = put_link_handle;  // Placeholder

    clock_t end = clock();
    result.execution_time = (float)(end - start) / CLOCKS_PER_SEC;

    return result;
}

/**
 * cogllama_procedure_register - Register built-in procedure
 */
int cogllama_procedure_register(
    struct cogllama_expression_context *context,
    const char *name,
    cogllama_procedure_fn procedure
) {
    if (!context || !name || !procedure) {
        return -1;
    }

    struct cogllama_expression_context_internal *internal =
        (struct cogllama_expression_context_internal *)context;

    struct procedure_entry *entry =
        (struct procedure_entry *)malloc(sizeof(struct procedure_entry));
    if (!entry) {
        return -1;
    }

    entry->name = strdup(name);
    if (!entry->name) {
        free(entry);
        return -1;
    }

    entry->procedure = procedure;
    entry->next = internal->procedures;
    internal->procedures = entry;

    return 0;
}

/**
 * cogllama_variable_binding_create - Create variable binding
 */
struct cogllama_variable_binding *cogllama_variable_binding_create(
    const char *variable_name,
    uint64_t atom_handle
) {
    if (!variable_name || atom_handle == 0) {
        return NULL;
    }

    struct cogllama_variable_binding *binding =
        (struct cogllama_variable_binding *)malloc(
            sizeof(struct cogllama_variable_binding));
    
    if (!binding) {
        return NULL;
    }

    binding->variable_name = strdup(variable_name);
    if (!binding->variable_name) {
        free(binding);
        return NULL;
    }

    binding->atom_handle = atom_handle;

    return binding;
}

/**
 * cogllama_variable_binding_free - Free variable binding
 */
void cogllama_variable_binding_free(
    struct cogllama_variable_binding *binding
) {
    if (!binding) {
        return;
    }

    free(binding->variable_name);
    free(binding);
}

/**
 * cogllama_query_result_free - Free query result
 */
void cogllama_query_result_free(
    struct cogllama_query_result *result
) {
    if (!result || !result->bindings) {
        return;
    }

    for (size_t i = 0; i < result->n_results; i++) {
        if (result->bindings[i]) {
            for (size_t j = 0; j < result->n_bindings_per_result; j++) {
                cogllama_variable_binding_free(&result->bindings[i][j]);
            }
            free(result->bindings[i]);
        }
    }

    free(result->bindings);
    result->bindings = NULL;
    result->n_results = 0;
}

/**
 * cogllama_execution_result_free - Free execution result
 */
void cogllama_execution_result_free(
    struct cogllama_execution_result *result
) {
    if (!result) {
        return;
    }

    if (result->error_message) {
        free(result->error_message);
        result->error_message = NULL;
    }
}

/**
 * cogllama_expression_eval - Generic expression evaluation
 */
struct cogllama_execution_result cogllama_expression_eval(
    struct cogllama_expression_context *context,
    uint64_t expr_handle
) {
    struct cogllama_execution_result result = {
        .status = COGLLAMA_EXEC_ERROR,
        .result_handle = 0,
        .execution_time = 0.0f,
        .error_message = NULL
    };

    if (!context || expr_handle == 0) {
        result.error_message = strdup("Invalid parameters");
        return result;
    }

    // In a full implementation, this would:
    // 1. Determine the expression type
    // 2. Dispatch to appropriate evaluation function
    // For now, return a generic success
    
    result.status = COGLLAMA_EXEC_SUCCESS;
    result.result_handle = expr_handle;

    return result;
}
