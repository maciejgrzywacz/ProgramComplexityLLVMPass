
#include "ProgramComplexity.h"

#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/Analysis/BranchProbabilityInfo.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/ScalarEvolutionExpressions.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/ValueSymbolTable.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/JSON.h"

#include <cassert>

using namespace llvm;

#define DEBUG_TYPE "program-complexity"

AnalysisKey ProgramComplexity::Key;

static cl::opt<bool> UseBranchProbability(
    "use-branch-probability", cl::init(false), cl::Hidden,
    cl::desc("ProgramComplexity: Use branch probability info form "
             "BranchProbabilityAnalysis, for branches probabilities."));

void ProgramComplexity::createDebugInfoMap() {
  DebugInfoFinder dif;

  DISubprogram *sp = F->getSubprogram();
  assert(sp && "input LLVM IR has to be compiled in debug mode (clang -g option, debug llvm build)");

  dif.processSubprogram(sp);

  // Source file checksum
  assert(dif.compile_unit_count() == 1 && "Not only compile unit");
  for(DICompileUnit *cu : dif.compile_units()) {
    DIFile *file = dyn_cast<DIFile>(cu->getOperand(0));
    sourceFileChecksum = file->getChecksum()->Value;
  }

  // local variables
  for (DILocalVariable *lv : dif.local_variables()) {
    MetadataAsValue *mav = MetadataAsValue::getIfExists(F->getContext(), lv);
    if (!mav) {
      // TODO: check this case when it happens and decide what to do. Probably just continue.
      assert(0 && "Can't get Value from DILocalVariable.");
      //continue;
    }

    for(Value* mavUser : mav->users()) {
      if(DbgInfoIntrinsic *dii = dyn_cast<DbgInfoIntrinsic>(mavUser)) {
        Value *v = dii->getOperand(0);
        debugValueMap[v] = ValueDebugInfo{lv->getName(), lv->getLine()};
        // dbgs() << "Assigning local variable: " << v->getNameOrAsOperand() << " with code variable named: "
        //        << lv->getName() << ", line: " << lv->getLine() << "\n";
      }
      else {
        assert(false && "User of metadata should be a debug info intrinsic instruction.");
      }
    }
  }

  // For global variables we have to have access to module, which means this has to be converted into a module pass.
  /*
  for (auto Global = F->getModule() M->getModule()->global_begin();
              Global != M->getModule()->global_end();
              ++Global)

  for (DIGlobalVariableExpression *gve : dif.global_variables()) {
    DIGlobalVariable *gv = gve->getVariable();
    if (!gv) {
      //TODO: What that this case represent, should it be handled? Or just continue?
      assert(0 && "Global variable expression does not have a variable.");
    }

    MetadataAsValue *mav = MetadataAsValue::getIfExists(F->getContext(), gve);
    if (!mav) {
      // TODO: check this case when it happens and decide what to do. Probably just continue.
      // assert(0 && "Can't get Value from DIGlobalVariable.");
      //continue;

    }

    // This does not work as debug metadata is assigned directly to global variables, not by llvm.dbg functions.
    // for(Value* mavUser : mav->users()) {
    //   if(DbgInfoIntrinsic *dii = dyn_cast<DbgInfoIntrinsic>(mavUser)) {
    //     Value *v = dii->getOperand(0);
    //     dbgs() << "Assigning global variable: " << v->getNameOrAsOperand() << " with code variable named: "
    //            << gv->getName() << ", line: " << gv->getLine() << "\n";
    //   }
    //   else {
    //     assert(false && "User of metadata should be a debug info intrinsic instruction.");
    //   }
    // }
  }
  */
}

void ProgramComplexity::trackValue(Value* inst) {
  // Traverse IR in reverse to track given variable dependencies
}

ProgramComplexity::Result ProgramComplexity::run(Function &F,
                                                 FunctionAnalysisManager &AM) {
  this->F = &F;
  // Get required analysis
  BPI = &AM.getResult<BranchProbabilityAnalysis>(F);
  LI = &AM.getResult<LoopAnalysis>(F);
  SE = &AM.getResult<ScalarEvolutionAnalysis>(F);

  createDebugInfoMap();

  LLVM_DEBUG(
    dbgs() << "FUNCTION: " << F.getName() << "( ";
    for (Argument &A : F.args()) {
      std::string str;
      llvm::raw_string_ostream output(str);
      A.getType()->print(dbgs());
      dbgs() << ": " << A.getName() << ", ";
    }
    dbgs() << ")\n";
  );

  std::shared_ptr<ProgramInfo::Function> infoFunction = std::make_shared<ProgramInfo::Function>();
  infoFunction->setName(F.getNameOrAsOperand());
  for (Argument &A : F.args()) {
    std::string typeStr;
    llvm::raw_string_ostream OS(typeStr);
    A.getType()->print(OS);

    infoFunction->addArgument(A.getNameOrAsOperand(), typeStr);
  }

  // Handle loops and their BBs
  for (Loop *loop : LI->getTopLevelLoops()) {
    infoFunction->addChild((handleLoop(*loop)));
  }

  // Handle free BBs
  for (BasicBlock &BB : F) {
    if (visitedBlocks.count(&BB)) {
      // LLVM_DEBUG(dbgs() << "Skipping visited bb: " << BB.getName() << "\n");
      continue;
    }

    std::shared_ptr<ProgramInfo::Block> b = handleBB(BB);
    if (b) infoFunction->addChild(b);
  }

  return infoFunction;
}

void ProgramComplexity::simplifyScev(const SCEV* scev) {
    std::string scevStr;
    llvm::raw_string_ostream OS(scevStr);

    for (const SCEV* s : scev->operands()) {
      if (s->getExpressionSize() == 1) {
        if (const SCEVUnknown *unknownS = dyn_cast<SCEVUnknown>(s)) {
          // This part of SCEV is a variable in IR
          Value* val = unknownS->getValue();

          if (Instruction *inst = dyn_cast<Instruction>(val)) {
            scevStr = "";
            inst->printAsOperand(OS);
          }
        }
      }
      else {
        simplifyScev(s);
      }
    }
}

std::shared_ptr<ProgramInfo::Loop> ProgramComplexity::handleLoop(const Loop &L) {
  // See example ScalarEvolution.cpp:13361
  StringRef loopName = L.getName();
  LLVM_DEBUG(dbgs() << "Handling loop: " << loopName << "\n");

  std::shared_ptr<ProgramInfo::Loop> infoLoop = std::make_shared<ProgramInfo::Loop>();
  infoLoop->setName(loopName.str());

  if (!L.isInnermost()) {
    // Handle nested loops. This is done before handling BBs in loop, to not go through BBs from nested loops twice
    for (Loop *subLoop : L.getSubLoops()) {
      infoLoop->addChild(handleLoop(*subLoop));
    }
  }

  if (SE->hasLoopInvariantBackedgeTakenCount(&L)) {
    const SCEV *backedgeTakenCount = SE->getBackedgeTakenCount(&L);
    LLVM_DEBUG(dbgs() << "Loop iteration count: " << *backedgeTakenCount << "\n");
    std::string scevStr;
    llvm::raw_string_ostream OS(scevStr);
    backedgeTakenCount->print(OS);
    infoLoop->setIterationCount(scevStr);

    simplifyScev(backedgeTakenCount);
  }
  else {
    LLVM_DEBUG(dbgs() << "Loop iteration count: undef\n");
    infoLoop->setIterationCount("Undef");
  }

  // Go through basic blocks in loop
  LLVM_DEBUG(dbgs() << "Handling BBs in loop: " << loopName << "\n");
  for (BasicBlock *BB : L.getBlocks()) {
    if (visitedBlocks.count(BB)) {
      continue;
    }

    infoLoop->addChild(handleBB(*BB));
  }

  return infoLoop;
}

std::shared_ptr<ProgramInfo::Block> ProgramComplexity::handleBB(BasicBlock &BB) {
  std::string bbName = BB.getNameOrAsOperand();

  std::shared_ptr<ProgramInfo::Block> infoBlock = std::make_shared<ProgramInfo::Block>();
  infoBlock->setName(bbName);

  // Handle instructions inside block
  for (Instruction &Inst : BB) {
    if (CallInst *callInst = dyn_cast<CallInst>(&Inst)) {
      // Handle call instruction

      // Ignore annotation intrinsics
      if (IntrinsicInst *iInst = dyn_cast<IntrinsicInst>(callInst)){
        if (iInst->isAssumeLikeIntrinsic()) {
          continue;
        }
      }

      ProgramInfo::Function f;
      f.setName(callInst->getCalledFunction()->getNameOrAsOperand());

      // special case for call instruction - tell which function is called
      for (unsigned int i = 0; i < callInst->getNumOperands() - 1; i++) {
        std::string result;
        raw_string_ostream OS(result);
        callInst->getOperand(i)->getType()->print(OS, true);

        f.addArgument(
          callInst->getOperand(i)->getNameOrAsOperand(),
          result);
      }
      infoBlock->addCallInstruction(f);

    } else {
      infoBlock->addInstruction(Inst.getOpcodeName());
    }
  }

  LLVM_DEBUG(
    dbgs() << "instruction in bb:\n";
    for(auto const& [name, count] : infoBlock->instructions) {
      dbgs() << name << ": " << count << "\n";
    }
  );

  LLVM_DEBUG(
    dbgs() << "calls in bb:\n";
    for(ProgramInfo::Function function : infoBlock->callInstructions) {
      dbgs() << function.name << "( ";
      for(ProgramInfo::Function::Argument arg : function.arguments) {
        dbgs() << arg.type << ": " << arg.name << ", ";
      }
      dbgs() << ")\n";
    }
  );

  // Handle block successors
  Instruction* blockTerminator = BB.getTerminator();
  if (blockTerminator) {
    // Track variables steering block successors
    if (BranchInst *bi = dyn_cast<BranchInst>(blockTerminator)) {
      if (bi->isConditional()) {
        Value *op1Value = bi->getOperand(0);
        // trackValue(op1Value);
      }
    }

    if (blockTerminator->getNumSuccessors() == 1) {
      std::string successorName =
          blockTerminator->getSuccessor(0)->getNameOrAsOperand();

      infoBlock->addSuccessor(successorName, "1");
    }

    if (UseBranchProbability) {
      // Use default compiler method, generating percent probabilities
      // of going to each successor to estimate branch probability
      for (BasicBlock *sucBB : successors(&BB)) {
        auto EP = BPI->getEdgeProbability(&BB, sucBB);
        double prob = double(EP.getNumerator()) / EP.getDenominator();

        infoBlock->addSuccessor(sucBB->getNameOrAsOperand(), std::to_string(prob));
      }
    }
    else {
      // Use method introducing additional variable based on which we define
      // branch probability
      if (blockTerminator->getNumSuccessors() == 2) {
        // For 2 successors, first successor probability is x, second one i 1-x
        std::string firstSuccessorName =
            blockTerminator->getSuccessor(0)->getNameOrAsOperand();
        std::string secondSuccessorName =
            blockTerminator->getSuccessor(1)->getNameOrAsOperand();

        std::string firstSuccessorVariable = "BranchProbability_" +
          bbName + "_" + firstSuccessorName;
        std::string secondSuccessorVariable = std::string("(1 - ").append(firstSuccessorVariable).append(")");

        infoBlock->addSuccessor(firstSuccessorName,
          firstSuccessorVariable);
        infoBlock->addSuccessor(secondSuccessorName,
          secondSuccessorVariable);
      }
      else {
        // For multiple successors (ex. for swich terminator) assign separate
        // branch probability variable to each branch
        for(unsigned int i = 0; i < blockTerminator->getNumSuccessors(); i++) {
          BasicBlock* successor = blockTerminator->getSuccessor(i);
          std::string successorName = successor->getNameOrAsOperand();
          std::string successorVariable = "BranchProbability_" + bbName +
            "_" + successorName;

          infoBlock->addSuccessor(successorName, successorVariable);
        }
      }
    }

    // Add metadata about block successors taken condition
    MDNode *dbgMD = blockTerminator->getMetadata("dbg");
    if (dbgMD) {
      unsigned int dbgLine = 0;
      unsigned int dbgColumn = 0;
      if (DILocation *loc = dyn_cast<DILocation>(dbgMD)) {
          dbgLine = loc->getLine();
          dbgColumn = loc->getColumn();
      }

      infoBlock->addTerminatorDbgLocation(dbgLine, dbgColumn);
    }
  }

  // Mark block as visited
  visitedBlocks.insert(&BB);

  return infoBlock;
}
