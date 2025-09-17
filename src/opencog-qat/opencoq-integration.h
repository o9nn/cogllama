#pragma once

#include "rocq-prover-bridge.h"
#include "neural-symbolic-bridge.h"
#include "extended-atomspace.h"
#include <memory>
#include <vector>
#include <string>

namespace opencog_qat {

/**
 * OpenCoq Neural-Symbolic Integration Manager
 * 
 * Provides a high-level interface for the complete OpenCoq neural-symbolic
 * bridge with automated theorem generation, formal verification, and 
 * attention allocation algorithms.
 */
class OpenCoqIntegration {
public:
    struct IntegrationConfig {
        // Rocq Prover settings
        std::string rocq_path;
        std::string scheme_interpreter;
        std::string ocaml_runtime;
        
        // Neural-symbolic bridge settings
        double precision_threshold;
        bool enable_formal_verification;
        bool enable_automated_proving;
        bool enable_attention_verification;
        
        // Performance settings
        size_t conversion_cache_size;
        size_t parallel_threads;
        bool enable_performance_optimization;
        
        // Safety settings
        size_t max_tensor_size;
        size_t max_atom_count;
        double max_conversion_time_ms;
        
        // Default constructor
        IntegrationConfig() :
            rocq_path(""),
            scheme_interpreter("guile"),
            ocaml_runtime("ocaml"),
            precision_threshold(1e-6),
            enable_formal_verification(true),
            enable_automated_proving(true),
            enable_attention_verification(true),
            conversion_cache_size(10000),
            parallel_threads(4),
            enable_performance_optimization(true),
            max_tensor_size(1000000),
            max_atom_count(100000),
            max_conversion_time_ms(5000.0) {}
    };
    
    struct IntegrationStatus {
        bool initialized = false;
        bool rocq_connected = false;
        bool bridge_ready = false;
        bool atomspace_ready = false;
        
        // Component status
        RocqProverBridge::RocqStatus rocq_status;
        NeuralSymbolicBridge::BridgeMetrics bridge_metrics;
        ExtendedAtomSpace::AtomSpaceStats atomspace_stats;
        
        // Overall health
        double overall_health_score = 0.0;
        std::vector<std::string> warnings;
        std::vector<std::string> errors;
    };

public:
    explicit OpenCoqIntegration(const IntegrationConfig& config = IntegrationConfig());
    ~OpenCoqIntegration();
    
    /**
     * Initialize the complete integration system
     */
    bool initialize();
    
    /**
     * Core Integration Functions
     */
    
    // Demonstrate complete neural-symbolic pipeline
    bool demonstrateNeuralSymbolicPipeline(
        const std::vector<float>& input_tensor_data,
        const std::vector<size_t>& tensor_shape
    );
    
    // Generate and verify theorem from hypergraph inference
    std::shared_ptr<RocqProverBridge::TheoremProof> processInferenceWithProofs(
        const std::vector<std::shared_ptr<ExtendedAtomSpace::Atom>>& premises,
        const std::shared_ptr<ExtendedAtomSpace::Atom>& conclusion,
        const std::string& inference_rule = "modus_ponens"
    );
    
    // Verify attention allocation algorithm with formal proofs
    bool verifyAttentionAllocationAlgorithm(
        const std::vector<float>& attention_weights,
        const std::vector<std::shared_ptr<ExtendedAtomSpace::Atom>>& target_atoms
    );
    
    // Perform type-safe symbolic-to-numeric conversion with verification
    std::shared_ptr<RocqProverBridge::TensorDescriptor> convertSymbolicWithVerification(
        const std::shared_ptr<ExtendedAtomSpace::Atom>& symbolic_atom,
        const std::string& target_dtype = "float32"
    );
    
    /**
     * Automated Theorem Generation
     */
    
    // Auto-generate theorems from AtomSpace patterns
    std::vector<std::shared_ptr<RocqProverBridge::TheoremProof>> generateTheoremsFromPatterns(
        const std::vector<std::shared_ptr<ExtendedAtomSpace::Link>>& patterns
    );
    
    // Generate bridge correctness theorems
    std::vector<std::shared_ptr<RocqProverBridge::TheoremProof>> generateBridgeCorrectnessTheorems();
    
    // Generate attention mechanism safety theorems
    std::vector<std::shared_ptr<RocqProverBridge::TheoremProof>> generateAttentionSafetyTheorems(
        const std::string& attention_algorithm = "softmax_attention"
    );
    
    /**
     * Formal Verification Pipeline
     */
    
    // Verify complete neural-symbolic bridge correctness
    bool verifyBridgeCorrectness();
    
    // Verify tensor operation type safety
    bool verifyTensorTypeSafety(
        const std::shared_ptr<RocqProverBridge::TensorDescriptor>& tensor
    );
    
    // Verify attention conservation properties
    bool verifyAttentionConservation(
        const std::shared_ptr<RocqProverBridge::TensorDescriptor>& attention_tensor
    );
    
    // Verify bijection property for conversions
    bool verifyConversionBijection(
        const std::shared_ptr<RocqProverBridge::TensorDescriptor>& test_tensor
    );
    
    /**
     * Integration Management
     */
    
    // Get current system status
    IntegrationStatus getStatus() const;
    
    // Generate comprehensive integration report
    std::string generateIntegrationReport() const;
    
    // Export all generated proofs
    bool exportProofsToFile(const std::string& filepath,
                           const std::string& format = "coq") const;
    
    // Run comprehensive integration tests
    bool runIntegrationTests();
    
    // Benchmark integration performance
    struct BenchmarkResults {
        double avg_conversion_time_ms;
        double avg_proof_time_ms;
        double avg_verification_time_ms;
        double total_integration_time_ms;
        size_t successful_operations;
        size_t failed_operations;
        double success_rate;
    };
    
    BenchmarkResults benchmarkIntegration(size_t num_iterations = 100);
    
    /**
     * Advanced Features
     */
    
    // Enable real-time formal verification during tensor operations
    void enableRealTimeVerification(bool enable = true);
    
    // Enable automated proof generation for all operations
    void enableAutomatedProvingForAllOperations(bool enable = true);
    
    // Configure custom verification policies
    struct VerificationPolicy {
        bool require_proof_for_conversions = true;
        bool require_proof_for_inference = true;
        bool require_proof_for_attention = true;
        double max_proof_time_seconds = 10.0;
        bool fail_on_proof_timeout = false;
    };
    
    void setVerificationPolicy(const VerificationPolicy& policy);
    
    // Get component references for advanced usage
    std::shared_ptr<RocqProverBridge> getRocqBridge() const { return rocq_bridge_; }
    std::shared_ptr<NeuralSymbolicBridge> getNeuralSymbolicBridge() const { return neural_bridge_; }
    std::shared_ptr<ExtendedAtomSpace> getAtomSpace() const { return atomspace_; }

private:
    IntegrationConfig config_;
    IntegrationStatus status_;
    VerificationPolicy verification_policy_;
    
    // Core components
    std::shared_ptr<RocqProverBridge> rocq_bridge_;
    std::shared_ptr<NeuralSymbolicBridge> neural_bridge_;
    std::shared_ptr<ExtendedAtomSpace> atomspace_;
    
    // State management
    bool initialized_ = false;
    bool real_time_verification_ = false;
    bool automated_proving_ = false;
    
    // Proof registry
    std::vector<std::shared_ptr<RocqProverBridge::TheoremProof>> generated_proofs_;
    
    // Helper methods
    bool initializeComponents();
    bool validateConfiguration();
    void updateSystemStatus();
    double calculateHealthScore() const;
    
    // Demonstration helpers
    std::shared_ptr<ExtendedAtomSpace::Link> createSampleInference();
    std::vector<std::shared_ptr<ExtendedAtomSpace::Atom>> createSampleAtoms();
    std::shared_ptr<RocqProverBridge::TensorDescriptor> createSampleTensor();
    
    // Verification helpers
    bool verifyComponentIntegrity();
    bool verifyInterComponentCommunication();
    
    // Performance monitoring
    struct PerformanceMetrics {
        double total_time_spent_ms = 0.0;
        size_t total_operations = 0;
        size_t successful_verifications = 0;
        size_t failed_verifications = 0;
        std::vector<double> operation_times_ms;
    } performance_metrics_;
    
    void recordOperationTime(double time_ms);
    void recordVerificationResult(bool success);
};

/**
 * Factory for creating OpenCoqIntegration instances
 */
class OpenCoqIntegrationFactory {
public:
    // Create integration with default configuration
    static std::unique_ptr<OpenCoqIntegration> createDefault();
    
    // Create integration optimized for performance
    static std::unique_ptr<OpenCoqIntegration> createPerformanceOptimized();
    
    // Create integration optimized for formal verification
    static std::unique_ptr<OpenCoqIntegration> createVerificationOptimized();
    
    // Create integration for testing/development
    static std::unique_ptr<OpenCoqIntegration> createForTesting();
    
    // Create integration with custom configuration
    static std::unique_ptr<OpenCoqIntegration> createCustom(
        const OpenCoqIntegration::IntegrationConfig& config
    );
};

} // namespace opencog_qat