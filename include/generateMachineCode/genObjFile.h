#ifndef GEN_OBJ_FILE_H
#define GEN_OBJ_FILE_H

#include "llvm/IR/Module.h"
#include <string>

class GenerateOBJ {
public:
	static void generateObjectFile(llvm::Module* module, const std::string& outputFilename);
};

#endif // GEN_OBJ_FILE_H