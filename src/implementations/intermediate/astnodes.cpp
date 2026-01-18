//
// Created by Sid B on 06/01/26.
//

#include "../../headers/intermediate/astnodes.h"
#include <iostream>

std::ostream & BLSL::ASTNode::PrintVisitor::_out_indent()
{
    if (_outFile)
    {
        for (unsigned i = 0; i < _tabLevel; i++)
        {
            _outFile << '\t';
        }
        return _outFile;
    }
    for (unsigned i = 0; i < _tabLevel; i++)
    {
        std::cout << '\t';
    }
    return std::cout;
}

std::ostream & BLSL::ASTNode::PrintVisitor::_out()
{
    if (_outFile)
    {
        return _outFile;
    }

    return std::cout;
}

BLSL::ASTNode::PrintVisitor::PrintVisitor(std::string outPath)
: _outPath(std::move(outPath))
{
    _outFile.open(_outPath);
    //TODO: EXCEPTION
}

BLSL::ASTNode::PrintVisitor::~PrintVisitor()
{
    _outFile.close();
}

void BLSL::ASTNode::PrintVisitor::visit(BodyNode *node)
{
    _out_indent() << "{\n";
    _indent();
    for (auto& subNode : node->nodes)
    {
        subNode->invite(*this);
    }
    _unindent();
    _out_indent() << "}\n";
}

void BLSL::ASTNode::PrintVisitor::visit(BinaryOperator *node)
{
    _out_indent() << "Binary Operator: " << static_cast<int>(node->type) << "\n";

    _out_indent() << "(\n";

    _indent();

    node->left->invite(*this);
    node->right->invite(*this);

    _unindent();

    _out_indent() << ")" << std::endl;
}

void BLSL::ASTNode::PrintVisitor::visit(UnaryOperator *node)
{
    _out_indent() << "Unary Operator: " << static_cast<int>(node->type) << "\n";

    _out_indent() << "(\n";

    _indent();
    node->right->invite(*this);
    _unindent();

    _out_indent() << ")" << std::endl;
}

void BLSL::ASTNode::PrintVisitor::visit(Literal *node)
{
    _out_indent() << "Literal: [" << static_cast<int>(node->type) << "] " << node->value << std::endl;
}

void BLSL::ASTNode::PrintVisitor::visit(Variable *node)
{
    _out_indent() << "Variable: " << node->identifier << std::endl;
}

void BLSL::ASTNode::PrintVisitor::visit(Func *node)
{
    _out_indent() << "func " << node->identifier << "<" << node->returnSize << ">\n";
    _out_indent() << "(";
    for (auto formalParam : node->parameters)
    {
        _out() << formalParam.identifier << "<" << formalParam.size << ">, ";
    }
    _out() << ")\n";
    node->body->invite(*this);
}
void BLSL::ASTNode::PrintVisitor::visit(For *node)
{
    _out_indent() << "for" << "(\n";
    _indent();
    if (node->initialize.has_value())
    {
        node->initialize.value()->invite(*this);
    }
    _out_indent() << ";\n";
    if (node->condition.has_value())
    {
        node->condition.value()->invite(*this);
    }
    _out_indent() << ";\n";
    if (node->update.has_value())
    {
        node->update.value()->invite(*this);
    }
    _out_indent() << ")\n";

    node->body->invite(*this);
}
void BLSL::ASTNode::PrintVisitor::visit(While *node) {}

void BLSL::ASTNode::PrintVisitor::visit(If *node)
{
    _out_indent() << "if (\n";
    node->condition->invite(*this);
    _out_indent() << ")\n";
    node->body->invite(*this);

    if (node->elseBranch.has_value())
    {
        _out_indent() << "else\n";
        node->elseBranch.value()->invite(*this);
    }
}

void BLSL::ASTNode::PrintVisitor::visit(MemInit *node)
{
    _out_indent() << "MemInit (" << node->size << " bytes)\n";
}

void BLSL::ASTNode::PrintVisitor::visit(Alloc *node)
{
    _out_indent() << "Alloc: " << node->identifier << " <" << node->size << ">\n";
}

void BLSL::ASTNode::BodyNode::invite(Visitor &visitor)
{
    visitor.visit(this);
}

void BLSL::ASTNode::Literal::invite(Visitor &visitor)
{
    visitor.visit(this);
}

void BLSL::ASTNode::Variable::invite(Visitor &visitor)
{
    visitor.visit(this);
}

void BLSL::ASTNode::UnaryOperator::invite(Visitor &visitor)
{
    visitor.visit(this);
}

void BLSL::ASTNode::BinaryOperator::invite(Visitor &visitor)
{
    visitor.visit(this);
}

void BLSL::ASTNode::MemInit::invite(Visitor &visitor)
{
    visitor.visit(this);
}

void BLSL::ASTNode::Alloc::invite(Visitor &visitor)
{
    visitor.visit(this);
}

void BLSL::ASTNode::Func::invite(Visitor &visitor)
{
    visitor.visit(this);
}

void BLSL::ASTNode::If::invite(Visitor &visitor)
{
    visitor.visit(this);
}

void BLSL::ASTNode::For::invite(Visitor &visitor)
{
    visitor.visit(this);
}

void BLSL::ASTNode::While::invite(Visitor &visitor)
{
    visitor.visit(this);
}

