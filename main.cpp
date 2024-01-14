
#include <iostream> // Include for input/output

#include "/opt/homebrew/Cellar/llvm/17.0.6/include/clang/Frontend/FrontendPluginRegistry.h"
#include "/opt/homebrew/Cellar/llvm/17.0.6/include/clang/Frontend/FrontendAction.h" //PluginASTAction
#include "/opt/homebrew/Cellar/llvm/17.0.6/include/clang/Frontend/FrontendPluginRegistry.h"


#include "/opt/homebrew/Cellar/llvm/17.0.6/include/clang/AST/ASTConsumer.h"
#include "/opt/homebrew/Cellar/llvm/17.0.6/include/clang/Basic/Diagnostic.h"
#include "/opt/homebrew/Cellar/llvm/17.0.6/include/clang/Frontend/CompilerInstance.h"
#include "/opt/homebrew/Cellar/llvm/17.0.6/include/clang/Frontend/FrontendActions.h"
#include "/opt/homebrew/Cellar/llvm/17.0.6/include/clang/Tooling/Tooling.h"

#include "/opt/homebrew/Cellar/llvm/17.0.6/include/clang/Tooling/CommonOptionsParser.h"

#include "PrintFunctionsAction.h"  // Your custom action

using namespace clang::tooling;
using namespace llvm;

static llvm::cl::OptionCategory MyToolCategory("my-tool options");

int main(int argc, const char **argv) {
    // Parse command-line options
    //CommonOptionsParser OptionsParser(argc, argv);
    //CommonOptionsParser OptionsParser(argc, argv, MyToolCategory);
    //ClangTool Tool(OptionsParser.getCompilations(),
//                   OptionsParser.getSourcePathList());

    // Run the Clang Tool with your custom FrontendAction
    //int result = Tool.run(newFrontendActionFactory<printfunctions::PrintFunctionsAction>().get());
    printfunctions::PrintFunctionsAction;
    printf("hello world");
    return 0;
}
