#pragma once

#include <vector>
#include <map>
#include <set>
#include <string>
#include <iostream>
#include <sstream>
#include <cassert>
#include <memory>
#include "llvm/Support/JSON.h"

#include "llvm/Support/Debug.h"

#define UNDEF_VALUE "Undef"

namespace ProgramInfo {

static std::string indent(int indent_level = 0) {
    std::string indent = "";
    for (int i = 0; i < indent_level; i++) indent += "\t";
    return indent;
};

struct DebugLocation {
    std::string line = "Undef";
    std::string column = "Undef";

    llvm::json::Object makeJson() const {
        llvm::json::Object dbgJson;
        dbgJson["line"] = line;
        dbgJson["column"] = column;
        return dbgJson;
    }
};

struct DebugVariableInfo {
    std::string irSymbolName = "Undef";
    std::string codeVariableName = "Undef";
    std::string line = "Undef";

    llvm::json::Object makeJson() const {
    llvm::json::Object dbgJson;
    dbgJson["LLVM_IR_name"] = irSymbolName;
    dbgJson["source_code_name"] = codeVariableName;
    dbgJson["line"] = line;
    return dbgJson;
    }
};

// classes and structures
struct ProgramPart {
    std::vector<std::shared_ptr<ProgramPart>> children;
    std::string name;

    virtual ~ProgramPart() = default;

    void setName(std::string ppName) {
        name = ppName;
    }

    void addChild(std::shared_ptr<ProgramPart> pp) {
        children.push_back(pp);
    }

    virtual void print(std::stringstream &ss, int indent_level = 0) = 0;
    virtual llvm::json::Object makeJson() const = 0;
};

struct Function : ProgramPart {
    struct Argument {
        std::string name;
        std::string type;
    };
    std::vector<Argument> arguments;

    void addArgument(std::string name, std::string type) {
        Argument a;
        a.name = name;
        a.type = type;
        arguments.push_back(a);
    }

    // friend class FunctionInfoHelper;

    // bool operator=(const Function &o) const {
    //     return name.compare(o.name) == 0;
    // }

    bool operator<(const Function &o) const {
        return (name.compare(o.name) < 1) ? true : false;
    }

    void print(std::stringstream &ss, int indent_level = 0) override {
        ss << indent(indent_level) << "Function " << name << "\n";
        ss << indent(indent_level) << "arguments: " << "\n";
        for (Argument const &a : arguments) {
            ss << indent(indent_level + 1) << a.type << ": " << a.name << "\n";
        }

        for (auto const &pp : children) {
            pp->print(ss, indent_level + 1);
        }
    }

    llvm::json::Object makeJson() const override {
        llvm::json::Object functionJson;

        functionJson["type"] = "function";
        functionJson["name"] = name;

        llvm::json::Array argumentsJson;
        for (Argument const &a : arguments) {
            llvm::json::Object argJson;
            argJson["name"] = a.name;
            argJson["type"] = a.type;
            argumentsJson.push_back(std::move(argJson));
        }
        functionJson["arguments"] = std::move(argumentsJson);

        if (!children.empty()) {
            llvm::json::Array childrenJson;
            for (std::shared_ptr<ProgramPart> const &pp : children) {
                childrenJson.push_back(pp->makeJson());
            }
            functionJson["children"] = std::move(childrenJson);
        }
        return functionJson;
    }
};

struct Block : ProgramPart {
    std::map<std::string, std::string> successors;
    std::map<std::string, unsigned int> instructions;
    std::vector<Function> callInstructions;
    DebugLocation terminatorDbgLocation;

    void addInstruction(std::string name) {
        instructions[name]++;
    }

    void addCallInstruction(Function f) {
        callInstructions.push_back(f);
    }

    void addSuccessor(std::string blockName, std::string probability) {
        successors[blockName] = probability;
    }

    void addTerminatorDbgLocation(unsigned int line, unsigned int column) {
        DebugLocation dl;
        if (line != 0) {
            dl.line = std::to_string(line);
        }
        if (column != 0) {
            dl.column = std::to_string(column);
        }
        terminatorDbgLocation = dl;
    }

    void print(std::stringstream &ss, int indent_level = 0) override {
        ss << indent(indent_level) << "Basic Block " << name << "\n";
        ss << indent(indent_level) << "instructions: " << "\n";
        for (auto const& [name, count] : instructions) {
            ss << indent(indent_level + 1) << name << ": " << count << "\n";
        }
    }

    llvm::json::Object makeJson() const override {
        llvm::json::Object bbJson;

        bbJson["type"] = "basic block";
        bbJson["name"] = name;

        llvm::json::Array instructionsJson;
        for (auto const& [name, count] : instructions) {
            llvm::json::Object instJson;
            instJson["instruction"] = name;
            instJson["count"] = count;
            instructionsJson.push_back(std::move(instJson));
        }
        bbJson["instructions"] = std::move(instructionsJson);

        llvm::json::Array callsJson;
        for (Function const &function : callInstructions) {
            llvm::json::Object functionJson;
            functionJson["function"] = std::move(function.makeJson());

            callsJson.push_back(std::move(functionJson));
        }
        bbJson["function calls"] = std::move(callsJson);

        llvm::json::Array successorsJson;
        for (auto const& [succ, probab] : successors) {
            llvm::json::Object succJson;
            succJson["successor"] = succ;
            succJson["probability"] = probab;

            successorsJson.push_back(std::move(succJson));
        }
        bbJson["successors"] = std::move(successorsJson);

        bbJson["terminator_dbg_location"] = std::move(terminatorDbgLocation.makeJson());

        return bbJson;
    }
};

struct Loop : ProgramPart {
    std::string iterations;
    std::vector<DebugVariableInfo> iterationsDebugInfo;


    void setIterationCount(std::string iterationCount) {
        iterations = iterationCount;
    }

    void setIterationDebugInfo(std::vector<DebugVariableInfo> &iterationsDebugInfo) {
        this->iterationsDebugInfo = iterationsDebugInfo;
    }

    virtual void print(std::stringstream &ss, int indent_level = 0) override {

    }

    llvm::json::Object makeJson() const override {
        llvm::json::Object loopJson;

        loopJson["type"] = "loop";
        loopJson["name"] = name;
        loopJson["iterations"] = iterations;

        llvm::json::Array iterationsDbgInfoJson;
        for(DebugVariableInfo i : iterationsDebugInfo) {
            iterationsDbgInfoJson.push_back(std::move(i.makeJson()));
        }
        loopJson["iterations_debug_info"] = std::move(iterationsDbgInfoJson);

        if (!children.empty()) {
            llvm::json::Array childrenJson;
            for (std::shared_ptr<ProgramPart> const &pp : children) {
                childrenJson.push_back(pp->makeJson());
            }
            loopJson["children"] = std::move(childrenJson);
        }

        return loopJson;
    }
};

}
