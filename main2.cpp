#include "/opt/homebrew/Cellar/llvm/17.0.6/include/clang/AST/AST.h"
#include "/opt/homebrew/Cellar/llvm/17.0.6/include/clang/AST/ASTConsumer.h"
#include "/opt/homebrew/Cellar/llvm/17.0.6/include/clang/AST/ASTContext.h"
#include "/opt/homebrew/Cellar/llvm/17.0.6/include/clang/AST/RecursiveASTVisitor.h"
#include "/opt/homebrew/Cellar/llvm/17.0.6/include/clang/Driver/Options.h"
#include "/opt/homebrew/Cellar/llvm/17.0.6/include/clang/Frontend/ASTConsumers.h"
#include "/opt/homebrew/Cellar/llvm/17.0.6/include/clang/Frontend/CompilerInstance.h"
#include "/opt/homebrew/Cellar/llvm/17.0.6/include/clang/Frontend/FrontendActions.h"
#include "/opt/homebrew/Cellar/llvm/17.0.6/include/clang/Rewrite/Core/Rewriter.h"
#include "/opt/homebrew/Cellar/llvm/17.0.6/include/clang/Tooling/CommonOptionsParser.h"
#include "/opt/homebrew/Cellar/llvm/17.0.6/include/clang/Tooling/Tooling.h"

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
