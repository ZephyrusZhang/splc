#include "Node.h"
#include "Container.h"
#include <utility>
#include <algorithm>
#include <vector>
#include <iterator>
#include <queue>
#include "Scope.h"
#include "Exp.h"
#include "Stmt.h"

void Node::printTree(const Node *root, std::ostream &outputStream) {
    recursivePrint(root, 0, outputStream);
}

void Node::recursivePrint(const Node *cur, int depth, std::ostream &outputStream) {
    if (cur->children.empty() && cur->type == DataType::PROD) return;
    if (cur->children.empty()) {
        for (int j = 0; j < depth; j++) {
            outputStream << "  ";
        }
        if (cur->type == DataType::DTYPE) {
            outputStream << "TYPE: " << cur->data << std::endl;
        } else if (cur->type == DataType::ID) {
            outputStream << "ID: " << cur->data << std::endl;
        } else if (cur->type == DataType::INT) {
            outputStream << "INT: " << cur->data << std::endl;
        } else if (cur->type == DataType::FLOAT) {
            outputStream << "FLOAT: " << cur->data << std::endl;
        } else if (cur->type == DataType::CHAR) {
            outputStream << "CHAR: " << cur->data << std::endl;
        } else if (cur->type == DataType::STRING) {
            outputStream << "STRING: " << cur->data << std::endl;
        } else {
            outputStream << cur->tokenName << std::endl;
        }
        return;
    } else {
        for (int j = 0; j < depth; j++)
            outputStream << "  ";
        outputStream << cur->tokenName << " (" << cur->lineno << ")" << std::endl;
        for (auto &i: cur->children) {
            recursivePrint(i, depth + 1, outputStream);
        }
    }
}

Node *Node::createNodeWithChildren(const std::string &tokenName, int lineno, DataType type,
                                   std::initializer_list<Node *> childList, const std::string &data) {
    Node *parent = new Node(tokenName, lineno, type, data);
    for (Node *child: childList) {
        child->parent = parent;
        parent->children.push_back(child);
    }
    if (parent->container) {
        parent->container->installChild(parent->children);
    }
    for (const auto &item: parent->children) {
        if (item->container) item->container->onThisInstalled();
    }
    return parent;
}

Node *Node::createExpNodeWithChildren(const std::string &tokenName, int lineno, ExpType expType,
                                      std::initializer_list<Node *> childList) {
    Node *parent = new Node(tokenName, lineno, DataType::PROD, "");
    for (Node *child: childList) {
        child->parent = parent;
        parent->children.push_back(child);
    }
    parent->container = std::make_shared<Exp>(parent, expType);
    parent->container->installChild(parent->children);
    for (const auto &item: parent->children) {
        if (item->container) item->container->onThisInstalled();
    }
    return parent;
}

Node *Node::createStmtNodeWithChildren(const std::string &tokenName, int lineno, StmtType stmtType,
                                       std::initializer_list<Node *> childlist) {
    Node *parent = new Node(tokenName, lineno, DataType::PROD, "");
    for (Node *child : childlist) {
        child->parent = parent;
        parent->children.push_back(child);
    }
    parent->container = std::make_shared<Stmt>(parent, stmtType);
    parent->container->installChild(parent->children);
    for (const auto &item : parent->children) {
        if (item->container) item->container->onThisInstalled();
    }
    return parent;
}


Node::Node(std::string tokenName, int lineno, DataType type, std::string data)
        : tokenName(std::move(tokenName)), lineno(lineno), type(type), data(std::move(data)) {
    Container::generateContainer(this);
}

std::vector<Node *>
Node::convertTreeToVector(const Node *root, const std::string &recursiveName,
                          std::initializer_list<const std::string> acceptItemsToken,
                          bool anyRecursiveName) {
    std::vector<Node *> ret;
    const Node *current = root;
    while (current != nullptr) {
        assert(current->tokenName == recursiveName || anyRecursiveName);
        const Node *next = nullptr;
        for (const auto &child: current->children) {
            const auto shouldAccept = [child](const std::string &str) { return child->tokenName == str; };
            if (std::any_of(acceptItemsToken.begin(), acceptItemsToken.end(), shouldAccept)) {
                ret.push_back(child);
            }
            if (child->tokenName == recursiveName || anyRecursiveName) next = child;
        }
        current = next;
    }
    for (const auto &item: ret) {
        assert(std::any_of(acceptItemsToken.begin(), acceptItemsToken.end(),
                           [&item](const std::string &str) { return str == item->tokenName; }));
    }
    return ret;
}

bool Node::hasToken(std::initializer_list<const std::string> acceptItemsToken) const {
    std::queue<const Node *> queue;
    queue.push(this);
    while (!queue.empty()) {
        const Node *cur = queue.front();
        queue.pop();
        const auto shouldAccept = [&cur] (const std::string &str) { return cur->tokenName == str; };
        if (std::any_of(acceptItemsToken.begin(), acceptItemsToken.end(), shouldAccept)) {
            return true;
        }
        for (const auto &next : cur->children) {
            const auto shouldAccept = [&next] (const std::string &str) { return next->tokenName == str; };
            if (std::any_of(acceptItemsToken.begin(), acceptItemsToken.end(), shouldAccept)) {
                return true;
            }
            queue.push(next);
        }
    }
    return false;
}

