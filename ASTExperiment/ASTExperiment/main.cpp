//#include "/opt/homebrew/Cellar/llvm/17.0.6/include/clang/AST/AST.h"
//#include "/opt/homebrew/Cellar/llvm/17.0.6/include/clang/AST/ASTConsumer.h"
//#include "/opt/homebrew/Cellar/llvm/17.0.6/include/clang/AST/ASTContext.h"
//#include "/opt/homebrew/Cellar/llvm/17.0.6/include/clang/AST/RecursiveASTVisitor.h"
//#include "/opt/homebrew/Cellar/llvm/17.0.6/include/clang/Driver/Options.h"
//#include "/opt/homebrew/Cellar/llvm/17.0.6/include/clang/Frontend/ASTConsumers.h"
//#include "/opt/homebrew/Cellar/llvm/17.0.6/include/clang/Frontend/CompilerInstance.h"
//#include "/opt/homebrew/Cellar/llvm/17.0.6/include/clang/Frontend/FrontendActions.h"
//#include "/opt/homebrew/Cellar/llvm/17.0.6/include/clang/Rewrite/Core/Rewriter.h"
//#include "/opt/homebrew/Cellar/llvm/17.0.6/include/clang/Tooling/CommonOptionsParser.h"
//#include "/opt/homebrew/Cellar/llvm/17.0.6/include/clang/Tooling/Tooling.h"

#include "/Users/bill/Library/Developer/Toolchains/LLVM18.0.0git.xctoolchain/usr/include/clang/AST/AST.h"
#include "/Users/bill/Library/Developer/Toolchains/LLVM18.0.0git.xctoolchain/usr/include/clang/AST/ASTConsumer.h"
#include "/Users/bill/Library/Developer/Toolchains/LLVM18.0.0git.xctoolchain/usr/include/clang/AST/ASTContext.h"
#include "/Users/bill/Library/Developer/Toolchains/LLVM18.0.0git.xctoolchain/usr/include/clang/AST/RecursiveASTVisitor.h"
#include "/Users/bill/Library/Developer/Toolchains/LLVM18.0.0git.xctoolchain/usr/include/clang/Driver/Options.h"
#include "/Users/bill/Library/Developer/Toolchains/LLVM18.0.0git.xctoolchain/usr/include/clang/Frontend/ASTConsumers.h"
#include "/Users/bill/Library/Developer/Toolchains/LLVM18.0.0git.xctoolchain/usr/include/clang/Frontend/CompilerInstance.h"
#include "/Users/bill/Library/Developer/Toolchains/LLVM18.0.0git.xctoolchain/usr/include/clang/Frontend/FrontendActions.h"
#include "/Users/bill/Library/Developer/Toolchains/LLVM18.0.0git.xctoolchain/usr/include/clang/Rewrite/Core/Rewriter.h"
#include "/Users/bill/Library/Developer/Toolchains/LLVM18.0.0git.xctoolchain/usr/include/clang/Tooling/CommonOptionsParser.h"
#include "/Users/bill/Library/Developer/Toolchains/LLVM18.0.0git.xctoolchain/usr/include/clang/Tooling/Tooling.h"


//ModularizationTools/generate-xcode-compilation-database.sh xcodebuild -resolvePackageDependencies NO -workspace ForeFlightMobile.xcworkspace -scheme ForeFlight -destination generic/platform=iOS -toolchain org.llvm.14.0.6 COMPILER_INDEX_STORE_ENABLE=NO
//org.llvm.17.0.6
//xcodebuild -scheme HelloCommandLine -toolchain org.llvm.17.0.6 COMPILER_INDEX_STORE_ENABLE=NO
//org.llvm.14.0.6
//org.llvm.15.0.7

//Checking C++ version used with clang
///opt/homebrew/Cellar/llvm/17.0.6/bin/clang++ -dM -E -x c++ /dev/null | grep __cplusplus // - 201703L
///clang++ -dM -E -x c++ /dev/null | grep __cplusplus // #define __cplusplus 199711L
///

/*
 c++98 or c++03: Support for the C++ 1998 or 2003 standard.
 c++11: Support for the C++ 2011 standard.
 c++14: Support for the C++ 2014 standard.
 c++17: Support for the C++ 2017 standard.
 c++20: Support for the C++ 2020 standard.
 */

using namespace clang;
using namespace clang::tooling;

class FindNamedCallVisitor : public RecursiveASTVisitor<FindNamedCallVisitor> {
 public:
  explicit FindNamedCallVisitor(ASTContext *Context, std::string fName)
      : Context(Context), fName(fName) {}

  bool VisitCallExpr(CallExpr *CallExpression) {
    QualType q = CallExpression->getType();
    const Type *t = q.getTypePtrOrNull();

    if (t != NULL) {
      FunctionDecl *func = CallExpression->getDirectCallee();
      const std::string funcName = func->getNameInfo().getAsString();
      if (fName == funcName) {
        FullSourceLoc FullLocation =
            Context->getFullLoc(CallExpression->getBeginLoc());
        if (FullLocation.isValid())
          llvm::outs() << "Found call at "
                       << FullLocation.getSpellingLineNumber() << ":"
                       << FullLocation.getSpellingColumnNumber() << "\n";
      }
    }

    return true;
  }

 private:
  ASTContext *Context;
  std::string fName;
};

class FindNamedCallConsumer : public clang::ASTConsumer {
 public:
  explicit FindNamedCallConsumer(ASTContext *Context, std::string fName)
      : Visitor(Context, fName) {}

  virtual void HandleTranslationUnit(clang::ASTContext &Context) {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

 private:
  FindNamedCallVisitor Visitor;
};

class FindNamedCallAction : public clang::ASTFrontendAction {
 public:
  FindNamedCallAction() {}

  virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
      clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
    const std::string fName = "doSomething";
    return std::unique_ptr<clang::ASTConsumer>(
        new FindNamedCallConsumer(&Compiler.getASTContext(), fName));
  }
};

static llvm::cl::OptionCategory MyToolCategory("My tool options");

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
  ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());
  // run the Clang Tool, creating a new FrontendAction (explained below)
  int result = Tool.run(newFrontendActionFactory<FindNamedCallAction>().get());
  return result;
}
