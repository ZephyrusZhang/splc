
#include <cstdlib>
#include <fstream>
#include "Node.h"
#include "Scope.h"
#include "syntax.tab.h"
#include "CodeBlock.h"

extern int errCount;
extern FILE *yyin, *yyout;
std::string outputPath;
std::string irOutputPath;
std::ofstream outputFile;

Node *root = nullptr;

void yyerror(const char *s) {
    std::cerr << s << std::endl;
}

void initArgs(const std::string &fileName) {
    size_t index = fileName.find_last_of('.');
    outputPath = fileName.substr(0, index + 1) + "out";
    irOutputPath = fileName.substr(0, index + 1) + "ir";
    std::cout << "write to: " << outputPath << std::endl;
    outputFile.open(outputPath, std::ios::out);
}

void outputTree(Node *node) {
    std::cout << "Start to write output to " << outputPath << std::endl;
    Node::printTree(node);
}

Node * generateSpecifier(const std::string& type) {
    Node* typeNode = new Node("TYPE", -1, DataType::DTYPE, type);
    return Node::createNodeWithChildren("Specifier", -1, DataType::PROD, {typeNode});
}

void generateRead() {
    // int read() {}
    auto funRetSpecifier = std::make_shared<Specifier>(nullptr);
    funRetSpecifier->type = TypeInt;
    auto funDec = std::make_shared<Dec>(nullptr);
    funDec->identifier = std::make_unique<std::string>("read");
    funDec->funcDec = std::make_unique<std::vector<Dec::ParmaDec>>();
    Scope::getGlobalScope()->insertSymbol("read", funRetSpecifier, funDec);
    assert(Scope::getGlobalScope()->isSymbolExists("read"));
}

void generateWrite() {
    // int write(int);
    auto funRetSpecifier = std::make_shared<Specifier>(nullptr);
    funRetSpecifier->type = TypeInt;
    auto funDec = std::make_shared<Dec>(nullptr);
    funDec->identifier = std::make_unique<std::string>("write");
    funDec->funcDec = std::make_unique<std::vector<Dec::ParmaDec>>();
    {
        auto paramSpecifier = std::make_shared<Specifier>(nullptr);
        paramSpecifier->type = TypeInt;
        auto paramDec = std::make_shared<Dec>(nullptr);
        paramDec->identifier = std::make_unique<std::string>("data");
        funDec->funcDec->emplace_back(paramSpecifier, paramDec);
    }
    Scope::getGlobalScope()->insertSymbol("write", funRetSpecifier, funDec);
    assert(Scope::getGlobalScope()->isSymbolExists("write"));

}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <file_path>" << std::endl;
        return 1;
    } else if (!(yyin = fopen(argv[1], "r"))) {
        perror(argv[1]);
        return 1;
    }
    initArgs(argv[1]);

    generateRead();
    generateWrite();

    yyparse();


    if (errCount > 0) {
        std::cout << "Error Occur" << std::endl;
    } else {
        outputTree(root);
        // Ir Generation
        Node *extDefList = root->children[0];
        assert(extDefList->tokenName == "ExtDefList");
        auto defLists = Node::convertTreeToVector(extDefList, "ExtDefList", {"ExtDef"});
        std::vector<std::shared_ptr<FunctionCodeBlock>> functions;
        for (const auto &item: defLists) {
            if (item->children[1]->tokenName == "FunDec") {
                const std::string functionName = item->children[1]->children[0]->data;
                std::cout << "IRGen for function " << functionName << std::endl;
                functions.push_back(std::make_shared<FunctionCodeBlock>(item));
                functions.back()->startTranslation();
            }
        }
        std::ofstream irOutFile(irOutputPath);
        for (auto &item: functions) {
            item->generateIr(irOutFile);
        }
        for (int i = 0; i< 10;i ++) {

        }
    }
    outputFile.close();
    return 0;
}

