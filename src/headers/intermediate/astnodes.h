//
// Created by Sid B on 06/01/26.
//

#ifndef BLSLANG_ASTNODES_H
#define BLSLANG_ASTNODES_H

#include <fstream>
#include <utility>

#include "../core/blsl.h"

namespace BLSL
{
    namespace ASTNode {
        class Visitor;
        struct Node;
        struct BodyNode;
    }

    struct FormalParameter
    {
        size_t size;
        std::string identifier;
    };

    typedef std::vector<FormalParameter> FormalParameterDeclaration_t;
    typedef std::unique_ptr<ASTNode::Node> Node_t;
    typedef std::unique_ptr<ASTNode::BodyNode> BodyNode_t;

    namespace ASTNode
    {

        struct Node
        {
            virtual ~Node() = default;
            DebugPosition debugPos;

            virtual void invite(Visitor& visitor) = 0;
        };

        struct BodyNode final : Node
        {
            std::vector<Node_t> nodes;

            void invite(Visitor& visitor) override;
        };

        /*
         *  ATOMS
         */

        struct Literal final : public Node
        {
            LiteralType type;
            std::string value;

            void invite(Visitor& visitor) override;
        };

        struct Variable final : public Node
        {
            std::string identifier;


            void invite(Visitor& visitor) override;
        };

        /*
         *  OPERATORS
         */

        struct Operator : public Node
        {
            OperatorType type;
        };

        struct UnaryOperator final : public Operator
        {
            Node_t right;

            void invite(Visitor& visitor) override;
        };

        struct BinaryOperator final : public Operator
        {
            Node_t left;
            Node_t right;

            void invite(Visitor& visitor) override;
        };


        /*
         *  STATEMENTS
         */

        struct MemInit final : public Node
        {
            size_t size;

            void invite(Visitor& visitor) override;
        };

        struct Alloc final : public Node
        {
            size_t size;
            std::string identifier;

            void invite(Visitor& visitor) override;
        };

        struct Func final : public Node
        {
            std::string identifier;
            size_t returnSize;
            FormalParameterDeclaration_t parameters;
            Node_t body;

            void invite(Visitor& visitor) override;
        };

        struct If final : public Node
        {
            Node_t condition;
            Node_t body;
            std::optional<Node_t> elseBranch;

            void invite(Visitor& visitor) override;
        };

        struct For final : public Node
        {
            std::optional<Node_t> initialize;
            std::optional<Node_t> condition;
            std::optional<Node_t> update;

            Node_t body;

            void invite(Visitor& visitor) override;
        };

        struct While final : public Node
        {
            Node_t condition;
            Node_t body;

            void invite(Visitor& visitor) override;
        };

        /*
         * EXPRESSIONS
         */

        struct Call : public Node
        {
            Node_t callee;
            std::vector<Node_t> arguments;
        };
    }


    namespace ASTNode
    {
        class Visitor
        {
        public:
            virtual ~Visitor() = default;

            virtual void visit(BodyNode* node) = 0;

            virtual void visit(BinaryOperator* node) = 0;
            virtual void visit(UnaryOperator* node) = 0;
            virtual void visit(Literal* node) = 0;
            virtual void visit(Variable* node) = 0;

            virtual void visit(Func* node) = 0;
            virtual void visit(For* node) = 0;
            virtual void visit(While* node) = 0;
            virtual void visit(If* node) = 0;

            virtual void visit(MemInit* node) = 0;
            virtual void visit(Alloc* node) = 0;
        };

        class PrintVisitor final : public Visitor
        {
        private:
            std::string _outPath;
            std::ofstream _outFile;
            unsigned _tabLevel = 0;

        private:
            void _indent() {_tabLevel++;}
            void _unindent() {if (_tabLevel) _tabLevel--;}

            std::ostream& _out_indent();
            std::ostream& _out();

        public:
            PrintVisitor() = default;
            explicit PrintVisitor(std::string  outPath);
            ~PrintVisitor() override;

        public:
            void visit(BodyNode* node) override;

            void visit(BinaryOperator* node) override;
            void visit(UnaryOperator* node) override;
            void visit(Literal* node) override;
            void visit(Variable* node) override;

            void visit(Func* node) override;
            void visit(For* node) override;
            void visit(While* node) override;
            void visit(If* node) override;

            void visit(MemInit* node) override;
            void visit(Alloc* node) override;
        };
    }

}



#endif //BLSLANG_ASTNODES_H