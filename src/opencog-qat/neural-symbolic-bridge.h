#pragma once

#include "rocq-prover-bridge.h"
#include "extended-atomspace.h"
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <chrono>
#include <limits>

namespace opencog_qat {

/**
 * Neural-Symbolic Tensor Bridge with Formal Verification
 * 
 * Provides type-safe conversion between neural tensor representations
 * and symbolic AtomSpace structures with formal verification guarantees.
 */
class NeuralSymbolicBridge {
public:
    // Forward declarations
    struct ConversionSpec;
    struct VerificationContext;
    struct BridgeMetrics;
    
    /**
     * Tensor-AtomSpace Conversion Specification
     */
    struct ConversionSpec {
        // Source and target specifications
        std::string source_format;     // "tensor" or "atomspace"
        std::string target_format;     // "tensor" or "atomspace" 
        std::string tensor_dtype;      // "float32", "float64", etc.
        std::string symbolic_domain;   // "real", "complex", "discrete", etc.
        
        // Conversion parameters
        struct Parameters {
            double precision_threshold = 1e-6;   // Numerical precision
            size_t max_symbolic_depth = 10;      // Maximum symbolic expression depth
            bool preserve_gradients = true;      // Whether to maintain differentiability
            bool enforce_type_safety = true;     // Enable type safety checks
        } params;
        
        // Formal verification requirements
        struct VerificationRequirements {
            bool verify_bijection = true;        // Verify encode/decode bijection
            bool verify_preservation = true;     // Verify semantic preservation
            bool verify_type_safety = true;      // Verify type correctness
            bool generate_proofs = true;         // Generate formal proofs
        } verification;
        
        // Performance constraints
        struct Constraints {
            size_t max_tensor_size = 1000000;    // Maximum tensor elements
            size_t max_atom_count = 100000;      // Maximum atoms in conversion
            double max_conversion_time_ms = 5000.0; // Time limit
        } constraints;
    };
    
    /**
     * Verification Context for Formal Proofs
     */
    struct VerificationContext {
        std::shared_ptr<RocqProverBridge> rocq_bridge;
        std::vector<std::shared_ptr<RocqProverBridge::TheoremProof>> active_theorems;
        std::unordered_map<std::string, std::string> type_mappings;
        
        // Verification state
        bool context_valid = false;
        std::string last_error;
        double total_verification_time_ms = 0.0;
    };
    
    /**
     * Bridge Performance and Accuracy Metrics
     */
    struct BridgeMetrics {
        // Conversion statistics
        size_t total_conversions = 0;
        size_t successful_conversions = 0;
        size_t failed_conversions = 0;
        
        // Performance metrics
        double avg_conversion_time_ms = 0.0;
        double max_conversion_time_ms = 0.0;
        double min_conversion_time_ms = std::numeric_limits<double>::max();
        
        // Accuracy metrics
        double avg_precision_loss = 0.0;
        double max_precision_loss = 0.0;
        size_t perfect_conversions = 0;  // Zero precision loss
        
        // Formal verification metrics
        size_t theorems_generated = 0;
        size_t theorems_verified = 0;
        double avg_proof_time_ms = 0.0;
        
        // Type safety metrics
        size_t type_errors_detected = 0;
        size_t type_errors_prevented = 0;
        
        void updateConversionMetrics(double time_ms, double precision_loss) {
            total_conversions++;
            successful_conversions++;
            
            // Update time metrics
            avg_conversion_time_ms = (avg_conversion_time_ms * (total_conversions - 1) + time_ms) / total_conversions;
            max_conversion_time_ms = std::max(max_conversion_time_ms, time_ms);
            min_conversion_time_ms = std::min(min_conversion_time_ms, time_ms);
            
            // Update precision metrics
            avg_precision_loss = (avg_precision_loss * (total_conversions - 1) + precision_loss) / total_conversions;
            max_precision_loss = std::max(max_precision_loss, precision_loss);
            
            if (precision_loss < 1e-12) {
                perfect_conversions++;
            }
        }
    };

public:
    NeuralSymbolicBridge();
    explicit NeuralSymbolicBridge(const ConversionSpec& spec);
    ~NeuralSymbolicBridge();
    
    /**
     * Initialize bridge with verification context
     */
    bool initialize(std::shared_ptr<RocqProverBridge> rocq_bridge,
                   std::shared_ptr<ExtendedAtomSpace> atomspace);
    
    /**
     * Core Conversion Functions
     */
    
    // Convert tensor to symbolic representation
    std::shared_ptr<ExtendedAtomSpace::Atom> tensorToSymbolic(
        const std::shared_ptr<RocqProverBridge::TensorDescriptor>& tensor,
        const std::string& symbolic_type = "RealTensor"
    );
    
    // Convert symbolic atom to tensor representation  
    std::shared_ptr<RocqProverBridge::TensorDescriptor> symbolicToTensor(
        const std::shared_ptr<ExtendedAtomSpace::Atom>& atom,
        const std::vector<size_t>& target_shape = {},
        const std::string& dtype = "float32"
    );
    
    // Batch conversion: multiple tensors to AtomSpace
    std::vector<std::shared_ptr<ExtendedAtomSpace::Atom>> batchTensorToSymbolic(
        const std::vector<std::shared_ptr<RocqProverBridge::TensorDescriptor>>& tensors
    );
    
    // Batch conversion: AtomSpace subgraph to tensors
    std::vector<std::shared_ptr<RocqProverBridge::TensorDescriptor>> batchSymbolicToTensor(
        const std::vector<std::shared_ptr<ExtendedAtomSpace::Atom>>& atoms
    );
    
    /**
     * Type-Safe Conversion with Formal Verification
     */
    
    // Verify conversion preserves semantics
    bool verifySemanticPreservation(
        const std::shared_ptr<RocqProverBridge::TensorDescriptor>& tensor,
        const std::shared_ptr<ExtendedAtomSpace::Atom>& atom
    );
    
    // Generate formal proof of conversion correctness
    std::shared_ptr<RocqProverBridge::TheoremProof> generateConversionProof(
        const std::shared_ptr<RocqProverBridge::TensorDescriptor>& tensor,
        const std::shared_ptr<ExtendedAtomSpace::Atom>& atom,
        const std::string& theorem_name = ""
    );
    
    // Verify bijection property: encode(decode(x)) = x
    bool verifyBijectionProperty(
        const std::shared_ptr<RocqProverBridge::TensorDescriptor>& original_tensor
    );
    
    /**
     * Attention Mechanism Integration
     */
    
    // Convert attention tensor to symbolic attention values
    std::shared_ptr<ExtendedAtomSpace::Link> attentionTensorToSymbolic(
        const std::shared_ptr<RocqProverBridge::TensorDescriptor>& attention_tensor,
        const std::vector<std::shared_ptr<ExtendedAtomSpace::Atom>>& target_atoms
    );
    
    // Generate attention allocation from symbolic importance values
    std::shared_ptr<RocqProverBridge::TensorDescriptor> symbolicToAttentionTensor(
        const std::vector<std::shared_ptr<ExtendedAtomSpace::Atom>>& atoms_with_importance
    );
    
    // Verify attention conservation with formal proof
    std::shared_ptr<RocqProverBridge::TheoremProof> verifyAttentionConservation(
        const std::shared_ptr<RocqProverBridge::TensorDescriptor>& attention_tensor
    );
    
    /**
     * Gradient and Differentiability Preservation
     */
    
    // Ensure conversion preserves gradient information
    bool preserveGradients(
        const std::shared_ptr<RocqProverBridge::TensorDescriptor>& tensor,
        const std::shared_ptr<ExtendedAtomSpace::Atom>& symbolic_result
    );
    
    // Generate differentiability proof
    std::shared_ptr<RocqProverBridge::TheoremProof> generateDifferentiabilityProof(
        const std::shared_ptr<RocqProverBridge::TensorDescriptor>& tensor
    );
    
    /**
     * Error Handling and Recovery
     */
    
    // Detect and report type mismatches
    struct TypeMismatchReport {
        std::string error_type;
        std::string expected_type;
        std::string actual_type;
        std::string suggested_fix;
        bool recoverable;
    };
    
    std::vector<TypeMismatchReport> detectTypeMismatches(
        const std::shared_ptr<RocqProverBridge::TensorDescriptor>& tensor,
        const std::string& target_symbolic_type
    );
    
    // Attempt automatic type coercion
    bool attemptTypeCoercion(
        std::shared_ptr<RocqProverBridge::TensorDescriptor>& tensor,
        const std::string& target_type
    );
    
    /**
     * Performance Optimization
     */
    
    // Cache frequently used conversions
    void enableConversionCache(size_t max_cache_size = 1000);
    void disableConversionCache();
    void clearConversionCache();
    
    // Parallel conversion for batch operations
    void enableParallelConversion(size_t num_threads = 0);
    void disableParallelConversion();
    
    /**
     * Query and Configuration Functions
     */
    
    // Get current conversion specification
    const ConversionSpec& getConversionSpec() const { return spec_; }
    
    // Update conversion parameters
    bool updateConversionSpec(const ConversionSpec& new_spec);
    
    // Get performance metrics
    const BridgeMetrics& getMetrics() const { return metrics_; }
    
    // Reset metrics
    void resetMetrics();
    
    // Get verification context status
    const VerificationContext& getVerificationContext() const { return verification_context_; }
    
    // Validate bridge configuration
    bool validateConfiguration();
    
    /**
     * Advanced Integration Features
     */
    
    // Register custom conversion functions
    using CustomConverter = std::function<std::shared_ptr<ExtendedAtomSpace::Atom>(
        const std::shared_ptr<RocqProverBridge::TensorDescriptor>&)>;
    
    bool registerCustomConverter(const std::string& tensor_type, 
                                const std::string& symbolic_type,
                                CustomConverter converter);
    
    // Generate conversion statistics report
    std::string generatePerformanceReport() const;
    
    // Export conversion proofs to file
    bool exportProofsToFile(const std::string& filepath,
                           const std::string& format = "coq") const;

private:
    ConversionSpec spec_;
    VerificationContext verification_context_;
    BridgeMetrics metrics_;
    
    // Core components
    std::shared_ptr<RocqProverBridge> rocq_bridge_;
    std::shared_ptr<ExtendedAtomSpace> atomspace_;
    
    // Internal state
    bool initialized_ = false;
    bool cache_enabled_ = false;
    bool parallel_enabled_ = false;
    size_t num_threads_ = 1;
    
    // Conversion cache
    struct CacheEntry {
        std::string tensor_hash;
        std::shared_ptr<ExtendedAtomSpace::Atom> symbolic_result;
        std::chrono::steady_clock::time_point timestamp;
    };
    std::vector<CacheEntry> conversion_cache_;
    size_t max_cache_size_ = 1000;
    
    // Custom converters
    std::unordered_map<std::string, CustomConverter> custom_converters_;
    
    // Helper methods
    std::string computeTensorHash(const std::shared_ptr<RocqProverBridge::TensorDescriptor>& tensor);
    bool isValidTensorShape(const std::vector<size_t>& shape);
    bool isValidSymbolicType(const std::string& type);
    void updateMetricsOnFailure();
    
    // Type checking helpers
    bool checkTypeCompatibility(const std::string& tensor_dtype, const std::string& symbolic_type);
    std::string inferSymbolicType(const std::shared_ptr<RocqProverBridge::TensorDescriptor>& tensor);
    
    // Conversion implementation details
    std::shared_ptr<ExtendedAtomSpace::Node> convertScalarTensor(
        const std::shared_ptr<RocqProverBridge::TensorDescriptor>& tensor);
    std::shared_ptr<ExtendedAtomSpace::Link> convertVectorTensor(
        const std::shared_ptr<RocqProverBridge::TensorDescriptor>& tensor);
    std::shared_ptr<ExtendedAtomSpace::Link> convertMatrixTensor(
        const std::shared_ptr<RocqProverBridge::TensorDescriptor>& tensor);
    
    // Verification helpers
    bool verifyConversionInvariant(
        const std::shared_ptr<RocqProverBridge::TensorDescriptor>& tensor,
        const std::shared_ptr<ExtendedAtomSpace::Atom>& atom,
        const std::string& invariant_name);
};

/**
 * Factory for creating NeuralSymbolicBridge instances
 */
class NeuralSymbolicBridgeFactory {
public:
    // Create bridge with default settings
    static std::unique_ptr<NeuralSymbolicBridge> createDefault(
        std::shared_ptr<RocqProverBridge> rocq_bridge,
        std::shared_ptr<ExtendedAtomSpace> atomspace
    );
    
    // Create bridge optimized for performance
    static std::unique_ptr<NeuralSymbolicBridge> createPerformanceOptimized(
        std::shared_ptr<RocqProverBridge> rocq_bridge,
        std::shared_ptr<ExtendedAtomSpace> atomspace,
        size_t cache_size = 10000,
        size_t num_threads = 4
    );
    
    // Create bridge optimized for accuracy
    static std::unique_ptr<NeuralSymbolicBridge> createAccuracyOptimized(
        std::shared_ptr<RocqProverBridge> rocq_bridge,
        std::shared_ptr<ExtendedAtomSpace> atomspace,
        double precision_threshold = 1e-12
    );
    
    // Create bridge for testing
    static std::unique_ptr<NeuralSymbolicBridge> createForTesting();
};

} // namespace opencog_qat