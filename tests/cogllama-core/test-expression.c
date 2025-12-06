/**
 * @file test-expression.c
 * @brief Test suite for CogLlama expressions
 */

#include "expression.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

// Test utilities
#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            fprintf(stderr, "ASSERTION FAILED: %s at %s:%d\n", \
                    message, __FILE__, __LINE__); \
            return 0; \
        } \
    } while(0)

#define TEST_ASSERT_EQUAL(a, b, message) \
    TEST_ASSERT((a) == (b), message)

#define TEST_ASSERT_NOT_NULL(ptr, message) \
    TEST_ASSERT((ptr) != NULL, message)

// Test counter
static int tests_passed = 0;
static int tests_failed = 0;

/**
 * Test expression context initialization
 */
static int test_expression_context_init(void) {
    printf("Testing expression context initialization...\n");
    
    // Create atomspace
    struct cogllama_atomspace *atomspace = cogllama_atomspace_init(1000, 128);
    TEST_ASSERT_NOT_NULL(atomspace, "Failed to create atomspace");
    
    // Create expression context
    struct cogllama_expression_context *context = 
        cogllama_expression_context_init(atomspace, 100, 5.0f);
    TEST_ASSERT_NOT_NULL(context, "Failed to create expression context");
    TEST_ASSERT_EQUAL(context->atomspace, atomspace, "Atomspace not set correctly");
    TEST_ASSERT_EQUAL(context->max_recursion_depth, 100, "Max recursion depth not set");
    TEST_ASSERT(fabsf(context->timeout_seconds - 5.0f) < 0.001f, "Timeout not set correctly");
    TEST_ASSERT_EQUAL(context->current_depth, 0, "Initial depth should be 0");
    
    // Cleanup
    cogllama_expression_context_free(context);
    cogllama_atomspace_free(atomspace);
    
    printf("  ✓ Expression context initialization test passed\n");
    return 1;
}

/**
 * Test ExecutionLink creation
 */
static int test_execution_link_create(void) {
    printf("Testing ExecutionLink creation...\n");
    
    struct cogllama_atomspace *atomspace = cogllama_atomspace_init(1000, 128);
    TEST_ASSERT_NOT_NULL(atomspace, "Failed to create atomspace");
    
    // Create procedure atom
    float proc_embedding[128] = {0};
    proc_embedding[0] = 1.0f;
    uint64_t proc_handle = cogllama_atom_alloc(
        atomspace,
        COGLLAMA_ATOM_SCHEMA_NODE,
        "test_procedure",
        proc_embedding
    );
    TEST_ASSERT(proc_handle > 0, "Failed to create procedure atom");
    
    // Create argument atoms
    float arg1_embedding[128] = {0};
    arg1_embedding[0] = 2.0f;
    uint64_t arg1_handle = cogllama_atom_alloc(
        atomspace,
        COGLLAMA_ATOM_NUMBER_NODE,
        "arg1",
        arg1_embedding
    );
    TEST_ASSERT(arg1_handle > 0, "Failed to create arg1 atom");
    
    float arg2_embedding[128] = {0};
    arg2_embedding[0] = 3.0f;
    uint64_t arg2_handle = cogllama_atom_alloc(
        atomspace,
        COGLLAMA_ATOM_NUMBER_NODE,
        "arg2",
        arg2_embedding
    );
    TEST_ASSERT(arg2_handle > 0, "Failed to create arg2 atom");
    
    // Create ExecutionLink
    uint64_t args[2] = {arg1_handle, arg2_handle};
    uint64_t exec_link = cogllama_execution_link_create(
        atomspace,
        proc_handle,
        args,
        2
    );
    TEST_ASSERT(exec_link > 0, "Failed to create ExecutionLink");
    
    // Cleanup
    cogllama_atomspace_free(atomspace);
    
    printf("  ✓ ExecutionLink creation test passed\n");
    return 1;
}

/**
 * Test ExecutionLink evaluation
 */
static int test_execution_link_eval(void) {
    printf("Testing ExecutionLink evaluation...\n");
    
    struct cogllama_atomspace *atomspace = cogllama_atomspace_init(1000, 128);
    TEST_ASSERT_NOT_NULL(atomspace, "Failed to create atomspace");
    
    struct cogllama_expression_context *context = 
        cogllama_expression_context_init(atomspace, 100, 5.0f);
    TEST_ASSERT_NOT_NULL(context, "Failed to create expression context");
    
    // Create simple execution link
    float proc_embedding[128] = {0};
    uint64_t proc_handle = cogllama_atom_alloc(
        atomspace,
        COGLLAMA_ATOM_SCHEMA_NODE,
        "add",
        proc_embedding
    );
    
    uint64_t exec_link = cogllama_execution_link_create(
        atomspace,
        proc_handle,
        NULL,
        0
    );
    TEST_ASSERT(exec_link > 0, "Failed to create ExecutionLink");
    
    // Evaluate
    struct cogllama_execution_result result = 
        cogllama_execution_link_eval(context, exec_link);
    TEST_ASSERT_EQUAL(result.status, COGLLAMA_EXEC_SUCCESS, "Execution should succeed");
    TEST_ASSERT(result.result_handle > 0, "Result handle should be valid");
    
    // Cleanup
    cogllama_execution_result_free(&result);
    cogllama_expression_context_free(context);
    cogllama_atomspace_free(atomspace);
    
    printf("  ✓ ExecutionLink evaluation test passed\n");
    return 1;
}

/**
 * Test LambdaLink creation
 */
static int test_lambda_link_create(void) {
    printf("Testing LambdaLink creation...\n");
    
    struct cogllama_atomspace *atomspace = cogllama_atomspace_init(1000, 128);
    TEST_ASSERT_NOT_NULL(atomspace, "Failed to create atomspace");
    
    // Create variable atoms
    float var_embedding[128] = {0};
    uint64_t var_x = cogllama_atom_alloc(
        atomspace,
        COGLLAMA_ATOM_VARIABLE_NODE,
        "$x",
        var_embedding
    );
    TEST_ASSERT(var_x > 0, "Failed to create variable $x");
    
    uint64_t var_y = cogllama_atom_alloc(
        atomspace,
        COGLLAMA_ATOM_VARIABLE_NODE,
        "$y",
        var_embedding
    );
    TEST_ASSERT(var_y > 0, "Failed to create variable $y");
    
    // Create body atom
    float body_embedding[128] = {0};
    uint64_t body = cogllama_atom_alloc(
        atomspace,
        COGLLAMA_ATOM_CONCEPT_NODE,
        "body_expr",
        body_embedding
    );
    TEST_ASSERT(body > 0, "Failed to create body");
    
    // Create LambdaLink
    uint64_t variables[2] = {var_x, var_y};
    uint64_t lambda_link = cogllama_lambda_link_create(
        atomspace,
        variables,
        2,
        body
    );
    TEST_ASSERT(lambda_link > 0, "Failed to create LambdaLink");
    
    // Cleanup
    cogllama_atomspace_free(atomspace);
    
    printf("  ✓ LambdaLink creation test passed\n");
    return 1;
}

/**
 * Test LambdaLink application
 */
static int test_lambda_link_apply(void) {
    printf("Testing LambdaLink application...\n");
    
    struct cogllama_atomspace *atomspace = cogllama_atomspace_init(1000, 128);
    TEST_ASSERT_NOT_NULL(atomspace, "Failed to create atomspace");
    
    struct cogllama_expression_context *context = 
        cogllama_expression_context_init(atomspace, 100, 5.0f);
    TEST_ASSERT_NOT_NULL(context, "Failed to create expression context");
    
    // Create lambda
    float embedding[128] = {0};
    uint64_t var_x = cogllama_atom_alloc(
        atomspace,
        COGLLAMA_ATOM_VARIABLE_NODE,
        "$x",
        embedding
    );
    uint64_t body = cogllama_atom_alloc(
        atomspace,
        COGLLAMA_ATOM_CONCEPT_NODE,
        "body",
        embedding
    );
    
    uint64_t lambda_link = cogllama_lambda_link_create(
        atomspace,
        &var_x,
        1,
        body
    );
    TEST_ASSERT(lambda_link > 0, "Failed to create LambdaLink");
    
    // Create argument
    uint64_t arg = cogllama_atom_alloc(
        atomspace,
        COGLLAMA_ATOM_NUMBER_NODE,
        "42",
        embedding
    );
    
    // Apply lambda
    struct cogllama_execution_result result = 
        cogllama_lambda_link_apply(context, lambda_link, &arg, 1);
    TEST_ASSERT_EQUAL(result.status, COGLLAMA_EXEC_SUCCESS, "Lambda application should succeed");
    
    // Cleanup
    cogllama_execution_result_free(&result);
    cogllama_expression_context_free(context);
    cogllama_atomspace_free(atomspace);
    
    printf("  ✓ LambdaLink application test passed\n");
    return 1;
}

/**
 * Test GetLink creation
 */
static int test_get_link_create(void) {
    printf("Testing GetLink creation...\n");
    
    struct cogllama_atomspace *atomspace = cogllama_atomspace_init(1000, 128);
    TEST_ASSERT_NOT_NULL(atomspace, "Failed to create atomspace");
    
    // Create pattern atom
    float embedding[128] = {0};
    uint64_t pattern = cogllama_atom_alloc(
        atomspace,
        COGLLAMA_ATOM_CONCEPT_NODE,
        "pattern",
        embedding
    );
    TEST_ASSERT(pattern > 0, "Failed to create pattern");
    
    // Create variable
    uint64_t var = cogllama_atom_alloc(
        atomspace,
        COGLLAMA_ATOM_VARIABLE_NODE,
        "$var",
        embedding
    );
    TEST_ASSERT(var > 0, "Failed to create variable");
    
    // Create GetLink
    uint64_t get_link = cogllama_get_link_create(
        atomspace,
        pattern,
        &var,
        1
    );
    TEST_ASSERT(get_link > 0, "Failed to create GetLink");
    
    // Cleanup
    cogllama_atomspace_free(atomspace);
    
    printf("  ✓ GetLink creation test passed\n");
    return 1;
}

/**
 * Test PutLink creation
 */
static int test_put_link_create(void) {
    printf("Testing PutLink creation...\n");
    
    struct cogllama_atomspace *atomspace = cogllama_atomspace_init(1000, 128);
    TEST_ASSERT_NOT_NULL(atomspace, "Failed to create atomspace");
    
    // Create template
    float embedding[128] = {0};
    uint64_t template = cogllama_atom_alloc(
        atomspace,
        COGLLAMA_ATOM_CONCEPT_NODE,
        "template",
        embedding
    );
    TEST_ASSERT(template > 0, "Failed to create template");
    
    // Create binding
    uint64_t value = cogllama_atom_alloc(
        atomspace,
        COGLLAMA_ATOM_NUMBER_NODE,
        "42",
        embedding
    );
    
    struct cogllama_variable_binding *binding = 
        cogllama_variable_binding_create("$x", value);
    TEST_ASSERT_NOT_NULL(binding, "Failed to create binding");
    
    // Create PutLink
    uint64_t put_link = cogllama_put_link_create(
        atomspace,
        template,
        binding,
        1
    );
    TEST_ASSERT(put_link > 0, "Failed to create PutLink");
    
    // Cleanup
    cogllama_variable_binding_free(binding);
    cogllama_atomspace_free(atomspace);
    
    printf("  ✓ PutLink creation test passed\n");
    return 1;
}

/**
 * Test PutLink evaluation
 */
static int test_put_link_eval(void) {
    printf("Testing PutLink evaluation...\n");
    
    struct cogllama_atomspace *atomspace = cogllama_atomspace_init(1000, 128);
    TEST_ASSERT_NOT_NULL(atomspace, "Failed to create atomspace");
    
    struct cogllama_expression_context *context = 
        cogllama_expression_context_init(atomspace, 100, 5.0f);
    TEST_ASSERT_NOT_NULL(context, "Failed to create expression context");
    
    // Create PutLink
    float embedding[128] = {0};
    uint64_t template = cogllama_atom_alloc(
        atomspace,
        COGLLAMA_ATOM_CONCEPT_NODE,
        "template",
        embedding
    );
    
    uint64_t value = cogllama_atom_alloc(
        atomspace,
        COGLLAMA_ATOM_NUMBER_NODE,
        "42",
        embedding
    );
    
    struct cogllama_variable_binding *binding = 
        cogllama_variable_binding_create("$x", value);
    
    uint64_t put_link = cogllama_put_link_create(
        atomspace,
        template,
        binding,
        1
    );
    TEST_ASSERT(put_link > 0, "Failed to create PutLink");
    
    // Evaluate
    struct cogllama_execution_result result = 
        cogllama_put_link_eval(context, put_link);
    TEST_ASSERT_EQUAL(result.status, COGLLAMA_EXEC_SUCCESS, "PutLink evaluation should succeed");
    
    // Cleanup
    cogllama_execution_result_free(&result);
    cogllama_variable_binding_free(binding);
    cogllama_expression_context_free(context);
    cogllama_atomspace_free(atomspace);
    
    printf("  ✓ PutLink evaluation test passed\n");
    return 1;
}

/**
 * Test procedure registration
 */
static int test_procedure_registration(void) {
    printf("Testing procedure registration...\n");
    
    struct cogllama_atomspace *atomspace = cogllama_atomspace_init(1000, 128);
    TEST_ASSERT_NOT_NULL(atomspace, "Failed to create atomspace");
    
    struct cogllama_expression_context *context = 
        cogllama_expression_context_init(atomspace, 100, 5.0f);
    TEST_ASSERT_NOT_NULL(context, "Failed to create expression context");
    
    // Define a simple procedure
    struct cogllama_execution_result test_proc(
        struct cogllama_expression_context *ctx,
        const uint64_t *args,
        size_t n_args
    ) {
        struct cogllama_execution_result result = {
            .status = COGLLAMA_EXEC_SUCCESS,
            .result_handle = (n_args > 0) ? args[0] : 0,
            .execution_time = 0.0f,
            .error_message = NULL
        };
        return result;
    }
    
    // Register procedure
    int ret = cogllama_procedure_register(context, "test_proc", test_proc);
    TEST_ASSERT_EQUAL(ret, 0, "Procedure registration should succeed");
    
    // Cleanup
    cogllama_expression_context_free(context);
    cogllama_atomspace_free(atomspace);
    
    printf("  ✓ Procedure registration test passed\n");
    return 1;
}

/**
 * Test generic expression evaluation
 */
static int test_expression_eval(void) {
    printf("Testing generic expression evaluation...\n");
    
    struct cogllama_atomspace *atomspace = cogllama_atomspace_init(1000, 128);
    TEST_ASSERT_NOT_NULL(atomspace, "Failed to create atomspace");
    
    struct cogllama_expression_context *context = 
        cogllama_expression_context_init(atomspace, 100, 5.0f);
    TEST_ASSERT_NOT_NULL(context, "Failed to create expression context");
    
    // Create a simple expression
    float embedding[128] = {0};
    uint64_t expr = cogllama_atom_alloc(
        atomspace,
        COGLLAMA_ATOM_CONCEPT_NODE,
        "test_expr",
        embedding
    );
    
    // Evaluate
    struct cogllama_execution_result result = 
        cogllama_expression_eval(context, expr);
    TEST_ASSERT_EQUAL(result.status, COGLLAMA_EXEC_SUCCESS, "Generic evaluation should succeed");
    
    // Cleanup
    cogllama_execution_result_free(&result);
    cogllama_expression_context_free(context);
    cogllama_atomspace_free(atomspace);
    
    printf("  ✓ Generic expression evaluation test passed\n");
    return 1;
}

/**
 * Run all tests
 */
int main(void) {
    printf("=== CogLlama Expression Tests ===\n\n");
    
    // Run tests
    tests_passed += test_expression_context_init();
    tests_passed += test_execution_link_create();
    tests_passed += test_execution_link_eval();
    tests_passed += test_lambda_link_create();
    tests_passed += test_lambda_link_apply();
    tests_passed += test_get_link_create();
    tests_passed += test_put_link_create();
    tests_passed += test_put_link_eval();
    tests_passed += test_procedure_registration();
    tests_passed += test_expression_eval();
    
    // Print summary
    printf("\n=== Test Summary ===\n");
    printf("Passed: %d\n", tests_passed);
    printf("Failed: %d\n", tests_failed);
    
    if (tests_failed == 0) {
        printf("\n✓ All tests passed!\n");
        return 0;
    } else {
        printf("\n✗ Some tests failed!\n");
        return 1;
    }
}
