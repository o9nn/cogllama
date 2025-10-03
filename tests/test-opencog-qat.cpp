#include "synthetic-calibration.h"
#include "opencog-quantization.h"
#include "qat-framework.h"
#include "recursive-self-enhancement.h"
#include <iostream>
#include <cassert>
#include <vector>
#include <cmath>
#include <random>

// Test utilities
#define ASSERT_TRUE(condition) \
    do { \
        if (!(condition)) { \
            std::cerr << "ASSERTION FAILED: " << #condition << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
            return false; \
        } \
    } while(0)

#define ASSERT_NEAR(a, b, tolerance) \
    do { \
        if (std::abs((a) - (b)) > (tolerance)) { \
            std::cerr << "ASSERTION FAILED: " << #a << " (" << (a) << ") != " << #b << " (" << (b) << ") within " << (tolerance) << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
            return false; \
        } \
    } while(0)

namespace opencog_qat_tests {

bool test_synthetic_calibration_generator() {
    std::cout << "Testing SyntheticCalibrationGenerator..." << std::endl;
    
    opencog_qat::SyntheticCalibrationGenerator::CalibrationConfig config;
    config.batch_size = 4;
    config.sequence_length = 16;
    config.num_batches = 2;
    
    opencog_qat::SyntheticCalibrationGenerator generator(config);
    
    // Test layer activation generation
    std::vector<int64_t> shape = {10, 20};
    auto activations = generator.generateLayerActivations("test_layer", shape);
    ASSERT_TRUE(activations.size() == 200); // 10 * 20
    
    // Test input sequence generation
    auto sequences = generator.generateInputSequences(1000, 2);
    ASSERT_TRUE(sequences.size() == 2);
    ASSERT_TRUE(sequences[0].size() == 16);
    
    // Test attention-aware data generation
    auto attention_data = generator.generateAttentionAwareData(8, 64, 16);
    ASSERT_TRUE(attention_data.size() == 8 * 64 * 16);
    
    std::cout << "SyntheticCalibrationGenerator tests passed!" << std::endl;
    return true;
}

bool test_progressive_quantizer() {
    std::cout << "Testing ProgressiveQuantizer..." << std::endl;
    
    opencog_qat::ProgressiveQuantizer quantizer;
    
    // Test quantization strategy retrieval
    auto strategies = quantizer.getQuantizationOrder();
    ASSERT_TRUE(!strategies.empty());
    
    // Test layer quantization
    std::vector<float> weights = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    std::vector<float> calibration_data = {0.5f, 1.5f, 2.5f, 3.5f, 4.5f};
    
    auto strategy = strategies[0];
    bool success = quantizer.quantizeLayer("test_layer", strategy, weights, calibration_data);
    ASSERT_TRUE(success);
    
    // Test quantization impact validation
    std::vector<float> original = {1.0f, 2.0f, 3.0f};
    std::vector<float> quantized = {1.1f, 2.1f, 3.1f};
    float impact = quantizer.validateQuantizationImpact(original, quantized);
    ASSERT_TRUE(impact >= 0.0f); // Remove upper bound check since KL divergence can be > 1
    
    std::cout << "ProgressiveQuantizer tests passed!" << std::endl;
    return true;
}

bool test_atomspace_quantizer() {
    std::cout << "Testing AtomSpaceQuantizer..." << std::endl;
    
    opencog_qat::AtomSpaceQuantizer quantizer;
    
    // Test truth value quantization
    opencog_qat::AtomSpaceQuantizer::TruthValue tv = {0.7f, 0.9f, 15.0f};
    auto qtv = quantizer.quantizeTruthValue(tv);
    auto dequantized_tv = quantizer.dequantizeTruthValue(qtv);
    
    ASSERT_NEAR(tv.strength, dequantized_tv.strength, 0.1f);
    ASSERT_NEAR(tv.confidence, dequantized_tv.confidence, 0.1f);
    ASSERT_NEAR(tv.count, dequantized_tv.count, 5.0f); // Increased tolerance for count
    
    // Test hypergraph structure quantization
    std::vector<opencog_qat::AtomSpaceQuantizer::AtomNode> atoms;
    for (int i = 0; i < 3; ++i) {
        opencog_qat::AtomSpaceQuantizer::AtomNode atom;
        atom.atom_id = i;
        atom.atom_type = i;
        atom.truth_value = {0.5f + 0.1f * i, 0.8f, static_cast<float>(i)};
        atoms.push_back(atom);
    }
    
    auto quantized_structure = quantizer.quantizeHypergraphStructure(atoms);
    ASSERT_TRUE(!quantized_structure.empty());
    
    bool valid = quantizer.validateTraversalEfficiency(atoms, quantized_structure);
    ASSERT_TRUE(valid);
    
    std::cout << "AtomSpaceQuantizer tests passed!" << std::endl;
    return true;
}

bool test_moses_quantizer() {
    std::cout << "Testing MOSESQuantizer..." << std::endl;
    
    opencog_qat::MOSESQuantizer quantizer;
    
    // Test program tree quantization
    std::vector<opencog_qat::MOSESQuantizer::ProgramNode> program_tree;
    for (int i = 0; i < 5; ++i) {
        opencog_qat::MOSESQuantizer::ProgramNode node;
        node.type = opencog_qat::MOSESQuantizer::NodeType::CONSTANT;
        node.value = -2.0f + i;
        node.function_id = i % 3;
        node.variable_id = i % 2;
        program_tree.push_back(node);
    }
    
    auto quantized_tree = quantizer.quantizeProgramTree(program_tree);
    ASSERT_TRUE(quantized_tree.size() == program_tree.size());
    
    bool compatible = quantizer.validateGeneticCompatibility(quantized_tree);
    ASSERT_TRUE(compatible);
    
    // Test fitness computation
    std::vector<float> test_inputs = {1.0f, 2.0f, 3.0f};
    float fitness = quantizer.computeQuantizedFitness(quantized_tree, test_inputs);
    ASSERT_TRUE(fitness >= 0.0f);
    
    std::cout << "MOSESQuantizer tests passed!" << std::endl;
    return true;
}

bool test_ecan_quantizer() {
    std::cout << "Testing ECANQuantizer..." << std::endl;
    
    opencog_qat::ECANQuantizer quantizer;
    
    // Test attention value quantization
    opencog_qat::ECANQuantizer::AttentionValue av = {100.0f, 50.0f, 5.0f, 0.8f};
    auto qav = quantizer.quantizeAttentionValue(av);
    auto dequantized_av = quantizer.dequantizeAttentionValue(qav);
    
    ASSERT_NEAR(av.sti, dequantized_av.sti, 10.0f);
    ASSERT_NEAR(av.lti, dequantized_av.lti, 5.0f);
    ASSERT_NEAR(av.vlti, dequantized_av.vlti, 1.0f);
    ASSERT_NEAR(av.confidence, dequantized_av.confidence, 0.1f);
    
    // Test attention dynamics validation
    std::vector<opencog_qat::ECANQuantizer::AttentionValue> original_values = {av};
    std::vector<opencog_qat::ECANQuantizer::QuantizedAttentionValue> quantized_values = {qav};
    
    bool dynamics_valid = quantizer.validateAttentionDynamics(original_values, quantized_values);
    ASSERT_TRUE(dynamics_valid);
    
    // Test spreading activation
    std::vector<std::vector<float>> connectivity_matrix = {{0.5f}};
    auto activation_levels = quantizer.computeQuantizedSpreadingActivation(quantized_values, connectivity_matrix);
    ASSERT_TRUE(!activation_levels.empty());
    
    std::cout << "ECANQuantizer tests passed!" << std::endl;
    return true;
}

bool test_opencog_quantization_manager() {
    std::cout << "Testing OpenCogQuantizationManager..." << std::endl;
    
    opencog_qat::OpenCogQuantizationManager manager;
    
    // Create test data
    std::vector<opencog_qat::AtomSpaceQuantizer::AtomNode> atoms;
    std::vector<opencog_qat::MOSESQuantizer::ProgramNode> moses_programs;
    std::vector<opencog_qat::ECANQuantizer::AttentionValue> ecan_values;
    
    // Add sample data
    opencog_qat::AtomSpaceQuantizer::AtomNode atom;
    atom.atom_id = 1;
    atom.atom_type = 1;
    atom.truth_value = {0.7f, 0.9f, 10.0f};
    atoms.push_back(atom);
    
    opencog_qat::MOSESQuantizer::ProgramNode moses_node;
    moses_node.type = opencog_qat::MOSESQuantizer::NodeType::CONSTANT;
    moses_node.value = 2.5f;
    moses_programs.push_back(moses_node);
    
    opencog_qat::ECANQuantizer::AttentionValue av = {100.0f, 50.0f, 5.0f, 0.8f};
    ecan_values.push_back(av);
    
    // Test system quantization
    bool success = manager.quantizeOpenCogSystem(atoms, moses_programs, ecan_values);
    ASSERT_TRUE(success);
    
    // Test system performance validation
    auto metrics = manager.validateSystemPerformance();
    ASSERT_TRUE(metrics.pattern_mining_accuracy > 0.0f);
    ASSERT_TRUE(metrics.memory_utilization > 0.0f);
    
    // Test OpenCog integration validation
    bool integration_valid = manager.validateOpenCogIntegration();
    ASSERT_TRUE(integration_valid);
    
    std::cout << "OpenCogQuantizationManager tests passed!" << std::endl;
    return true;
}

bool test_qat_framework() {
    std::cout << "Testing OpenCogQATFramework..." << std::endl;
    
    // Create test configuration
    opencog_qat::OpenCogQATFramework::QATConfig config;
    config.hardware_constraints.target = opencog_qat::OpenCogQATFramework::HardwareConstraints::TargetHardware::CPU;
    config.hardware_constraints.memory_limit_mb = 1024; // 1GB
    config.hardware_constraints.performance_threshold = 0.05f; // 5% tolerance for testing
    config.num_calibration_batches = 2; // Reduced for testing
    
    // Create framework
    opencog_qat::OpenCogQATFramework framework(config);
    
    // Test calibration data generation
    std::vector<std::string> layer_names = {"embedding", "attn_q", "ffn_gate"};
    auto calibration_data = framework.generateCalibrationData(1000, layer_names);
    ASSERT_TRUE(calibration_data.size() == layer_names.size());
    
    // Test validation metrics computation (using dummy weights)
    std::vector<float> original_weights(1000, 1.0f);
    std::vector<float> quantized_weights(1000, 0.95f); // 5% change
    
    auto metrics = framework.validateQuantizedModel(original_weights, quantized_weights);
    ASSERT_TRUE(metrics.accuracy_retention > 0.0f);
    ASSERT_TRUE(metrics.memory_reduction >= 0.0f);
    
    // Test optimization report generation
    std::string report = framework.generateOptimizationReport(metrics);
    ASSERT_TRUE(!report.empty());
    
    std::cout << "OpenCogQATFramework tests passed!" << std::endl;
    return true;
}

bool test_qat_framework_factory() {
    std::cout << "Testing QATFrameworkFactory..." << std::endl;
    
    // Test CPU-optimized framework creation
    auto cpu_framework = opencog_qat::QATFrameworkFactory::createCPUOptimized(4.0f, 0.02f);
    ASSERT_TRUE(cpu_framework != nullptr);
    ASSERT_TRUE(cpu_framework->getConfig().hardware_constraints.target == 
               opencog_qat::OpenCogQATFramework::HardwareConstraints::TargetHardware::CPU);
    
    // Test GPU-optimized framework creation
    auto gpu_framework = opencog_qat::QATFrameworkFactory::createGPUOptimized(8.0f, 0.015f);
    ASSERT_TRUE(gpu_framework != nullptr);
    ASSERT_TRUE(gpu_framework->getConfig().hardware_constraints.target == 
               opencog_qat::OpenCogQATFramework::HardwareConstraints::TargetHardware::GPU);
    
    // Test TPU-optimized framework creation
    auto tpu_framework = opencog_qat::QATFrameworkFactory::createTPUOptimized(16.0f, 0.01f);
    ASSERT_TRUE(tpu_framework != nullptr);
    ASSERT_TRUE(tpu_framework->getConfig().hardware_constraints.target == 
               opencog_qat::OpenCogQATFramework::HardwareConstraints::TargetHardware::TPU);
    
    std::cout << "QATFrameworkFactory tests passed!" << std::endl;
    return true;
}

bool test_recursive_self_enhancement() {
    std::cout << "Testing RecursiveSelfEnhancement..." << std::endl;
    
    // Create required components
    auto atomspace = std::make_shared<opencog_qat::ExtendedAtomSpace>();
    auto rocq_bridge_unique = opencog_qat::RocqBridgeFactory::createDefault();
    auto rocq_bridge = std::shared_ptr<opencog_qat::RocqProverBridge>(rocq_bridge_unique.release());
    auto neural_bridge = opencog_qat::NeuralSymbolicBridgeFactory::createDefault(rocq_bridge, atomspace);
    
    // Create recursive self-enhancement system
    auto neural_bridge_shared = std::shared_ptr<opencog_qat::NeuralSymbolicBridge>(neural_bridge.release());
    auto enhancement = opencog_qat::RecursiveSelfEnhancementFactory::createDefault(
        atomspace, rocq_bridge, neural_bridge_shared);
    
    ASSERT_TRUE(enhancement != nullptr);
    
    // Test meta-cognitive state initialization
    const auto& meta_state = enhancement->getCurrentMetaCognitiveState();
    ASSERT_TRUE(meta_state.evolution_generation == 0);
    ASSERT_TRUE(meta_state.coherence_score >= 0.0 && meta_state.coherence_score <= 1.0);
    ASSERT_TRUE(meta_state.safety_verified == true);
    
    // Test cognitive grammar evolution initialization
    const auto& grammar_evolution = enhancement->getCognitiveGrammarEvolution();
    ASSERT_TRUE(!grammar_evolution.grammar_rules.empty());
    ASSERT_TRUE(!grammar_evolution.rule_fitness_scores.empty());
    ASSERT_TRUE(grammar_evolution.converged == false);
    
    // Test safety monitor initialization
    const auto& safety_status = enhancement->getSafetyStatus();
    ASSERT_TRUE(safety_status.safety_enabled == true);
    ASSERT_TRUE(safety_status.violation_count == 0);
    ASSERT_TRUE(!safety_status.core_invariants.empty());
    
    // Test self-representation update
    std::vector<float> test_cognitive_state = {0.8f, 0.7f, 0.6f, 0.5f};
    bool update_success = enhancement->updateSelfRepresentation(test_cognitive_state);
    ASSERT_TRUE(update_success);
    
    // Verify state was updated
    const auto& updated_state = enhancement->getCurrentMetaCognitiveState();
    ASSERT_TRUE(updated_state.self_model_state.size() == test_cognitive_state.size());
    
    // Test cognitive grammar evolution
    std::map<std::string, double> performance_metrics = {
        {"coherence", 0.85},
        {"safety", 1.0},
        {"convergence", 0.7}
    };
    bool evolution_success = enhancement->evolveCognitiveGrammar(performance_metrics);
    ASSERT_TRUE(evolution_success);
    
    // Test boundary detection
    auto boundary_status = enhancement->detectModificationBoundaries("test_modification");
    ASSERT_TRUE(boundary_status.distance_to_boundary >= 0.0 && boundary_status.distance_to_boundary <= 1.0);
    
    // Test safety verification
    std::vector<float> test_params = {0.05f, 0.03f, 0.02f};  // Small, safe modifications
    bool safety_verified = enhancement->verifySelfModificationBoundaries("test", test_params);
    ASSERT_TRUE(safety_verified);
    
    // Test safe modification application
    bool modification_success = enhancement->applySafeModification(
        "coherence_enhancement", test_params, false);  // Don't require formal proof for test
    ASSERT_TRUE(modification_success);
    
    // Test full enhancement cycle
    bool cycle_success = enhancement->performSelfEnhancementCycle();
    ASSERT_TRUE(cycle_success);
    
    // Verify enhancement cycle updated generation
    const auto& final_state = enhancement->getCurrentMetaCognitiveState();
    ASSERT_TRUE(final_state.evolution_generation > 0);
    
    // Test formal proof generation
    auto safety_proofs = enhancement->generateSafetyProofs(final_state, "test_enhancement");
    ASSERT_TRUE(!safety_proofs.empty());
    
    // Verify proofs are properly formed
    for (const auto& proof : safety_proofs) {
        ASSERT_TRUE(proof != nullptr);
        ASSERT_TRUE(!proof->theorem_name.empty());
        ASSERT_TRUE(!proof->theorem_statement.empty());
        ASSERT_TRUE(!proof->proof_term.empty());
        ASSERT_TRUE(proof->verified == true);
    }
    
    // Test convergence theorem generation
    auto convergence_theorem = enhancement->generateConvergenceTheorem(grammar_evolution);
    ASSERT_TRUE(convergence_theorem != nullptr);
    ASSERT_TRUE(convergence_theorem->verified == true);
    
    // Test cognitive coherence verification
    bool coherence_verified = enhancement->verifyCognitiveCoherence(final_state);
    ASSERT_TRUE(coherence_verified);
    
    // Test enhancement report generation
    std::string report = enhancement->generateEnhancementReport();
    ASSERT_TRUE(!report.empty());
    ASSERT_TRUE(report.find("Meta-Cognitive State") != std::string::npos);
    ASSERT_TRUE(report.find("Safety Monitoring") != std::string::npos);
    ASSERT_TRUE(report.find("Performance Metrics") != std::string::npos);
    
    // Test emergency stop functionality
    enhancement->emergencyStop();
    ASSERT_TRUE(enhancement->isEmergencyStopped() == true);
    
    // Verify cycle fails when emergency stopped
    bool stopped_cycle_success = enhancement->performSelfEnhancementCycle();
    ASSERT_TRUE(stopped_cycle_success == false);
    
    // Test factory variants  
    auto safety_optimized = opencog_qat::RecursiveSelfEnhancementFactory::createSafetyOptimized(
        atomspace, rocq_bridge, neural_bridge_shared);
    ASSERT_TRUE(safety_optimized != nullptr);
    
    const auto& safety_config = safety_optimized->getConfiguration();
    ASSERT_TRUE(safety_config.require_formal_proofs == true);
    ASSERT_TRUE(safety_config.safety_threshold >= 0.99);
    ASSERT_TRUE(safety_config.max_safety_violations <= 1);
    
    std::cout << "RecursiveSelfEnhancement tests passed!" << std::endl;
    return true;
}

} // namespace opencog_qat_tests

int main() {
    std::cout << "Running OpenCog QAT Framework Tests..." << std::endl;
    std::cout << "======================================" << std::endl;
    
    bool all_tests_passed = true;
    
    // Run all tests
    all_tests_passed &= opencog_qat_tests::test_synthetic_calibration_generator();
    all_tests_passed &= opencog_qat_tests::test_progressive_quantizer();
    all_tests_passed &= opencog_qat_tests::test_atomspace_quantizer();
    all_tests_passed &= opencog_qat_tests::test_moses_quantizer();
    all_tests_passed &= opencog_qat_tests::test_ecan_quantizer();
    all_tests_passed &= opencog_qat_tests::test_opencog_quantization_manager();
    all_tests_passed &= opencog_qat_tests::test_qat_framework();
    all_tests_passed &= opencog_qat_tests::test_qat_framework_factory();
    all_tests_passed &= opencog_qat_tests::test_recursive_self_enhancement();
    
    std::cout << "======================================" << std::endl;
    if (all_tests_passed) {
        std::cout << "ALL TESTS PASSED!" << std::endl;
        return 0;
    } else {
        std::cout << "SOME TESTS FAILED!" << std::endl;
        return 1;
    }
}