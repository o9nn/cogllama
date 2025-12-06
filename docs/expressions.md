# CogLlama Expressions

This document describes the CogLlama expressions implementation, which provides executable atoms based on OpenCog's expression system.

## Overview

Expressions in CogLlama are atoms that can be evaluated to produce results. They enable:
- **Procedural reasoning**: Execute procedures and actions
- **Lambda functions**: Anonymous functions with closures
- **Pattern matching**: Query the knowledge base
- **Variable substitution**: Instantiate templates with values

## Expression Types

### 1. ExecutionLink

Executes a procedure with arguments.

```c
// Create a procedure atom
float proc_embedding[128] = {0};
uint64_t proc_handle = cogllama_atom_alloc(
    atomspace,
    COGLLAMA_ATOM_SCHEMA_NODE,
    "add",
    proc_embedding
);

// Create argument atoms
uint64_t arg1 = cogllama_atom_alloc(atomspace, COGLLAMA_ATOM_NUMBER_NODE, "5", embedding);
uint64_t arg2 = cogllama_atom_alloc(atomspace, COGLLAMA_ATOM_NUMBER_NODE, "7", embedding);

// Create ExecutionLink
uint64_t args[] = {arg1, arg2};
uint64_t exec_link = cogllama_execution_link_create(
    atomspace,
    proc_handle,
    args,
    2
);

// Evaluate ExecutionLink
struct cogllama_expression_context *context = 
    cogllama_expression_context_init(atomspace, 100, 5.0f);
    
struct cogllama_execution_result result = 
    cogllama_execution_link_eval(context, exec_link);

if (result.status == COGLLAMA_EXEC_SUCCESS) {
    printf("Result: %llu\n", result.result_handle);
}

cogllama_execution_result_free(&result);
```

### 2. LambdaLink

Represents an anonymous function with variables and a body.

```c
// Create variables
uint64_t var_x = cogllama_atom_alloc(
    atomspace,
    COGLLAMA_ATOM_VARIABLE_NODE,
    "$x",
    embedding
);

uint64_t var_y = cogllama_atom_alloc(
    atomspace,
    COGLLAMA_ATOM_VARIABLE_NODE,
    "$y",
    embedding
);

// Create body expression
uint64_t body = cogllama_atom_alloc(
    atomspace,
    COGLLAMA_ATOM_CONCEPT_NODE,
    "add_expr",
    embedding
);

// Create LambdaLink
uint64_t variables[] = {var_x, var_y};
uint64_t lambda = cogllama_lambda_link_create(
    atomspace,
    variables,
    2,
    body
);

// Apply lambda to arguments
uint64_t arg1 = cogllama_atom_alloc(atomspace, COGLLAMA_ATOM_NUMBER_NODE, "3", embedding);
uint64_t arg2 = cogllama_atom_alloc(atomspace, COGLLAMA_ATOM_NUMBER_NODE, "4", embedding);
uint64_t args[] = {arg1, arg2};

struct cogllama_execution_result result = 
    cogllama_lambda_link_apply(context, lambda, args, 2);
```

### 3. GetLink

Performs pattern matching queries on the AtomSpace.

```c
// Create pattern with variables
uint64_t pattern = cogllama_atom_alloc(
    atomspace,
    COGLLAMA_ATOM_CONCEPT_NODE,
    "InheritanceLink($x, Animal)",
    embedding
);

// Create variable to bind
uint64_t var = cogllama_atom_alloc(
    atomspace,
    COGLLAMA_ATOM_VARIABLE_NODE,
    "$x",
    embedding
);

// Create GetLink
uint64_t get_link = cogllama_get_link_create(
    atomspace,
    pattern,
    &var,
    1
);

// Evaluate GetLink to get all matches
struct cogllama_query_result result = 
    cogllama_get_link_eval(context, get_link);

printf("Found %zu matches\n", result.n_results);

// Process matches
for (size_t i = 0; i < result.n_results; i++) {
    for (size_t j = 0; j < result.n_bindings_per_result; j++) {
        printf("  %s -> %llu\n", 
            result.bindings[i][j].variable_name,
            result.bindings[i][j].atom_handle);
    }
}

cogllama_query_result_free(&result);
```

### 4. PutLink

Substitutes variables in a template with specific values.

```c
// Create template with variable
uint64_t template = cogllama_atom_alloc(
    atomspace,
    COGLLAMA_ATOM_CONCEPT_NODE,
    "InheritanceLink($x, Animal)",
    embedding
);

// Create value to substitute
uint64_t value = cogllama_atom_alloc(
    atomspace,
    COGLLAMA_ATOM_CONCEPT_NODE,
    "Dog",
    embedding
);

// Create variable binding
struct cogllama_variable_binding *binding = 
    cogllama_variable_binding_create("$x", value);

// Create PutLink
uint64_t put_link = cogllama_put_link_create(
    atomspace,
    template,
    binding,
    1
);

// Evaluate PutLink to instantiate template
struct cogllama_execution_result result = 
    cogllama_put_link_eval(context, put_link);

printf("Instantiated: %llu\n", result.result_handle);

cogllama_execution_result_free(&result);
cogllama_variable_binding_free(binding);
```

## Expression Context

The expression context manages evaluation parameters:

```c
struct cogllama_expression_context *context = 
    cogllama_expression_context_init(
        atomspace,
        100,    // max_recursion_depth (0 = unlimited)
        5.0f    // timeout_seconds (0.0 = no timeout)
    );

// Use context for evaluations...

cogllama_expression_context_free(context);
```

### Context Parameters

- **max_recursion_depth**: Prevents infinite recursion (0 for unlimited)
- **timeout_seconds**: Execution timeout in seconds (0.0 for no timeout)
- **current_depth**: Tracks current recursion depth (automatically managed)

## Built-in Procedures

Register custom procedures that can be called via ExecutionLink:

```c
// Define procedure implementation
struct cogllama_execution_result add_proc(
    struct cogllama_expression_context *context,
    const uint64_t *args,
    size_t n_args
) {
    struct cogllama_execution_result result = {
        .status = COGLLAMA_EXEC_SUCCESS,
        .result_handle = 0,
        .execution_time = 0.0f,
        .error_message = NULL
    };
    
    // Implement addition logic here
    // ...
    
    return result;
}

// Register procedure
cogllama_procedure_register(context, "add", add_proc);

// Now "add" can be called via ExecutionLink
```

## Generic Expression Evaluation

Evaluate any expression type automatically:

```c
// Automatically detects expression type and evaluates
struct cogllama_execution_result result = 
    cogllama_expression_eval(context, expr_handle);

if (result.status == COGLLAMA_EXEC_SUCCESS) {
    printf("Success! Result: %llu\n", result.result_handle);
} else if (result.status == COGLLAMA_EXEC_TIMEOUT) {
    printf("Execution timeout\n");
} else if (result.error_message) {
    printf("Error: %s\n", result.error_message);
}

cogllama_execution_result_free(&result);
```

## Execution Status

All evaluations return a status code:

- **COGLLAMA_EXEC_SUCCESS**: Execution completed successfully
- **COGLLAMA_EXEC_ERROR**: General error occurred
- **COGLLAMA_EXEC_TIMEOUT**: Execution exceeded timeout
- **COGLLAMA_EXEC_INVALID_ARGS**: Invalid arguments provided

## Memory Management

Always free resources when done:

```c
// Free execution results
cogllama_execution_result_free(&result);

// Free query results
cogllama_query_result_free(&query_result);

// Free variable bindings
cogllama_variable_binding_free(binding);

// Free expression context
cogllama_expression_context_free(context);

// Free atomspace
cogllama_atomspace_free(atomspace);
```

## Integration with GGML

Expressions are integrated with GGML tensors:
- Expression evaluation can leverage GPU acceleration
- Pattern matching uses tensor operations for efficiency
- Lambda closures can capture tensor data
- Procedure results can be tensor atoms

## Testing

Run the expression tests:

```bash
cd build
./bin/test-expression
```

All tests should pass:
- Expression context initialization
- ExecutionLink creation and evaluation
- LambdaLink creation and application
- GetLink creation and evaluation
- PutLink creation and evaluation
- Procedure registration
- Generic expression evaluation

## Future Enhancements

Planned improvements:
1. **Full pattern matching**: Complete GPU-accelerated pattern matcher
2. **Lazy evaluation**: On-demand evaluation with memoization
3. **Parallel execution**: Concurrent evaluation of independent expressions
4. **Type checking**: Static type analysis for expressions
5. **Optimization**: Expression simplification and optimization
6. **Debugging**: Expression tracing and debugging support

## References

- [OpenCog AtomSpace](https://wiki.opencog.org/w/AtomSpace)
- [OpenCog Expressions](https://wiki.opencog.org/w/Execution)
- [Lambda Calculus in OpenCog](https://wiki.opencog.org/w/Lambda)
- [Pattern Matching](https://wiki.opencog.org/w/Pattern_matching)

## API Reference

See `expression.h` for complete API documentation.

## Examples

See `tests/cogllama-core/test-expression.c` for complete working examples.
