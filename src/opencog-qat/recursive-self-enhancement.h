#pragma once

#include "extended-atomspace.h"
#include "rocq-prover-bridge.h"
#include "neural-symbolic-bridge.h"
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <chrono>

namespace opencog_qat {

/**
 * Recursive Self-Enhancement Framework with OpenCoq Safety Verification
 * 
 * Implements Phase 6 Meta-Cognitive Enhancement including:
 * - Meta-cognitive tensors for self-representation
 * - Cognitive grammar evolution tracking
 * - Formal verification of recursive enhancement safety
 * - Automated proof generation for convergence theorems
 * - Self-modification boundary detection
 * - OpenCoq theorem verification for cognitive properties
 */
class RecursiveSelfEnhancement {
public:
    /**
     * Meta-cognitive tensor for tracking system self-representation
     */
    struct MetaCognitiveTensor {
        // Self-representation components
        std::vector<float> self_model_state;          // Current self-model representation
        std::vector<float> cognitive_grammar;         // Grammar rules for cognitive evolution
        std::vector<float> modification_history;      // History of self-modifications
        std::vector<float> safety_boundaries;         // Current safety constraint boundaries
        
        // Tensor specifications
        ExtendedAtomSpace::TensorShape self_model_shape;
        ExtendedAtomSpace::TensorShape grammar_shape;
        ExtendedAtomSpace::TensorShape history_shape;
        ExtendedAtomSpace::TensorShape boundary_shape;
        
        // Metadata
        std::chrono::steady_clock::time_point last_update;
        size_t evolution_generation;
        double coherence_score;
        bool safety_verified;
        
        MetaCognitiveTensor() : evolution_generation(0), coherence_score(0.0), safety_verified(false) {}
    };
    
    /**
     * Cognitive grammar evolution tracker
     */
    struct CognitiveGrammarEvolution {
        // Grammar rules and patterns
        std::map<std::string, std::vector<float>> grammar_rules;
        std::map<std::string, double> rule_fitness_scores;
        std::vector<std::string> evolution_log;
        
        // Evolution parameters
        double mutation_rate;
        double selection_pressure;
        size_t max_rule_complexity;
        size_t population_size;
        
        // Convergence tracking
        std::vector<double> convergence_metrics;
        bool converged;
        double convergence_threshold;
        
        CognitiveGrammarEvolution() : 
            mutation_rate(0.01), selection_pressure(0.8), 
            max_rule_complexity(100), population_size(50),
            converged(false), convergence_threshold(1e-6) {}
    };
    
    /**
     * Self-modification safety monitor
     */
    struct SelfModificationSafety {
        // Safety boundaries
        std::vector<double> core_invariants;          // Properties that must be preserved
        std::vector<double> modification_limits;      // Maximum allowed changes per cycle
        std::map<std::string, bool> safety_proofs;    // Formal safety proofs
        
        // Monitoring state
        bool safety_enabled;
        size_t violation_count;
        std::vector<std::string> safety_alerts;
        
        // Verification results
        std::vector<std::shared_ptr<RocqProverBridge::TheoremProof>> safety_theorems;
        std::vector<std::shared_ptr<RocqProverBridge::TheoremProof>> convergence_proofs;
        
        SelfModificationSafety() : safety_enabled(true), violation_count(0) {}
    };

public:
    explicit RecursiveSelfEnhancement(
        std::shared_ptr<ExtendedAtomSpace> atomspace,
        std::shared_ptr<RocqProverBridge> rocq_bridge,
        std::shared_ptr<NeuralSymbolicBridge> neural_bridge
    );
    
    ~RecursiveSelfEnhancement();
    
    /**
     * Initialize the recursive self-enhancement system
     */
    bool initialize();
    
    /**
     * Core Self-Enhancement Operations
     */
    
    // Perform one cycle of recursive self-enhancement
    bool performSelfEnhancementCycle();
    
    // Update meta-cognitive self-representation
    bool updateSelfRepresentation(const std::vector<float>& cognitive_state);
    
    // Evolve cognitive grammar based on performance feedback
    bool evolveCognitiveGrammar(const std::map<std::string, double>& performance_metrics);
    
    // Apply safe self-modification with formal verification
    bool applySafeModification(
        const std::string& modification_type,
        const std::vector<float>& modification_params,
        bool require_formal_proof = true
    );
    
    /**
     * Formal Safety Verification
     */
    
    // Verify recursive enhancement safety properties
    std::shared_ptr<RocqProverBridge::TheoremProof> verifyRecursiveSafety(
        const MetaCognitiveTensor& current_state,
        const MetaCognitiveTensor& proposed_state
    );
    
    // Generate convergence theorem for enhancement process
    std::shared_ptr<RocqProverBridge::TheoremProof> generateConvergenceTheorem(
        const CognitiveGrammarEvolution& evolution_state
    );
    
    // Verify self-modification boundaries
    bool verifySelfModificationBoundaries(
        const std::string& modification_type,
        const std::vector<float>& modification_params
    );
    
    // Generate formal proof of enhancement correctness
    std::shared_ptr<RocqProverBridge::TheoremProof> generateEnhancementCorrectnessProof(
        const std::string& enhancement_description,
        const std::vector<float>& before_state,
        const std::vector<float>& after_state
    );
    
    /**
     * Cognitive Property Verification
     */
    
    // Verify cognitive coherence properties
    bool verifyCognitiveCoherence(const MetaCognitiveTensor& tensor);
    
    // Verify consciousness preservation during enhancement
    std::shared_ptr<RocqProverBridge::TheoremProof> verifyConsciousnessPreservation(
        const MetaCognitiveTensor& before_state,
        const MetaCognitiveTensor& after_state
    );
    
    // Verify goal alignment properties
    bool verifyGoalAlignment(const std::vector<std::string>& goals);
    
    // Generate theorem for cognitive improvement bounds
    std::shared_ptr<RocqProverBridge::TheoremProof> generateCognitiveImprovementBounds();
    
    /**
     * Self-Modification Boundary Detection
     */
    
    // Detect when approaching modification boundaries
    struct BoundaryStatus {
        bool approaching_boundary;
        std::string boundary_type;
        double distance_to_boundary;
        std::vector<std::string> safety_recommendations;
        bool requires_formal_verification;
    };
    
    BoundaryStatus detectModificationBoundaries(
        const std::string& proposed_modification
    );
    
    // Set custom safety boundaries
    bool setSafetyBoundaries(
        const std::vector<double>& core_invariants,
        const std::vector<double>& modification_limits
    );
    
    // Validate proposed modification against boundaries
    bool validateModificationSafety(
        const std::string& modification_type,
        const std::vector<float>& parameters
    );
    
    /**
     * Automated Proof Generation
     */
    
    // Generate all necessary safety proofs for enhancement cycle
    std::vector<std::shared_ptr<RocqProverBridge::TheoremProof>> generateSafetyProofs(
        const MetaCognitiveTensor& current_state,
        const std::string& proposed_enhancement
    );
    
    // Generate convergence theorem with formal bounds
    std::shared_ptr<RocqProverBridge::TheoremProof> generateFormalConvergenceTheorem(
        const CognitiveGrammarEvolution& evolution
    );
    
    // Generate self-consistency proofs
    std::vector<std::shared_ptr<RocqProverBridge::TheoremProof>> generateSelfConsistencyProofs();
    
    /**
     * System State and Monitoring
     */
    
    // Get current meta-cognitive state
    const MetaCognitiveTensor& getCurrentMetaCognitiveState() const { return meta_cognitive_tensor_; }
    
    // Get cognitive grammar evolution status
    const CognitiveGrammarEvolution& getCognitiveGrammarEvolution() const { return grammar_evolution_; }
    
    // Get safety monitoring status
    const SelfModificationSafety& getSafetyStatus() const { return safety_monitor_; }
    
    // Generate comprehensive enhancement report
    std::string generateEnhancementReport() const;
    
    // Export all generated proofs and theorems
    bool exportEnhancementProofs(const std::string& filepath) const;
    
    /**
     * Configuration and Control
     */
    
    struct EnhancementConfig {
        // Enhancement parameters
        double enhancement_rate;
        size_t max_enhancement_cycles;
        bool enable_continuous_enhancement;
        
        // Safety parameters
        bool require_formal_proofs;
        double safety_threshold;
        size_t max_safety_violations;
        
        // Convergence parameters
        double convergence_threshold;
        size_t max_convergence_iterations;
        bool enable_adaptive_convergence;
        
        // Performance parameters
        size_t tensor_cache_size;
        bool enable_parallel_verification;
        size_t verification_threads;
        
        EnhancementConfig() :
            enhancement_rate(0.1), max_enhancement_cycles(1000),
            enable_continuous_enhancement(false), require_formal_proofs(true),
            safety_threshold(0.95), max_safety_violations(3),
            convergence_threshold(1e-6), max_convergence_iterations(10000),
            enable_adaptive_convergence(true), tensor_cache_size(10000),
            enable_parallel_verification(true), verification_threads(4) {}
    };
    
    void setConfiguration(const EnhancementConfig& config);
    const EnhancementConfig& getConfiguration() const { return config_; }
    
    // Enable/disable continuous enhancement
    void enableContinuousEnhancement(bool enable);
    
    // Emergency stop for safety
    void emergencyStop();
    bool isEmergencyStopped() const { return emergency_stopped_; }

private:
    // Core components
    std::shared_ptr<ExtendedAtomSpace> atomspace_;
    std::shared_ptr<RocqProverBridge> rocq_bridge_;
    std::shared_ptr<NeuralSymbolicBridge> neural_bridge_;
    
    // Enhancement state
    MetaCognitiveTensor meta_cognitive_tensor_;
    CognitiveGrammarEvolution grammar_evolution_;
    SelfModificationSafety safety_monitor_;
    
    // Configuration and control
    EnhancementConfig config_;
    bool initialized_;
    bool enhancement_active_;
    bool emergency_stopped_;
    
    // Enhancement history
    std::vector<MetaCognitiveTensor> state_history_;
    std::vector<std::shared_ptr<RocqProverBridge::TheoremProof>> all_proofs_;
    
    // Private helper methods
    bool initializeMetaCognitiveTensor();
    bool initializeCognitiveGrammar();
    bool initializeSafetyMonitor();
    
    // Enhancement operations
    bool updateSelfModel(const std::vector<float>& new_state);
    bool mutateCognitiveGrammar();
    bool selectBestGrammarRules();
    
    // Safety operations
    bool checkCoreInvariants(const MetaCognitiveTensor& state);
    bool validateProposedModification(const std::string& type, const std::vector<float>& params);
    void recordSafetyViolation(const std::string& description);
    
    // Verification helpers
    std::string generateCoqTheorem(const std::string& theorem_name, const std::string& statement);
    std::string generateCoqProof(const std::string& theorem_name, const std::string& proof_tactic);
    
    // Utility methods
    double calculateCoherenceScore(const MetaCognitiveTensor& tensor);
    double calculateConvergenceMetric(const CognitiveGrammarEvolution& evolution);
    void updatePerformanceMetrics();
    
    // Tensor operations
    std::vector<float> tensorAdd(const std::vector<float>& a, const std::vector<float>& b);
    std::vector<float> tensorScale(const std::vector<float>& tensor, double scale);
    double tensorNorm(const std::vector<float>& tensor);
    
    // Performance tracking
    struct PerformanceMetrics {
        size_t total_enhancement_cycles;
        size_t successful_enhancements;
        size_t safety_violations;
        double avg_cycle_time_ms;
        double improvement_rate;
        std::chrono::steady_clock::time_point start_time;
    } performance_;
};

/**
 * Factory for creating RecursiveSelfEnhancement instances
 */
class RecursiveSelfEnhancementFactory {
public:
    // Create with default configuration
    static std::unique_ptr<RecursiveSelfEnhancement> createDefault(
        std::shared_ptr<ExtendedAtomSpace> atomspace,
        std::shared_ptr<RocqProverBridge> rocq_bridge,
        std::shared_ptr<NeuralSymbolicBridge> neural_bridge
    );
    
    // Create with safety-optimized configuration
    static std::unique_ptr<RecursiveSelfEnhancement> createSafetyOptimized(
        std::shared_ptr<ExtendedAtomSpace> atomspace,
        std::shared_ptr<RocqProverBridge> rocq_bridge,
        std::shared_ptr<NeuralSymbolicBridge> neural_bridge
    );
    
    // Create with performance-optimized configuration
    static std::unique_ptr<RecursiveSelfEnhancement> createPerformanceOptimized(
        std::shared_ptr<ExtendedAtomSpace> atomspace,
        std::shared_ptr<RocqProverBridge> rocq_bridge,
        std::shared_ptr<NeuralSymbolicBridge> neural_bridge
    );
    
    // Create for testing/development
    static std::unique_ptr<RecursiveSelfEnhancement> createForTesting(
        std::shared_ptr<ExtendedAtomSpace> atomspace,
        std::shared_ptr<RocqProverBridge> rocq_bridge,
        std::shared_ptr<NeuralSymbolicBridge> neural_bridge
    );
};

} // namespace opencog_qat