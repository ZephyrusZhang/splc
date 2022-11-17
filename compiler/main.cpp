
#include <cstdlib>
#include <fstream>
#include "Node.h"
#include "syntax.tab.h"

extern int errCount;
extern FILE *yyin, *yyout;
std::string outputPath;
std::ofstream outputFile;

Node *root = nullptr;

void yyerror(const char *s) {
    std::cerr << s << std::endl;
}

void initArgs(const std::string &fileName) {
    size_t index = fileName.find_last_of('.');
    outputPath = fileName.substr(0, index + 1) + "out";
    std::cout << "write to: " << outputPath << std::endl;
    outputFile.open(outputPath, std::ios::out);
}

void outputTree(Node *node) {
    std::cout << "Start to write output to " << outputPath << std::endl;
    Node::printTree(node);
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

    yyparse();

    if (errCount > 0) {
        std::cout << "Error Occur" << std::endl;
    } else {
        outputTree(root);
    }
    outputFile.close();
    return 0;
}

