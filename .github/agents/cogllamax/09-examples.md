# CogLlama Examples

## Example 1: Basic Cognitive Cycle

```c
#include <cogllama/cogllama.h>

int main() {
    // Initialize system
    struct cogllama_system_params params = {
        .atomspace_size = 1000000,
        .attention_budget = 100.0f,
        .n_threads = 8,
    };
    
    struct cogllama_system *system = cogllama_system_init(&params);
    
    // Add some knowledge
    uint64_t cat = cogllama_atom_alloc(
        system->atomspace,
        COGLLAMA_NODE,
        "cat",
        NULL
    );
    
    uint64_t animal = cogllama_atom_alloc(
        system->atomspace,
        COGLLAMA_NODE,
        "animal",
        NULL
    );
    
    // Create inheritance link
    uint64_t outgoing[] = {cat, animal};
    cogllama_link_create(
        system->atomspace,
        COGLLAMA_INHERITANCE_LINK,
        outgoing,
        2,
        (float[]){0.9f, 0.8f}  // [strength, confidence]
    );
    
    // Run cognitive cycle
    for (int i = 0; i < 100; i++) {
        cogllama_cognitive_cycle(system->cogserver, 0.1f);
    }
    
    // Cleanup
    cogllama_system_free(system);
    
    return 0;
}
```

## Example 2: Perception and Reasoning

```c
#include <cogllama/cogllama.h>

int main() {
    struct cogllama_system *system = cogllama_system_init(NULL);
    
    // Load image
    uint8_t *image_data = load_image("cat.jpg");
    
    // Perceive image
    struct ggml_tensor *perception = cogllama_vision_perceive(
        system->perception,
        image_data,
        640, 480, 3
    );
    
    // Convert to atoms
    cogllama_perception_to_atoms(
        system->atomspace,
        perception,
        COGLLAMA_VISION
    );
    
    // Reason about perception
    struct cogllama_pattern pattern = {
        .type = COGLLAMA_SIMILARITY_PATTERN,
        .threshold = 0.8f
    };
    
    cogllama_pattern_match(
        system->atomspace,
        &pattern,
        match_callback,
        NULL
    );
    
    cogllama_system_free(system);
    return 0;
}
```

## Example 3: Learning and Evolution

```c
#include <cogllama/cogllama.h>

// Fitness function for program evolution
float program_fitness(struct ggml_tensor *program, void *data) {
    // Evaluate program on test data
    // Return fitness score
    return 0.85f;
}

int main() {
    struct cogllama_system *system = cogllama_system_init(NULL);
    
    // Configure MOSES
    system->moses->population_size = 100;
    system->moses->mutation_rate = 0.1f;
    system->moses->crossover_rate = 0.7f;
    
    // Evolve programs
    struct cogllama_program best_program;
    cogllama_moses_evolve(
        system->moses,
        program_fitness,
        50,  // 50 generations
        &best_program
    );
    
    // Use best program
    printf("Best fitness: %f\n", best_program.fitness);
    
    cogllama_system_free(system);
    return 0;
}
```

## Advanced Examples

### Quantization Support

```c
// Load quantized model for language components
struct llama_model_params model_params = {
    .n_gpu_layers = 32,
    .quantization = LLAMA_QUANT_Q4_K_M,
};

struct llama_model *llm = llama_load_model_from_file(
    "model.gguf",
    model_params
);

system->language_learner->llm = llm;
```

### Distributed Cognition

```c
// Multi-agent cognitive system
struct cogllama_system *agents[3];

for (int i = 0; i < 3; i++) {
    agents[i] = cogllama_system_init(NULL);
}

// Share AtomSpace across agents
for (int i = 0; i < 3; i++) {
    cogllama_communication_connect(
        agents[i]->communication,
        agents
    );
}

// Agents now share knowledge via communication
```

### Custom Tensor Operations

```c
// Define custom cognitive operation
struct ggml_tensor *custom_reasoning_op(
    struct ggml_context *ctx,
    struct ggml_tensor *input
) {
    // Custom tensor manipulation
    struct ggml_tensor *output = ggml_mul_mat(ctx, W, input);
    output = ggml_relu(ctx, output);
    return output;
}

// Register with system
cogllama_register_custom_op(
    system,
    "custom_reasoning",
    custom_reasoning_op
);
```
