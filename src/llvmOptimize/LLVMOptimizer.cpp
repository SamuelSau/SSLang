#include <iostream>


#include "llvm/IR/PassManager.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Utils.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Analysis/LoopAnalysisManager.h"
#include "llvmOptimize/LLVMOptimizer.h"
#include "llvm/Analysis/CGSCCPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/OptimizationLevel.h"
#include "llvm/IR/Verifier.h"


void LLVMOptimizer::optimize(llvm::Module* module) {
	// Create the analysis managers.
	// These must be declared in this order so that they are destroyed in the
	// correct order due to inter-analysis-manager references.

	if (!module) {
		std::cerr << "Optimization aborted: module pointer is null." << std::endl;
		return;
	}

	llvm::LoopAnalysisManager LAM;
	llvm::FunctionAnalysisManager FAM;
	llvm::CGSCCAnalysisManager CGAM;
	llvm::ModuleAnalysisManager MAM;

	std::cerr << "Initializing pass managers..." << std::endl;

	// Create the new pass manager builder.
	// Take a look at the PassBuilder constructor parameters for more
	// customization, e.g. specifying a TargetMachine or various debugging
	// options.

	llvm::PassBuilder PB;

	std::cout << "Initializing pass builder..." << std::endl;

	// Register all the basic analyses with the managers.
	PB.registerModuleAnalyses(MAM);
	PB.registerCGSCCAnalyses(CGAM);
	PB.registerFunctionAnalyses(FAM);
	PB.registerLoopAnalyses(LAM);
	PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);

	std::cout << "Registering analyses..." << std::endl;

	// Create the pass manager.
	// This one corresponds to a typical -O2 optimization pipeline.
	llvm::ModulePassManager MPM = PB.buildPerModuleDefaultPipeline(llvm::OptimizationLevel::O3);

	std::cout << "Building default pipeline..." << std::endl;

	if (llvm::verifyModule(*module, &llvm::errs())) {
		throw std::runtime_error("Module verification failed, cannot optimize.");
	}

	// Optimize the IR!
	MPM.run(*module, MAM);

	std::cout << "Optimization complete." << std::endl;
}