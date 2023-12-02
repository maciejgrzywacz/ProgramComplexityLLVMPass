
#include "llvm/Analysis/ProgramComplexity.h"
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

using namespace llvm;

#define DEBUG_TYPE "program-complexity"

AnalysisKey ProgramComplexity::Key;

ProgramComplexity::Result ProgramComplexity::run(Module &M,
                                                 ModuleAnalysisManager &AM) {

  json::Object mJson;

  json::Array functions;
  for (Function &F : M) {
    // Get required analysis
    FunctionAnalysisManager &FAM =
        AM.getResult<FunctionAnalysisManagerModuleProxy>(M).getManager();
    ScalarEvolution &SE = FAM.getResult<ScalarEvolutionAnalysis>(F);
    TargetTransformInfo &TTI = FAM.getResult<TargetIRAnalysis>(F);
    auto &LI = FAM.getResult<LoopAnalysis>(F);

    json::Object functionJson;
    functionJson["name"] = F.getName();
    functionJson["type"] = "function";

    /*
    dbgs() << "Classifying expressions for: ";
    F.printAsOperand(dbgs(), false);
    dbgs() << "\n";
    for (Instruction &I : instructions(F)) {
      if (I.getType()->isIntOrPtrTy() && !isa<CmpInst>(I)) {
        dbgs() << I << '\n';
        dbgs() << "  -->  ";
        const SCEV *SV = SE.getSCEV(&I);
        SV->print(dbgs());
        if (!isa<SCEVCouldNotCompute>(SV)) {
          dbgs() << " U: ";
          SE.getUnsignedRange(SV).print(dbgs());
          dbgs() << " S: ";
          SE.getSignedRange(SV).print(dbgs());
        }

        const Loop *L = LI.getLoopFor(I.getParent());

        const SCEV *AtUse = SE.getSCEVAtScope(SV, L);
        if (AtUse != SV) {
          dbgs() << "  -->  ";
          AtUse->print(dbgs());
          if (!isa<SCEVCouldNotCompute>(AtUse)) {
            dbgs() << " U: ";
            SE.getUnsignedRange(AtUse).print(dbgs());
            dbgs() << " S: ";
            SE.getSignedRange(AtUse).print(dbgs());
          }
        }

        if (L) {
          dbgs() << "\t\t" "Exits: ";
          const SCEV *ExitValue = SE.getSCEVAtScope(SV, L->getParentLoop());
          if (!SE.isLoopInvariant(ExitValue, L)) {
            dbgs() << "<<Unknown>>";
          } else {
            dbgs() << *ExitValue;
          }

          bool First = true;
          for (const auto *Iter = L; Iter; Iter = Iter->getParentLoop()) {
            if (First) {
              dbgs() << "\t\t" "LoopDispositions: { ";
              First = false;
            } else {
              dbgs() << ", ";
            }

            Iter->getHeader()->printAsOperand(dbgs(), false);
            dbgs() << ": " << SE.getLoopDisposition(SV, Iter);
          }

          for (const auto *InnerL : depth_first(L)) {
            if (InnerL == L)
              continue;
            if (First) {
              dbgs() << "\t\t" "LoopDispositions: { ";
              First = false;
            } else {
              dbgs() << ", ";
            }

            InnerL->getHeader()->printAsOperand(dbgs(), false);
            dbgs() << ": " << SE.getLoopDisposition(SV, InnerL);
          }

          dbgs() << " }";
        }

        dbgs() << "\n";
      }
    }
    */

    // Handle loops
    json::Array loopsJsonArray;
    for (Loop *loop : LI.getTopLevelLoops()) {
      // iterating top loops
      loopsJsonArray.push_back(std::move(handleLoop(*loop, SE, TTI)));
    }
    functionJson["loops"] = json::Value(std::move(loopsJsonArray));

    // Handle free BBs
    json::Array freeBBsJsonArray;
    bool skip = false;
    for (BasicBlock &BB : F) {
      // Skip blocks belonging to a loop
      for (Loop *L : LI) {
        if (L->contains(&BB)) {
          LLVM_DEBUG(dbgs() << "Ignoring bb: " << BB.getName()
                 << ",as it is part of loop: " << L->getName() << "\n");
          skip = true;
        }
      }
      if (skip == true) {
        skip = false;
        continue;
      }

      // Handle blocks with branch
      if (BB.getTerminator()->getNumSuccessors() > 1) {
        // TODO: make a list of handled bbs, to not iterate them again ?
      }

      freeBBsJsonArray.push_back(std::move(handleBB(BB, TTI)));
    }

    functionJson["basic blocks"] = json::Value(std::move(freeBBsJsonArray));
    functions.push_back(std::move(functionJson));
  }

  mJson["program"] = json::Value(std::move(functions));
  return mJson;
}

json::Object ProgramComplexity::handleLoop(const Loop &L, ScalarEvolution &SE, TargetTransformInfo &TTI) {
  StringRef loopName = L.getName();
  dbgs() << "Handling loop: " << loopName << ", header: ";
  L.getHeader()->printAsOperand(dbgs(), /*PrintType=*/false);
  dbgs() << "\n";

  json::Object loopJson;
  loopJson["name"] = loopName;
  loopJson["type"] = "loop";

  if (!L.isInnermost()) {
    // TODO: implement this
    // handle subloops
    for (Loop *subLoop : L.getSubLoops()) {
      handleLoop(*subLoop, SE, TTI);
    }
  }

  // determine num of loop iterations (also based on runtime variables)
  // code taken from ScalarEvolution.cpp PrintLoopInfo
  /*
  SmallVector<BasicBlock *, 8> ExitingBlocks;
  L.getExitingBlocks(ExitingBlocks);
  if (ExitingBlocks.size() != 1)
    dbgs() << "<multiple exits> ";
  */

  if (SE.hasLoopInvariantBackedgeTakenCount(&L)) {
    const SCEV *backedgeTakenCount = SE.getBackedgeTakenCount(&L);
    dbgs() << "backedge-taken count is " << *backedgeTakenCount << "\n";

    std::string str;
    raw_string_ostream output(str);
    output << *backedgeTakenCount;
    loopJson["iterations"] = str;
  } else {
    dbgs() << "backedge-taken count is undef\n";
    loopJson["iterations"] = "Undef";
  }

  /*
  if (ExitingBlocks.size() > 1)
    for (BasicBlock *ExitingBlock : ExitingBlocks) {
      dbgs() << "  exit count for " << ExitingBlock->getName() << ": "
        << *SE.getExitCount(&L, ExitingBlock) << "\n";
    }

  dbgs() << "Loop ";
  L.getHeader()->printAsOperand(dbgs(), PrintType=false);
  dbgs() << ": ";

  auto *ConstantBTC = SE.getConstantMaxBackedgeTakenCount(&L);
  if (!isa<SCEVCouldNotCompute>(ConstantBTC)) {
    dbgs() << "constant max backedge-taken count is " << *ConstantBTC;
    if (SE.isBackedgeTakenCountMaxOrZero(&L))
      dbgs() << ", actual taken count either this or zero.";
  } else {
    dbgs() << "Unpredictable constant max backedge-taken count. ";
  }

  dbgs() << "\n"
        "Loop ";
  L.getHeader()->printAsOperand(dbgs(), PrintType=false);
  dbgs() << ": ";

  auto *SymbolicBTC = SE.getSymbolicMaxBackedgeTakenCount(&L);
  if (!isa<SCEVCouldNotCompute>(SymbolicBTC)) {
    dbgs() << "symbolic max backedge-taken count is " << *SymbolicBTC;
    if (SE.isBackedgeTakenCountMaxOrZero(&L))
      dbgs() << ", actual taken count either this or zero.";
  } else {
    dbgs() << "Unpredictable symbolic max backedge-taken count. ";
  }
  dbgs() << "\n";
  */

  // Go through basic blocks in loop
  json::Array loopBasicBlockJsonArray;
  dbgs() << "Handling BBs in loop: " << loopName;
  for (BasicBlock *BB : L.getBlocks()) {
    loopBasicBlockJsonArray.push_back(std::move(handleBB(*BB, TTI)));
  }
  loopJson["basic blocks"] = std::move(loopBasicBlockJsonArray);

  return loopJson;
}


json::Object ProgramComplexity::handleBranchBB(BasicBlock &BB, TargetTransformInfo &TTI) {
  StringRef bbName = BB.getName();
  dbgs() << "Handling branch BB: " << bbName;

  json::Object bbJson;
  bbJson["name"] = bbName;



  return bbJson;
}


json::Object ProgramComplexity::handleBB(BasicBlock &BB, TargetTransformInfo &TTI) {
  StringRef bbName = BB.getName();
  dbgs() << "Handling BB: " << bbName;

  json::Object bbJson;
  bbJson["name"] = bbName;

  // TODO: Maybe to optimize code it is possible to check for InstructionCost
  // validity (.IsValid()), when getting single values and stopping BB
  // evaluation on invalid cost

  InstructionCost totalCost;

  for (Instruction &Inst : BB) {
    // TODO: Use a pass parameter instead of cl::opt CostKind to determine
    // which cost kind to print.
    InstructionCost Cost;
    auto *II = dyn_cast<IntrinsicInst>(&Inst);
    if (II /*&& TypeBasedIntrinsicCost*/) { // TODO: handle this option?
                                            // Example:
                                            // CostModelPrinterPass::run(
      IntrinsicCostAttributes ICA(II->getIntrinsicID(), *II,
                                  InstructionCost::getInvalid(), true);
      Cost = TTI.getIntrinsicInstrCost(ICA, TargetCostKind);
    } else {
      Cost = TTI.getInstructionCost(&Inst, TargetCostKind);
    }

    totalCost += Cost;

    dbgs() << "Estimated cost of inst: " << Inst.getName() << " is: ";
    if (auto CostVal = Cost.getValue())
      dbgs() << *CostVal;
    else
      dbgs() << "Undef";
    dbgs() << "\n";
  }

  if (std::optional<InstructionCost::CostType> CostVal = totalCost.getValue()) {
    bbJson["cost"] = std::move(*CostVal); //*CostVal;
  } else {
    bbJson["cost"] = "Undef";
  }

  return bbJson;
}

PreservedAnalyses ProgramComplexityPrinterPass::run(Module &M,
                                                    ModuleAnalysisManager &AM) {
  json::Object result = AM.getResult<ProgramComplexity>(M);

  json::OStream JOS(OS, /*PrettyPrint*/ 2);
  JOS.value(std::move(result));
  OS << '\n';
  return PreservedAnalyses::all();
}

extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "ProgramComplexity", "v0.1",
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
              [](StringRef PassName, ModulePassManager &PM,
                ArrayRef<PassBuilder::PipelineElement>) {
                if (PassName == "test-pp") {
                  PM.addPass(ProgramComplexityPrinterPass(dbgs()));
                  return true;
                }
                return false;
              });
          }};
}
