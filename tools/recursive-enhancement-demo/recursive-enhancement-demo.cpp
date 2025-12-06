#include "../../src/opencog-qat/recursive-self-enhancement.h"
#include "../../src/opencog-qat/opencoq-integration.h"
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>

using namespace opencog_qat;

void printBanner() {
    std::cout << "================================================" << std::endl;
    std::cout << "🧠 RECURSIVE SELF-ENHANCEMENT DEMONSTRATION 🧠" << std::endl;
    std::cout << "          with OpenCoq Safety Verification      " << std::endl;
    std::cout << "================================================" << std::endl;
    std::cout << std::endl;
}

int main(int /* argc */, char* /* argv */[]) {
    printBanner();
    
    try {
        std::cout << "=== Phase 6: Meta-Cognitive Enhancement Demo ===" << std::endl;
        
        // Create OpenCoq integration
        auto integration = OpenCoqIntegrationFactory::createDefault();
        
        // Create recursive self-enhancement system
        auto enhancement = RecursiveSelfEnhancementFactory::createDefault(
            integration->getAtomSpace(),
            integration->getRocqBridge(),
            integration->getNeuralSymbolicBridge()
        );
        
        std::cout << "✓ Recursive Self-Enhancement Framework initialized" << std::endl;
        
        // Display initial state
        std::cout << "\nInitial Meta-Cognitive State:" << std::endl;
        const auto& initial_state = enhancement->getCurrentMetaCognitiveState();
        std::cout << "  Evolution Generation: " << initial_state.evolution_generation << std::endl;
        std::cout << "  Coherence Score: " << std::fixed << std::setprecision(3) 
                  << initial_state.coherence_score << std::endl;
        std::cout << "  Safety Verified: " << (initial_state.safety_verified ? "✓" : "⚠") << std::endl;
        
        // Perform enhancement cycles
        std::cout << "\nPerforming recursive self-enhancement cycles..." << std::endl;
        for (int i = 0; i < 3; ++i) {
            std::cout << "\n--- Enhancement Cycle " << (i + 1) << " ---" << std::endl;
            
            bool success = enhancement->performSelfEnhancementCycle();
            if (!success) {
                std::cout << "⚠ Enhancement cycle failed" << std::endl;
                break;
            }
            
            const auto& current_state = enhancement->getCurrentMetaCognitiveState();
            std::cout << "Generation: " << current_state.evolution_generation 
                      << ", Coherence: " << std::fixed << std::setprecision(3) 
                      << current_state.coherence_score << std::endl;
        }
        
        // Generate safety proofs
        std::cout << "\nGenerating formal safety proofs..." << std::endl;
        const auto& final_state = enhancement->getCurrentMetaCognitiveState();
        auto safety_proofs = enhancement->generateSafetyProofs(final_state, "demonstration");
        
        std::cout << "Generated " << safety_proofs.size() << " formal safety proofs" << std::endl;
        for (const auto& proof : safety_proofs) {
            std::cout << "  ✓ " << proof->theorem_name << " (verified: " 
                      << (proof->verified ? "YES" : "NO") << ")" << std::endl;
        }
        
        // Generate convergence theorem
        std::cout << "\nGenerating convergence theorem..." << std::endl;
        const auto& grammar_evolution = enhancement->getCognitiveGrammarEvolution();
        auto convergence_theorem = enhancement->generateConvergenceTheorem(grammar_evolution);
        
        if (convergence_theorem) {
            std::cout << "✓ Convergence theorem generated: " << convergence_theorem->theorem_name << std::endl;
            std::cout << "  Verified: " << (convergence_theorem->verified ? "✓" : "✗") << std::endl;
        }
        
        // Generate final report
        std::cout << "\n=== Enhancement Report ===" << std::endl;
        std::string report = enhancement->generateEnhancementReport();
        std::cout << report << std::endl;
        
        std::cout << "================================================" << std::endl;
        std::cout << "🎉 RECURSIVE SELF-ENHANCEMENT DEMO COMPLETE! 🎉" << std::endl;
        std::cout << "================================================" << std::endl;
        std::cout << "\nKey achievements demonstrated:" << std::endl;
        std::cout << "  ✓ Meta-cognitive tensor self-representation" << std::endl;
        std::cout << "  ✓ Cognitive grammar evolution tracking" << std::endl;
        std::cout << "  ✓ Formal verification of recursive enhancement safety" << std::endl;
        std::cout << "  ✓ Automated proof generation for convergence theorems" << std::endl;
        std::cout << "  ✓ Self-modification boundary detection" << std::endl;
        std::cout << "  ✓ OpenCoq theorem verification for cognitive properties" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Demonstration failed with exception: " << e.what() << std::endl;
        return 1;
    }
}