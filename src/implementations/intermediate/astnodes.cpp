//
// Created by Sid B on 06/01/26.
//

#include "../../headers/intermediate/astnodes.h"
#include <iostream>

static class
{
private:
    unsigned _tabLevel = 0;

public:

    void indent() {_tabLevel++;}
    void unindent() {if (_tabLevel) _tabLevel--;}

    std::ostream& cout() const {for (unsigned i = 0; i < _tabLevel; i++) std::cout << '\t'; return std::cout;}
} consoleTree;



void BLSL::ASTNode::ConsoleVisitor::visit(BinaryOperator *node)
{
    consoleTree.cout() << "Binary Operator: " << static_cast<int>(node->type) << std::endl;
    consoleTree.indent();

    node->left->invite(*this);
    node->right->invite(*this);

    consoleTree.unindent();
}

void BLSL::ASTNode::ConsoleVisitor::visit(Literal *node)
{
    consoleTree.cout() << "Literal: <" << static_cast<int>(node->type) << "> " << node->value << std::endl;
}

void BLSL::ASTNode::ConsoleVisitor::visit(Variable *node)
{
    consoleTree.cout() << "Variable: " << node->identifier << std::endl;
}

void BLSL::ASTNode::Literal::invite(Visitor &visitor)
{
    visitor.visit(this);
}

void BLSL::ASTNode::Variable::invite(Visitor &visitor)
{
    visitor.visit(this);
}
void BLSL::ASTNode::BinaryOperator::invite(Visitor &visitor)
{
    visitor.visit(this);
}

