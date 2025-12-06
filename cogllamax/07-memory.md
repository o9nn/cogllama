# Component Group 7: cogllama-memory

**Memory and context management**

## Components
- **atomspace-rocks**: RocksDB persistence backend
- **attention**: Economic Attention Networks (ECAN)

## GGML Implementation

### AtomSpace Persistence

```c
/**
 * cogllama_storage - Persistent AtomSpace storage
 * 
 * Manages persistent storage of AtomSpace using RocksDB
 * with tensor serialization.
 */
struct cogllama_storage {
    void *db_handle;                     // RocksDB handle
    struct cogllama_atomspace *atomspace;
    char *db_path;
};

/**
 * cogllama_storage_save - Save AtomSpace to disk
 * @storage: Storage backend
 * 
 * Serializes entire AtomSpace to persistent storage.
 * Efficient tensor serialization with compression.
 */
int cogllama_storage_save(
    struct cogllama_storage *storage
);

/**
 * cogllama_storage_load - Load AtomSpace from disk
 * @storage: Storage backend
 * 
 * Deserializes AtomSpace from persistent storage.
 * Reconstructs tensor graphs from disk.
 */
int cogllama_storage_load(
    struct cogllama_storage *storage
);

/**
 * cogllama_storage_sync - Incremental synchronization
 * @storage: Storage backend
 * 
 * Synchronizes changes to disk incrementally.
 * Only writes modified atoms for efficiency.
 */
int cogllama_storage_sync(
    struct cogllama_storage *storage
);
```

### Attention Allocation (ECAN)

```c
/**
 * cogllama_attention - Economic Attention Networks
 * 
 * Implements ECAN attention allocation using tensor operations
 * for efficient parallel attention spreading.
 */
struct cogllama_attention {
    struct cogllama_atomspace *atomspace;
    struct ggml_tensor *sti_tensor;      // Short-term importance
    struct ggml_tensor *lti_tensor;      // Long-term importance
    struct ggml_tensor *vlti_tensor;     // Very long-term importance
    float attention_budget;
};

/**
 * cogllama_attention_update - Update attention values
 * @attention: Attention system
 * @delta_time: Time since last update
 * 
 * Updates attention values using spreading activation.
 * Implements economic attention allocation as tensor ops.
 */
int cogllama_attention_update(
    struct cogllama_attention *attention,
    float delta_time
);

/**
 * cogllama_attention_focus - Get focal atoms
 * @attention: Attention system
 * @top_k: Number of atoms to return
 * @focused_atoms: Output array of focused atoms
 * 
 * Returns atoms with highest attention values.
 * Efficient tensor sorting for attention selection.
 */
int cogllama_attention_focus(
    struct cogllama_attention *attention,
    size_t top_k,
    uint64_t *focused_atoms
);

/**
 * cogllama_attention_spread - Spread activation
 * @attention: Attention system
 * @source_atom: Source of activation
 * @intensity: Activation intensity
 * 
 * Spreads activation from source atom to neighbors.
 * Tensor-parallel activation spreading.
 */
int cogllama_attention_spread(
    struct cogllama_attention *attention,
    uint64_t source_atom,
    float intensity
);
```
