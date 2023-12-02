
#ifndef PROGRAMCOMPLEXITY_H
#define PROGRAMCOMPLEXITY_H

#include "llvm/IR/PassManager.h"
#include "llvm/Analysis/TargetTransformInfo.h"

class ScalarEvolution;

namespace json {
  class Object;
}

class ProgramComplexity : public AnalysisInfoMixin<ProgramComplexity> {
  friend AnalysisInfoMixin<ProgramComplexity>;
  static AnalysisKey Key;

  TargetTransformInfo::TargetCostKind TargetCostKind = TargetTransformInfo::TCK_RecipThroughput;

public:
  typedef json::Object Result;

  Result run(Module &M, ModuleAnalysisManager &AM);

private:

  json::Object handleLoop(const Loop &L, ScalarEvolution &SE, TargetTransformInfo &TTI);
  json::Object handleBranchBB(BasicBlock &BB, TargetTransformInfo &TTI);
  json::Object handleBB(BasicBlock &BB, TargetTransformInfo &TTI);
};

/// Printer pass for the \c ProgramComplexity results.
class ProgramComplexityPrinterPass : public PassInfoMixin<ProgramComplexityPrinterPass> {
  raw_ostream &OS;

public:
  explicit ProgramComplexityPrinterPass(raw_ostream &OS) : OS(OS) {}

  PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM);
};

#endif // PROGRAMCOMPLEXITY_H