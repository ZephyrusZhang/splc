#include "Node.h"
#include "Exp.h"
#include "Container.h"
#include <vector>

Exp::Exp(Node *node, ExpType expType)
        : Container(node, containerType) {
    this->expType = expType;
}
void Exp::installChild(const std::vector<Node *> &vector) {

}