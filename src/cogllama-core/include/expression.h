/**
 * @file expression.h
 * @brief CogLlama expressions implementation for OpenCog
 * 
 * This implements OpenCog's expression system including:
 * - ExecutionLink: For executing procedures and actions
 * - LambdaLink: For lambda expressions and anonymous functions
 * - GetLink: For pattern matching queries
 * - PutLink: For variable substitution and instantiation
 * 
 * Expressions provide the executable component of the AtomSpace,
 * enabling procedural reasoning and dynamic computation.
 */

#ifndef COGLLAMA_EXPRESSION_H
#define COGLLAMA_EXPRESSION_H

#include "atomspace.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Expression types - different kinds of executable atoms
 */
enum cogllama_expression_type {
    COGLLAMA_EXPR_EXECUTION = 0,    // ExecutionLink - execute procedures
    COGLLAMA_EXPR_LAMBDA,           // LambdaLink - lambda expressions
    COGLLAMA_EXPR_GET,              // GetLink - pattern matching queries
    COGLLAMA_EXPR_PUT,              // PutLink - variable substitution
    COGLLAMA_EXPR_DEFINE,           // DefineLink - name definitions
    COGLLAMA_EXPR_QUOTE,            // QuoteLink - prevent evaluation
    COGLLAMA_EXPR_UNQUOTE,          // UnquoteLink - force evaluation
};

/**
 * Execution result status
 */
enum cogllama_exec_status {
    COGLLAMA_EXEC_SUCCESS = 0,
    COGLLAMA_EXEC_ERROR,
    COGLLAMA_EXEC_TIMEOUT,
    COGLLAMA_EXEC_INVALID_ARGS,
};

/**
 * Expression context - execution environment
 */
struct cogllama_expression_context {
    struct cogllama_atomspace *atomspace;
    struct ggml_context *ctx;
    size_t max_recursion_depth;
    size_t current_depth;
    float timeout_seconds;
    void *user_data;
};

/**
 * Variable binding for lambda and pattern matching
 */
struct cogllama_variable_binding {
    char *variable_name;
    uint64_t atom_handle;
};

/**
 * Execution result
 */
struct cogllama_execution_result {
    enum cogllama_exec_status status;
    uint64_t result_handle;      // Handle to result atom
    float execution_time;
    char *error_message;
};

/**
 * Lambda expression structure
 */
struct cogllama_lambda {
    uint64_t *variable_handles;   // Variable declarations
    size_t n_variables;
    uint64_t body_handle;         // Expression body
    struct cogllama_atomspace *closure_space; // Captured environment
};

/**
 * Pattern for GetLink queries
 */
struct cogllama_query_pattern {
    uint64_t pattern_handle;      // Pattern to match
    uint64_t *variable_handles;   // Variables to bind
    size_t n_variables;
    float min_confidence;         // Minimum truth confidence
};

/**
 * Query result set
 */
struct cogllama_query_result {
    struct cogllama_variable_binding **bindings;
    size_t n_results;
    size_t n_bindings_per_result;
};

/**
 * Procedure function pointer for built-in procedures
 */
typedef struct cogllama_execution_result (*cogllama_procedure_fn)(
    struct cogllama_expression_context *context,
    const uint64_t *args,
    size_t n_args
);

/**
 * cogllama_expression_context_init - Initialize expression context
 * @atomspace: AtomSpace for expression evaluation
 * @max_recursion_depth: Maximum recursion depth (0 for unlimited)
 * @timeout_seconds: Timeout for execution (0.0 for no timeout)
 * 
 * Creates a new expression execution context.
 * 
 * Returns: Pointer to initialized context, or NULL on failure
 */
struct cogllama_expression_context *cogllama_expression_context_init(
    struct cogllama_atomspace *atomspace,
    size_t max_recursion_depth,
    float timeout_seconds
);

/**
 * cogllama_expression_context_free - Free expression context
 * @context: Context to free
 */
void cogllama_expression_context_free(
    struct cogllama_expression_context *context
);

/**
 * cogllama_execution_link_create - Create ExecutionLink
 * @atomspace: AtomSpace context
 * @procedure_handle: Handle to procedure/schema to execute
 * @args: Array of argument atom handles
 * @n_args: Number of arguments
 * 
 * Creates an ExecutionLink that can be evaluated to execute a procedure.
 * 
 * Returns: Handle to ExecutionLink, or 0 on failure
 */
uint64_t cogllama_execution_link_create(
    struct cogllama_atomspace *atomspace,
    uint64_t procedure_handle,
    const uint64_t *args,
    size_t n_args
);

/**
 * cogllama_execution_link_eval - Evaluate ExecutionLink
 * @context: Expression context
 * @execution_link_handle: Handle to ExecutionLink
 * 
 * Executes the procedure specified in the ExecutionLink with its arguments.
 * 
 * Returns: Execution result with status and result handle
 */
struct cogllama_execution_result cogllama_execution_link_eval(
    struct cogllama_expression_context *context,
    uint64_t execution_link_handle
);

/**
 * cogllama_lambda_link_create - Create LambdaLink
 * @atomspace: AtomSpace context
 * @variable_handles: Array of variable atom handles
 * @n_variables: Number of variables
 * @body_handle: Handle to body expression
 * 
 * Creates a LambdaLink representing an anonymous function.
 * 
 * Returns: Handle to LambdaLink, or 0 on failure
 */
uint64_t cogllama_lambda_link_create(
    struct cogllama_atomspace *atomspace,
    const uint64_t *variable_handles,
    size_t n_variables,
    uint64_t body_handle
);

/**
 * cogllama_lambda_link_apply - Apply lambda to arguments
 * @context: Expression context
 * @lambda_handle: Handle to LambdaLink
 * @args: Array of argument atom handles
 * @n_args: Number of arguments (must match lambda parameters)
 * 
 * Applies a lambda expression to the given arguments.
 * Substitutes variables with arguments and evaluates body.
 * 
 * Returns: Execution result with status and result handle
 */
struct cogllama_execution_result cogllama_lambda_link_apply(
    struct cogllama_expression_context *context,
    uint64_t lambda_handle,
    const uint64_t *args,
    size_t n_args
);

/**
 * cogllama_get_link_create - Create GetLink
 * @atomspace: AtomSpace context
 * @pattern_handle: Handle to pattern to match
 * @variable_handles: Array of variable atom handles to bind
 * @n_variables: Number of variables
 * 
 * Creates a GetLink for pattern matching queries.
 * 
 * Returns: Handle to GetLink, or 0 on failure
 */
uint64_t cogllama_get_link_create(
    struct cogllama_atomspace *atomspace,
    uint64_t pattern_handle,
    const uint64_t *variable_handles,
    size_t n_variables
);

/**
 * cogllama_get_link_eval - Evaluate GetLink
 * @context: Expression context
 * @get_link_handle: Handle to GetLink
 * 
 * Executes pattern matching query and returns all matching bindings.
 * 
 * Returns: Query result set with all matches
 */
struct cogllama_query_result cogllama_get_link_eval(
    struct cogllama_expression_context *context,
    uint64_t get_link_handle
);

/**
 * cogllama_put_link_create - Create PutLink
 * @atomspace: AtomSpace context
 * @template_handle: Handle to template with variables
 * @bindings: Array of variable bindings
 * @n_bindings: Number of bindings
 * 
 * Creates a PutLink for variable substitution.
 * 
 * Returns: Handle to PutLink, or 0 on failure
 */
uint64_t cogllama_put_link_create(
    struct cogllama_atomspace *atomspace,
    uint64_t template_handle,
    const struct cogllama_variable_binding *bindings,
    size_t n_bindings
);

/**
 * cogllama_put_link_eval - Evaluate PutLink
 * @context: Expression context
 * @put_link_handle: Handle to PutLink
 * 
 * Performs variable substitution on the template.
 * Replaces all variables with their bound values.
 * 
 * Returns: Execution result with instantiated template
 */
struct cogllama_execution_result cogllama_put_link_eval(
    struct cogllama_expression_context *context,
    uint64_t put_link_handle
);

/**
 * cogllama_procedure_register - Register built-in procedure
 * @context: Expression context
 * @name: Procedure name
 * @procedure: Function pointer to procedure implementation
 * 
 * Registers a built-in procedure that can be called via ExecutionLink.
 * 
 * Returns: 0 on success, -1 on failure
 */
int cogllama_procedure_register(
    struct cogllama_expression_context *context,
    const char *name,
    cogllama_procedure_fn procedure
);

/**
 * cogllama_variable_binding_create - Create variable binding
 * @variable_name: Name of variable
 * @atom_handle: Handle to atom to bind
 * 
 * Creates a variable binding for use with PutLink.
 * 
 * Returns: Pointer to binding, or NULL on failure
 */
struct cogllama_variable_binding *cogllama_variable_binding_create(
    const char *variable_name,
    uint64_t atom_handle
);

/**
 * cogllama_variable_binding_free - Free variable binding
 * @binding: Binding to free
 */
void cogllama_variable_binding_free(
    struct cogllama_variable_binding *binding
);

/**
 * cogllama_query_result_free - Free query result
 * @result: Result to free
 */
void cogllama_query_result_free(
    struct cogllama_query_result *result
);

/**
 * cogllama_execution_result_free - Free execution result
 * @result: Result to free
 */
void cogllama_execution_result_free(
    struct cogllama_execution_result *result
);

/**
 * cogllama_expression_eval - Generic expression evaluation
 * @context: Expression context
 * @expr_handle: Handle to expression atom
 * 
 * Evaluates any expression type automatically detecting the type.
 * 
 * Returns: Execution result
 */
struct cogllama_execution_result cogllama_expression_eval(
    struct cogllama_expression_context *context,
    uint64_t expr_handle
);

#ifdef __cplusplus
}
#endif

#endif // COGLLAMA_EXPRESSION_H
