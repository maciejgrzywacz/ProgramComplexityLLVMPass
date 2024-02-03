
#ifndef PROGRAMCOMPLEXITY_H
#define PROGRAMCOMPLEXITY_H

#include "FunctionInfo.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include <set>
#include <vector>

namespace llvm {
class BranchProbabilityInfo;
class LoopInfo;
class ScalarEvolution;
class SCEV;
class Function;

namespace json {
class Object;
}
} // namespace llvm

class ProgramComplexity : public llvm::AnalysisInfoMixin<ProgramComplexity> {
  friend llvm::AnalysisInfoMixin<ProgramComplexity>;
  static llvm::AnalysisKey Key;

public:
  typedef std::shared_ptr<ProgramInfo::Function> Result;

  Result run(llvm::Function &F, llvm::FunctionAnalysisManager &AM);

private:
  llvm::BranchProbabilityInfo *BPI;
  llvm::LoopInfo *LI;
  llvm::ScalarEvolution *SE;

  llvm::Function* F;
  std::set<llvm::BasicBlock *> visitedBlocks;

  llvm::StringRef sourceFileChecksum;
  // struct ValueDebugInfo {
  //   llvm::StringRef sourceName;
  //   unsigned int lineNumber;

  // };
  std::vector<llvm::Value*> functionArguments;
  std::map<std::string, ProgramInfo::DebugVariableInfo> debugValueMap;

  void createDebugInfoMap();
  void trackValue(llvm::Value* val);
  std::vector<ProgramInfo::DebugVariableInfo> getScevDebugInfo(const llvm::SCEV *s);
  std::vector<ProgramInfo::DebugVariableInfo> getScevDebugInfo(const llvm::SCEV *s, std::vector<ProgramInfo::DebugVariableInfo> &iterationsDebugInfo);
  std::shared_ptr<ProgramInfo::Loop> handleLoop(const llvm::Loop &L);
  std::shared_ptr<ProgramInfo::Block> handleBB(llvm::BasicBlock &BB);
};

/// Printer pass for the \c ProgramComplexity results.
class ProgramComplexityPrinterPass
    : public llvm::PassInfoMixin<ProgramComplexityPrinterPass> {
  llvm::raw_ostream &OS;
  static llvm::AnalysisKey Key;

public:
  explicit ProgramComplexityPrinterPass(llvm::raw_ostream &OS) : OS(OS) {}

  llvm::PreservedAnalyses run(llvm::Function &F,
                              llvm::FunctionAnalysisManager &AM);
};

#endif // PROGRAMCOMPLEXITY_H
