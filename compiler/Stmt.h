#ifndef SPLC_STMT_H
#define SPLC_STMT_H


#include "Container.h"

class Stmt : public Container {
public:
    StmtType stmtType;

    explicit Stmt(Node *node);
    explicit Stmt(Node *node, StmtType stmtType);

public:
    ~Stmt() override = default;

    void installChild(const std::vector<Node *> &vector) override;

    bool isWhileStmt() const;
    bool isForStmt() const;

};


#endif //SPLC_STMT_H
