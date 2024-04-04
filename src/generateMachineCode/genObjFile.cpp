#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/TargetParser/Host.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/MC/TargetRegistry.h"

#include <iostream>
#include <filesystem>
#include <string>

#include "generateMachineCode/genObjFile.h"

void GenerateOBJ::generateObjectFile(llvm::Module* module, const std::string& outputFilename) {
	
    if (!module) {
        std::cerr << "Module is null, cannot generate object file\n";
        return;
    }
    
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmPrinters();
    LLVMInitializeX86TargetInfo();
    LLVMInitializeX86Target();
    LLVMInitializeX86TargetMC();
    LLVMInitializeX86AsmPrinter();

    auto TargetTriple = llvm::sys::getDefaultTargetTriple();
    std::string Error;
    auto Target = llvm::TargetRegistry::lookupTarget(TargetTriple, Error);

    llvm::TargetOptions opt;
    llvm::Reloc::Model RM = llvm::Reloc::Model::PIC_;
    auto targetMachine = Target->createTargetMachine(TargetTriple, "generic", "", opt, RM);

    module->setDataLayout(targetMachine->createDataLayout());
    module->setTargetTriple(TargetTriple);

    std::filesystem::path objPath = outputFilename;
    objPath.replace_extension(".o");
    std::error_code EC;
    llvm::raw_fd_ostream dest(objPath.string(), EC, llvm::sys::fs::OF_None);

    llvm::legacy::PassManager pass;
    auto fileType = llvm::CodeGenFileType::ObjectFile;
    if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, fileType)) {
        llvm::errs() << "TheTargetMachine can't emit a file of this type";
        return;
    }

    pass.run(*module);
    dest.flush();
    std::cout << "Object file generated: " << objPath << std::endl;

}