#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <variant>

namespace opencog_qat {

/**
 * FFI Interface for Scheme/OCaml with OpenCoq bindings
 * 
 * Provides Foreign Function Interface (FFI) for interoperating with
 * Rocq Prover (formerly Coq) and supports both Scheme and OCaml bindings.
 */
class RocqProverBridge {
public:
    // Forward declarations
    struct TensorDescriptor;
    struct TheoremProof;
    struct SymbolicExpression;
    struct NumericTensor;
    
    // FFI Value types for cross-language marshalling
    using FFIValue = std::variant<
        int32_t,
        double,
        std::string,
        std::vector<double>,
        std::shared_ptr<TensorDescriptor>
    >;
    
    /**
     * Tensor Data Marshalling for Rocq Prover Interop
     */
    struct TensorDescriptor {
        std::vector<size_t> shape;
        std::string dtype;  // "float32", "float64", "int32", etc.
        std::vector<uint8_t> data;  // Raw tensor data
        std::string rocq_type;      // Corresponding Rocq type specification
        
        // Metadata for formal verification
        struct VerificationMetadata {
            std::string well_formed_predicate;  // e.g., "well_formed_tensor"
            std::vector<std::string> invariants; // Mathematical invariants
            std::string shape_theorem;           // Shape consistency theorem
        } verification;
        
        size_t totalElements() const {
            size_t total = 1;
            for (size_t dim : shape) total *= dim;
            return total;
        }
    };
    
    /**
     * Symbolic Expression Representation
     */
    struct SymbolicExpression {
        enum class Type {
            ATOM,
            LINK,
            VARIABLE,
            LAMBDA,
            APPLY
        };
        
        Type type;
        std::string name;
        std::vector<std::shared_ptr<SymbolicExpression>> children;
        std::unordered_map<std::string, FFIValue> attributes;
        
        // Rocq representation
        std::string rocq_term;
        std::string rocq_type;
    };
    
    /**
     * Theorem and Proof Representation
     */
    struct TheoremProof {
        std::string theorem_name;
        std::string theorem_statement;  // Rocq theorem statement
        std::string proof_term;        // Proof term/tactic script
        std::vector<std::string> dependencies; // Required lemmas/axioms
        bool verified;                 // Proof verification status
        
        // Metadata
        struct ProofMetadata {
            std::string complexity_class;  // Proof complexity
            double verification_time_ms;
            std::vector<std::string> tactics_used;
        } metadata;
    };
    
    /**
     * Attention Allocation Formal Specification
     */
    struct AttentionSpec {
        std::shared_ptr<TensorDescriptor> attention_tensor;
        std::string conservation_theorem;  // "sum_attention a = 1"
        std::string allocation_algorithm;  // Algorithm specification
        std::vector<std::string> safety_properties;
    };

public:
    RocqProverBridge();
    ~RocqProverBridge();
    
    /**
     * Initialize FFI interface and Rocq Prover connection
     */
    bool initialize(const std::string& rocq_path = "",
                   const std::string& scheme_interpreter = "guile",
                   const std::string& ocaml_runtime = "ocaml");
    
    /**
     * Tensor Data Marshalling Functions
     */
    
    // Convert C++ tensor to Rocq-compatible format
    std::shared_ptr<TensorDescriptor> marshalTensorToRocq(
        const std::vector<float>& data,
        const std::vector<size_t>& shape,
        const std::string& verification_spec = ""
    );
    
    // Convert Rocq tensor back to C++ format
    std::vector<float> unmarshalTensorFromRocq(
        const std::shared_ptr<TensorDescriptor>& tensor_desc
    );
    
    // Verify tensor well-formedness in Rocq
    bool verifyTensorWellFormedness(
        const std::shared_ptr<TensorDescriptor>& tensor
    );
    
    /**
     * Automated Theorem Generation from Hypergraph Inference
     */
    
    // Generate theorem from hypergraph inference result
    std::shared_ptr<TheoremProof> generateTheoremFromInference(
        const std::vector<std::shared_ptr<SymbolicExpression>>& premises,
        const std::shared_ptr<SymbolicExpression>& conclusion,
        const std::string& inference_rule
    );
    
    // Auto-generate proofs for neural-symbolic bridge properties
    std::vector<std::shared_ptr<TheoremProof>> generateBridgeProofs(
        const std::shared_ptr<TensorDescriptor>& neural_state,
        const std::vector<std::shared_ptr<SymbolicExpression>>& symbolic_atoms
    );
    
    /**
     * Type-Safe Symbolic-to-Numeric Conversion
     */
    
    // Convert symbolic expression to numeric tensor with type safety
    std::shared_ptr<TensorDescriptor> symbolicToNumeric(
        const std::shared_ptr<SymbolicExpression>& symbolic,
        const std::string& target_type = "float32"
    );
    
    // Convert numeric tensor to symbolic representation
    std::shared_ptr<SymbolicExpression> numericToSymbolic(
        const std::shared_ptr<TensorDescriptor>& numeric,
        const std::string& symbolic_domain = "real"
    );
    
    // Verify type-safe conversion properties
    bool verifyConversionCorrectness(
        const std::shared_ptr<SymbolicExpression>& symbolic,
        const std::shared_ptr<TensorDescriptor>& numeric
    );
    
    /**
     * OpenCoq Tactic Integration for Automated Proving
     */
    
    // Execute Rocq tactic on theorem
    bool executeTactic(
        const std::string& tactic_name,
        std::shared_ptr<TheoremProof>& theorem,
        const std::vector<std::string>& args = {}
    );
    
    // Auto-prove theorem using available tactics
    bool autoProve(
        std::shared_ptr<TheoremProof>& theorem,
        int timeout_seconds = 60
    );
    
    // Generate custom tactic from inference pattern
    std::string generateCustomTactic(
        const std::vector<std::shared_ptr<TheoremProof>>& example_proofs,
        const std::string& tactic_name
    );
    
    /**
     * Formal Verification of Attention Allocation Algorithms
     */
    
    // Verify attention conservation property
    std::shared_ptr<TheoremProof> verifyAttentionConservation(
        const std::shared_ptr<AttentionSpec>& attention_spec
    );
    
    // Verify attention allocation safety properties
    std::vector<std::shared_ptr<TheoremProof>> verifyAttentionSafety(
        const std::shared_ptr<AttentionSpec>& attention_spec
    );
    
    // Generate formal specification for attention mechanism
    std::shared_ptr<AttentionSpec> generateAttentionSpec(
        const std::shared_ptr<TensorDescriptor>& attention_tensor,
        const std::string& algorithm_name
    );
    
    /**
     * FFI Binding Functions
     */
    
    // Register Scheme bindings
    bool registerSchemeBindings(const std::string& module_name = "rocq-bridge");
    
    // Register OCaml bindings  
    bool registerOCamlBindings(const std::string& module_name = "rocq_bridge");
    
    // Execute Scheme code with FFI access
    FFIValue executeScheme(const std::string& scheme_code);
    
    // Execute OCaml code with FFI access
    FFIValue executeOCaml(const std::string& ocaml_code);
    
    /**
     * Query and Management Functions
     */
    
    // Get verification status of all theorems
    std::vector<std::shared_ptr<TheoremProof>> getAllVerifiedTheorems() const;
    
    // Get Rocq Prover status and health
    struct RocqStatus {
        bool connected;
        std::string version;
        double response_time_ms;
        size_t active_theorems;
        size_t verified_theorems;
    };
    
    RocqStatus getRocqStatus() const;
    
    // Shutdown and cleanup
    void shutdown();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper methods
    void registerCoreFunctions();
};

/**
 * Factory for creating RocqProverBridge instances
 */
class RocqBridgeFactory {
public:
    // Create bridge with default configuration
    static std::unique_ptr<RocqProverBridge> createDefault();
    
    // Create bridge with custom Rocq installation
    static std::unique_ptr<RocqProverBridge> createCustom(
        const std::string& rocq_path,
        const std::string& config_file = ""
    );
    
    // Create bridge for testing (mock implementation)
    static std::unique_ptr<RocqProverBridge> createForTesting();
};

} // namespace opencog_qat