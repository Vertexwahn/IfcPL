/*
    Copyright (c) 2017 Technical University of Munich
    Chair of Computational Modeling and Simulation.

    TUM Open Infra Platform is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License Version 3
    as published by the Free Software Foundation.

    TUM Open Infra Platform is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "OpenInfraPlatform/IfcPL/namespace.h"
#include <iostream>
#include <llvm/IR/Value.h>
#include <memory>
#include <vector>

OPENINFRAPLATFORM_IFCPL_NAMESPACE_BEGIN

class AccessModifier;
class ArrayAccess;
class ArrayCreationExpression;
class ArrayDeclaration;
class Assignment;
class BinaryOperator;
class Block;
class BreakStatement;
class Char;
class ClassArray;
class ClassDeclaration;
class IntefaceDeclaration;
class ClassMember;
class ClassVariable;
class CodeGenContext;
class ContinueStatement;
class CtorDeclaration;
class Double;
class DoWhileStatement;
class EnumerationBody;
class EnumerationDeclaration;
class EnumTypeName;
class Expression;
class ExpressionStatement;
class CastExpression;
class InstanceOfExpression;
class Float;
class ForStatement;
class FunctionDeclaration;
class Identifier;
class IfStatement;
class ImportStatement;
class Integer;
class InterfaceDeclaration;
class InterfaceMethod;
class MethodOrFunctionCall;
class MethodDeclaration;
class ModuleStatement;
class Node;
class PostfixExpression;
class PrefixExpression;
class ReferencedMethodCall;
class ReturnStatement;
class Statement;
class String;
class SwitchStatement;
class ThrowStatement;
class TrinaryExpression;
class UnaryExpression;
class UsingStatement;
class VariableDeclaration;
class CollectionVariableDeclaration;
class WhileStatement;
class NullExpr;
class ThisExpr;
class NewExpression;
class CollectionParamterDeclaration;

typedef std::vector<Statement *> StatementList;
typedef std::vector<Expression *> ExpressionList;
typedef std::vector<VariableDeclaration *> VariableList;

class NodeVisitor {
public:
    virtual void visit(const ClassMember *cm, std::ostream &out) = 0;
    virtual void visit(const Node *node, std::ostream &out) = 0;
    virtual void visit(const Char *node, std::ostream &out) = 0;
    virtual void visit(const Integer *node, std::ostream &out) = 0;
    virtual void visit(const Block *node, std::ostream &out) = 0;
    virtual void visit(const FunctionDeclaration *node, std::ostream &out) = 0;
    virtual void visit(const MethodDeclaration *node, std::ostream &out) = 0;
    virtual void visit(const InterfaceMethod *node, std::ostream &out) = 0;
    virtual void visit(const InterfaceDeclaration *node, std::ostream &out) = 0;
    virtual void visit(const IfStatement *node, std::ostream &out) = 0;
    virtual void visit(const BinaryOperator *node, std::ostream &out) = 0;
    virtual void visit(const Double *node, std::ostream &out) = 0;
    virtual void visit(const Float *node, std::ostream &out) = 0;
    virtual void visit(const ReturnStatement *node, std::ostream &out) = 0;
    virtual void visit(const MethodOrFunctionCall *node, std::ostream &out) = 0;
    virtual void visit(const ExpressionStatement *node, std::ostream &out) = 0;
    virtual void visit(const ClassDeclaration *node, std::ostream &out) = 0;
    virtual void visit(const String *node, std::ostream &out) = 0;
    virtual void visit(const ImportStatement *node, std::ostream &out) = 0;
	virtual void visit(const CastExpression *node, std::ostream &out) = 0;
	virtual void visit(const InstanceOfExpression *node, std::ostream &out) = 0;
    virtual void visit(const ModuleStatement *node, std::ostream &out) = 0;
    virtual void visit(const UsingStatement *node, std::ostream &out) = 0;
    virtual void visit(const ArrayAccess *node, std::ostream &out) = 0;
    virtual void visit(const ArrayDeclaration *node, std::ostream &out) = 0;
    virtual void visit(const ArrayCreationExpression *node, std::ostream &out) = 0;
    virtual void visit(const VariableDeclaration *node, std::ostream &out) = 0;
	virtual void visit(const CollectionVariableDeclaration *node, std::ostream &out) = 0;
	virtual void visit(const Identifier *node, std::ostream &out) = 0;
    virtual void visit(const PrefixExpression *node, std::ostream &out) = 0;
    virtual void visit(const PostfixExpression *node, std::ostream &out) = 0;
    virtual void visit(const ForStatement *node, std::ostream &out) = 0;
    virtual void visit(const WhileStatement *node, std::ostream &out) = 0;
    virtual void visit(const DoWhileStatement *node, std::ostream &out) = 0;
    virtual void visit(const SwitchStatement *node, std::ostream &out) = 0;
    virtual void visit(const BreakStatement *node, std::ostream &out) = 0;
    virtual void visit(const ContinueStatement *node, std::ostream &out) = 0;
    virtual void visit(const TrinaryExpression *node, std::ostream &out) = 0;
    virtual void visit(const UnaryExpression *node, std::ostream &out) = 0;
    virtual void visit(const CtorDeclaration *node, std::ostream &out) = 0;
    virtual void visit(const ClassVariable *node, std::ostream &out) = 0;
    virtual void visit(const ClassArray *node, std::ostream &out) = 0;
    virtual void visit(const Assignment *node, std::ostream &out) = 0;
    virtual void visit(const EnumTypeName *node, std::ostream &out) = 0;
    virtual void visit(const EnumerationDeclaration *node, std::ostream &out) = 0;
    virtual void visit(const ThrowStatement *node, std::ostream &out) = 0;
    virtual void visit(const NullExpr *node, std::ostream &out) = 0;
    virtual void visit(const ThisExpr *node, std::ostream &out) = 0;
    virtual void visit(const NewExpression *node, std::ostream &out) = 0;
	virtual void visit(const CollectionParamterDeclaration *node, std::ostream &out) = 0;
};

enum class eNodeType { Unknown, ThisExpr, Identifier, MethodCall, EnumTypeName };

class Node {
public:
    virtual ~Node() {
    }
    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }

    virtual eNodeType getType() const {
        return eNodeType::Unknown;
    }
};

class Expression : public Node {
public:
    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }
};

class Statement : public Node {
public:
    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }
};

class Char : public Expression {
public:
    char value;
    Char(char value) : value(value) {
    }
    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }
};

class Integer : public Expression {
public:
    long long value;
    Integer(long long value) : value(value) {
    }
    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }
};

class Double : public Expression {
public:
    double value;
    Double(double value) : value(value) {
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }
};

class Float : public Expression {
public:
    float value;
    Float(float value) : value(value) {
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }
};

enum class eOperation {
    PlusPlus,   // ++
    MinusMinus, // --
    Minus,
    Plus, // +
    Divde,
    Modulo,
    Subtract,
    Multiplication, // *
    EQ,             // ==
    LT,             // <
    GT,             // >
    LogicalNot,     // !
    NOTEQ,          // !=
    Assign,         // =
    GE,             // >=
    Index,          // []
    NoOperator
};

class UnaryExpression : public Expression {
public:
    eOperation operation;
    std::shared_ptr<Expression> expression;

    UnaryExpression(eOperation operation, std::shared_ptr<Expression> expression) : operation(operation), expression(expression) {
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }
};

class TrinaryExpression : public Expression {
public:
    std::shared_ptr<Expression> conditionExpr;
    std::shared_ptr<Expression> thenExpr;
    std::shared_ptr<Expression> elseExpr;

    TrinaryExpression(std::shared_ptr<Expression> conditionExpr, std::shared_ptr<Expression> thenExpr, std::shared_ptr<Expression> elseExpr)
        : conditionExpr(conditionExpr), thenExpr(thenExpr), elseExpr(elseExpr) {
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }
};

class String : public Expression {
public:
    std::string value;
    String(const std::string &value) : value(value) {
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }
};

enum class eAccessModfier { Public, Protected, Private };

eAccessModfier strToAccessModfier(const char *str);

std::string accessModfierToStr(const eAccessModfier am);

class AccessModifier : public Node {
public:
    eAccessModfier value;
    AccessModifier(eAccessModfier value) : value(value) {
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }
};

class Identifier : public Expression {
public:
    std::string name;
    Identifier(const std::string &name) : name(name) {
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }

    virtual eNodeType getType() const override;
};

class BreakStatement : public Statement {
public:
    BreakStatement() {
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }
};

class ContinueStatement : public Statement {
public:
    ContinueStatement() {
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }
};

class MethodOrFunctionCall : public Expression {
public:
    const Identifier &id;
    std::vector<Expression *> arguments;

    MethodOrFunctionCall(const Identifier &id, ExpressionList &arguments) : id(id), arguments(arguments) {
    }

    MethodOrFunctionCall(const Identifier &id) : id(id) {
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }

    virtual eNodeType getType() const override;
};

class BinaryOperator : public Expression {
public:
    eOperation op;
    Expression &lhs;
    Expression &rhs;
    BinaryOperator(Expression &lhs, eOperation op, Expression &rhs) : lhs(lhs), rhs(rhs), op(op) {
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }
};

enum class eAssignmentOperator { EQ, MULEQ, DIVEQ, MODEQ, ADDEQ, SUBEQ, SREQ, SLEQ, ANDEQ, XOREQ, OREQ };

class ThisExpr : public Expression {
public:
    ThisExpr() {
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }

    virtual eNodeType getType() const override {
        return eNodeType::ThisExpr;
    }
};

class NullExpr : public Expression {
public:
    NullExpr() {
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }
};

class Assignment : public Expression {
public:
    Expression &lhs;
    Expression &rhs;
    eAssignmentOperator aop;

    Assignment(Expression &lhs, eAssignmentOperator aop, Expression &rhs) : lhs(lhs), aop(aop), rhs(rhs) {
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }
};

class EnumTypeName : public Expression {
public:
    Expression &lhs;
    Expression &rhs;

    EnumTypeName(Expression &lhs, Expression &rhs) : lhs(lhs), rhs(rhs) {
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }

    virtual eNodeType getType() const override {
        return eNodeType::EnumTypeName;
    }
};

class Block : public Statement {
public:
    StatementList statements;
    Block() {
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }
};

// TODO Should be a ThrowExpression?
class ThrowStatement : public Statement {
public:
    const Expression &expr;

    ThrowStatement(const Expression &expr) : expr(expr) {
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const override {
        vistor->visit(this, out);
    }
};

class NewExpression : public Expression {
public:
    const Expression &expr;

    NewExpression(const Expression &expr) : expr(expr) {
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }

    virtual eNodeType getType() const {
        return eNodeType::MethodCall;
    }
};

class ExpressionStatement : public Statement {
public:
    Expression &expression;
    ExpressionStatement(Expression &expression) : expression(expression) {
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const override {
        vistor->visit(this, out);
    }
};

class ReturnStatement : public Statement {
public:
    Expression *expression = nullptr;
    ReturnStatement(Expression *expression) : expression(expression) {
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }
};

class ArrayCreationExpression : public Statement {
public:
    bool elementsAreValueTypes = true;

    const Identifier &elementType;
    Expression &expr;

    bool elementsAreReferenceTypes() const {
        return !elementsAreValueTypes;
    }

    ArrayCreationExpression(bool elementsAreValueTypes, const Identifier &elementType, Expression &expr)
        : elementsAreValueTypes(elementsAreValueTypes), elementType(elementType), expr(expr) {
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const override {
        vistor->visit(this, out);
    }
};

class ArrayAccess : public Expression {
public:
    const Identifier &id;
    const Expression &indexExpr;
    const Expression *assignmentExpr;

    ArrayAccess(const Identifier &id, const Expression &indexExpr, const Expression *assignmentExpr) : 
        id(id), 
        indexExpr(indexExpr), 
        assignmentExpr(assignmentExpr) {
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const override {
        vistor->visit(this, out);
    }
};

class VariableDeclaration : public Statement {
public:
    bool isReferenceType = false;
    bool isConst = false;
    const Identifier &type;
    Identifier &id;
    Expression *assignmentExpr;
    VariableDeclaration(const Identifier &type, Identifier &id) : type(type), id(id) {
        assignmentExpr = nullptr;
    }
    VariableDeclaration(const Identifier &type, Identifier &id, Expression *assignmentExpr) : type(type), id(id), assignmentExpr(assignmentExpr) {
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }
};

class ArrayDeclaration : public VariableDeclaration {
public:
	bool isConst = false;
	const Identifier &type;
	Identifier &id;
	ArrayCreationExpression *ace = nullptr;

	ArrayDeclaration(const Identifier &type, Identifier &id, ArrayCreationExpression *ace) : type(type), id(id), ace(ace), VariableDeclaration(type, id) {
	}

	virtual void accept(NodeVisitor *vistor, std::ostream &out) const override {
		vistor->visit(this, out);
	}
};

enum class eCollectionType {
	Array,
	Bag,
	Set,
	List,
	Unknown
};

class CollectionParamterDeclaration : public Statement {
public:
	std::shared_ptr<Identifier> ident;
	eCollectionType type = eCollectionType::Unknown;
	std::shared_ptr<Statement> collection_type;

	CollectionParamterDeclaration(std::shared_ptr<Identifier> ident) : ident(ident) {

	}

	CollectionParamterDeclaration(std::shared_ptr<Statement> collection_type, eCollectionType type) :
		collection_type(collection_type), type(type) {

	}

	virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
		vistor->visit(this, out);
	}
};

class CollectionVariableDeclaration : public VariableDeclaration {
public:
	Statement & cpd;
	eCollectionType type = eCollectionType::Unknown;
	//Identifier &id;

	CollectionVariableDeclaration(Statement &cpd, Identifier &id, eCollectionType type) : cpd(cpd), VariableDeclaration(id,id), type(type) {
	}

	virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
		vistor->visit(this, out);
	}
};

class IfStatement : public Statement {
public:
    std::unique_ptr<Expression> conditionExpr;
    std::unique_ptr<Statement> thenExpr;
    std::unique_ptr<Statement> elseExpr;

    IfStatement(std::unique_ptr<Expression> conditionExpr, std::unique_ptr<Statement> thenExpr, std::unique_ptr<Statement> elseExpr)
        : conditionExpr(std::move(conditionExpr)), thenExpr(std::move(thenExpr)), elseExpr(std::move(elseExpr)) {
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }
};

class ForStatement : public Statement {
public:
    std::shared_ptr<Statement> initStatement;
    std::shared_ptr<Expression> condition;
    std::shared_ptr<Statement> counterStmt;
    std::shared_ptr<Statement> body;

    ForStatement(std::shared_ptr<Statement> initStatement, std::shared_ptr<Expression> condition, std::shared_ptr<Statement> counterStmt, std::shared_ptr<Statement> body)
        : initStatement(initStatement), condition(condition), counterStmt(counterStmt), body(body) {
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }
};

class WhileStatement : public Statement {
public:
    std::shared_ptr<Expression> condition;
    std::shared_ptr<Block> body;

    WhileStatement(std::shared_ptr<Expression> condition, std::shared_ptr<Block> body) : condition(condition), body(body) {
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }
};

class DoWhileStatement : public Statement {
public:
    std::shared_ptr<Expression> condition;
    std::shared_ptr<Block> body;

    DoWhileStatement(std::shared_ptr<Block> body, std::shared_ptr<Expression> condition) : body(body), condition(condition) {
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }
};

enum class switchLabelType { Case, Default };

class SwitchLabel : public Node {
public:
    SwitchLabel(switchLabelType type, std::shared_ptr<Expression> expression) : type(type), expression(expression) {
    }

    switchLabelType type;
    std::shared_ptr<Expression> expression;

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }
};

class SwitchSection : public Node {
public:
    SwitchSection(std::shared_ptr<SwitchLabel> switchLabel, std::shared_ptr<Block> body) : switchLabel(switchLabel), body(body) {
    }

    std::shared_ptr<SwitchLabel> switchLabel;
    std::shared_ptr<Block> body;

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }
};

class SwitchSections : public Node {
public:
    std::vector<std::shared_ptr<SwitchSection>> switch_sections_;

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }

    void addSwitchSection(std::shared_ptr<SwitchSection> ss) {
        switch_sections_.push_back(ss);
    }
};

class SwitchStatement : public Statement {
public:
    std::shared_ptr<Expression> switchExpression;
    std::shared_ptr<SwitchSections> switchSections;

    SwitchStatement(std::shared_ptr<Expression> switchExpression, std::shared_ptr<SwitchSections> switchSections)
        : switchExpression(switchExpression), switchSections(switchSections) {
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }
};

// class NArrayDeclaration : public NStatement
// {
// public:
// 	const NIdentifier& type;
// 	NIdentifier& id;
//
// 	virtual llvm::Value* codeGen(CodeGenContext& context) override
// 	{
// 		throw std::logic_error("The method or operation is not
// implemented.");
// 	}
// };

class FunctionDeclaration : public Statement {
public:
    const Identifier &type;
    const Identifier &id;
    VariableList arguments;
    Block &block;
    eOperation OperationType = eOperation::NoOperator;

    FunctionDeclaration(const Identifier &type, const Identifier &id, const VariableList &arguments, Block &block) : type(type), id(id), arguments(arguments), block(block) {
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }
};

class InstanceOfExpression : public Expression {
public:
	const Expression &instance_id;
	const Identifier &subtype_id;

	InstanceOfExpression(const  Expression &id1, const Identifier &id2) :
		instance_id(id1), subtype_id(id2) {
	}

	virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
		vistor->visit(this, out);
	}
};

class CastExpression : public Expression {
public:
	const Identifier &id;
	const Expression &expression;

	CastExpression(const Identifier &id, const Expression& exp) : id(id), expression(exp) {
	}

	virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
		vistor->visit(this, out);
	}
};

class ImportStatement : public Statement {
public:
    std::string name;

    ImportStatement(const std::string &importName) : name(importName) {
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }
};

class ModuleStatement : public Statement {
public:
    std::string name;

    ModuleStatement(const std::string &name) : name(name) {
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }
};

class UsingStatement : public Statement {
public:
    std::string name;

    UsingStatement(const std::string &name) : name(name) {
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }
};

class PrefixExpression : public Expression {
public:
    const Identifier &id;
    eOperation op;

    PrefixExpression(const Identifier &id, eOperation op) : id(id), op(op) {
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }
};

class PostfixExpression : public Expression {
public:
    const Identifier &id;
    eOperation op;

    PostfixExpression(const Identifier &id, eOperation op) : id(id), op(op) {
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }
};

class ClassMember : public Node {
public:
    virtual void accept(NodeVisitor *vistor, std::ostream &out) const override {
        vistor->visit(this, out);
    }
};

class InterfaceMethod : public Node {
public:
    bool isConst = false;
    bool returnValueIsReference = false;
    bool isOperator = false;
    eOperation OperatorType = eOperation::NoOperator;

    const Identifier &type;
    const Identifier &id;
    VariableList arguments;

    InterfaceMethod(const Identifier &type,
        const Identifier &id, 
        const VariableList &arguments, bool isConst = false)
        : type(type), id(id), arguments(arguments), isConst(isConst) {
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }
};

class MethodDeclaration : public ClassMember {
public:
    bool isStatic = false;
    bool isOverride = false;
    bool isConst = false;
    bool isVirtual = false;
    bool returnValueIsReference = false;
    bool isOperator = false;
    eOperation OperatorType = eOperation::NoOperator;

    const AccessModifier &access;
    const Identifier &type;
    const Identifier &id;
    VariableList arguments;
    Block &block;
    MethodDeclaration(const AccessModifier &access, const Identifier &type, const Identifier &id, const VariableList &arguments, Block &block, bool isStatic)
        : access(access), type(type), id(id), arguments(arguments), block(block), isStatic(isStatic) {
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }
};

class CtorDeclaration : public ClassMember {
public:
    const AccessModifier &access;
    const Identifier &id;
    VariableList arguments;
    Block &block;
    bool hasBaseClassCtor = false;
    ExpressionList baseClassArguments;

    CtorDeclaration(const AccessModifier &access, const Identifier &id, const VariableList &arguments, Block &block) : access(access), id(id), arguments(arguments), block(block) {
    }

    CtorDeclaration(const AccessModifier &access, const Identifier &id, const VariableList &arguments, Block &block, ExpressionList &bcarguments)
        : access(access), id(id), arguments(arguments), block(block), baseClassArguments(bcarguments) {
        hasBaseClassCtor = true;
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }
};

class ClassVariable : public ClassMember {
public:
    const AccessModifier &access;
    const bool isStatic = false;
    const VariableDeclaration &var_decl;

    ClassVariable(const AccessModifier &access, const VariableDeclaration &var_decl, const bool isStatic = false) : access(access), var_decl(var_decl), isStatic(isStatic) {
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const override {
        vistor->visit(this, out);
    }
};

class ClassArray : public ClassMember {
public:
    const AccessModifier &access;
    const bool isStatic = false;
    const ArrayDeclaration &array_decl;

    ClassArray(const AccessModifier &access, const ArrayDeclaration &array_decl, const bool isStatic = false) : access(access), array_decl(array_decl), isStatic(isStatic) {
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }
};

class ClassBody : public Node {
public:
    std::vector<std::shared_ptr<ClassMember>> members_;

    void addClassMember(std::shared_ptr<ClassMember> cv) {
        members_.push_back(cv);
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }
};

class InterfaceBody : public Node {
public:
    std::vector<std::shared_ptr<InterfaceMethod>> members_;

    void addMember(std::shared_ptr<InterfaceMethod> cv) {
        members_.push_back(cv);
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }
};

class ClassDeclaration : public Node {
public:
    const Identifier &id;
    std::shared_ptr<ClassBody> body;
    bool hasBaseClass = false;
    std::shared_ptr<Identifier> baseClass;

    ClassDeclaration(const Identifier &id, std::shared_ptr<ClassBody> body) : id(id), body(body) {
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }
};

class InterfaceDeclaration : public Node {
public:
    const Identifier &id;
    std::shared_ptr<InterfaceBody> body;

    InterfaceDeclaration(const Identifier &id, std::shared_ptr<InterfaceBody> body) : id(id), body(body) {
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }
};

class EnumerationBody : public Node {
public:
    std::vector<Identifier> memberDeclarations;

    void addMemberDeclaration(Identifier id) {
        memberDeclarations.push_back(id);
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }
};

class EnumerationDeclaration : public Node {
public:
    const Identifier id;
    std::shared_ptr<EnumerationBody> body;

    EnumerationDeclaration(Identifier id, std::shared_ptr<EnumerationBody> body) : id(id), body(body) {
    }

    virtual void accept(NodeVisitor *vistor, std::ostream &out) const {
        vistor->visit(this, out);
    }
};

OPENINFRAPLATFORM_IFCPL_NAMESPACE_END
