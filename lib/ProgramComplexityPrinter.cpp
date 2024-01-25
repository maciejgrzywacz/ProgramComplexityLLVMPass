
#include "ProgramComplexity.h"
#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/JSON.h"

#include <iostream>

using namespace llvm;

#define DEBUG_TYPE "program-complexity"

AnalysisKey ProgramComplexityPrinterPass::Key;

PreservedAnalyses
ProgramComplexityPrinterPass::run(Function &F, FunctionAnalysisManager &AM) {
  ProgramComplexity::Result result = AM.getResult<ProgramComplexity>(F);

  json::OStream JOS(OS, /*PrettyPrint*/ 1);
  JOS.value(std::move(result->makeJson()));
  OS << '\n';

  return PreservedAnalyses::all();
}

extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "ProgramComplexityPrinterPass", "v0.1",
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "program-complexity") {
                    FPM.addPass(ProgramComplexityPrinterPass(dbgs()));
                    return true;
                  }
                  return false;
                });
            PB.registerAnalysisRegistrationCallback(
                [](FunctionAnalysisManager &FAM) {
                  FAM.registerPass([&] { return ProgramComplexity(); });
                });
          }};
}
