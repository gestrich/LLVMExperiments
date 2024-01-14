#include <iostream>
using namespace std;

#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Driver/Options.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/CompilationDatabase.h"
#include "clang/Tooling/Refactoring.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;

static llvm::cl::OptionCategory MyToolCategory("My tool options");

void print(const std::vector<CompileCommand> &Commands) {
  if (Commands.empty()) {
    return;
  }
  for (auto opt : Commands[0].CommandLine) {
    llvm::errs() << "\t" << opt << "\n";
  }
}

int main(int argc, const char **argv) {
  const std::string fName = "doSomething";

  auto OptionsParserExpected = CommonOptionsParser::create(argc, argv, MyToolCategory);
  if (!OptionsParserExpected) {
    // Handle error: OptionsParserExpected contains a description of the error.
    llvm::errs() << "Error while creating CommonOptionsParser: "
                 << llvm::toString(OptionsParserExpected.takeError()) << "\n";
    return 1;
  }

  CommonOptionsParser &OptionsParser = *OptionsParserExpected;

 CompilationDatabase &Compilations = OptionsParser.getCompilations();

  for (const auto &File :
       OptionsParser.getSourcePathList()) {
    llvm::errs() << "filename: " << File << "\n";
    llvm::errs() << "opts:\n";
    auto Commands = Compilations.getCompileCommands(File);
    print(Commands);
  }

  ClangTool Tool(OptionsParser.getCompilations(),
                   OptionsParser.getSourcePathList());

    auto Matcher = cxxRecordDecl(hasName("SimpleClass")).bind("class");

    class MyClassPrinter : public clang::ast_matchers::MatchFinder::MatchCallback {
    public:
        virtual void run(const MatchFinder::MatchResult &Result) {
            if (const CXXRecordDecl *RD = Result.Nodes.getNodeAs<CXXRecordDecl>("class")) {
                RD->dump();  // Or perform other actions with the matched node.
            }
        }
    };

    MyClassPrinter Printer;
    MatchFinder Finder;
    Finder.addMatcher(Matcher, &Printer);
    
    return Tool.run(newFrontendActionFactory(&Finder).get());
}