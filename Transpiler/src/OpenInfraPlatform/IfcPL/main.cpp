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

#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <vector>
#include "SymbolTable.h"

// A simple std::map wrapper.
// For some reason the C++ committee did never manage to create a easy usable map.
template <typename Key, typename Value>
class Map : public std::map<Key, Value> {
public:
	bool contains(const Key& key) {
		return this->find(key) != this->end();
	}

	void insert(const Key& k, const Value& v) {
		(*this)[k] = v;
	}

	Value getValue(const Key& k) {
		return (*this)[k];
	}
};



#include "node.h"
#include "parser.hpp"
#include "tclap/CmdLine.h"
using namespace std;
using namespace OpenInfraPlatform::IfcPL;

extern int yyparse();
extern OpenInfraPlatform::IfcPL::Block* programBlock;
extern FILE* yyin;
extern int line_num;

void yyerror(const char* s) {
	cout << "Parse error!  Message: " << s << endl;
	cout << "Line number: " << line_num << endl;
	// might as well halt now:
	exit(-1);
}

std::string g_SourceDirectory = "";
std::string g_OutputDirectory = "";
std::string g_schemaName = "";

bool hasEnding(std::string const& fullString, std::string const& ending) {
	if (fullString.length() >= ending.length()) {
		return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
	} else {
		return false;
	}
}

bool startsWith(const string& haystack, const string& needle) {
	return needle.length() <= haystack.length() && std::equal(needle.begin(), needle.end(), haystack.begin());
}

class CppTransform : public NodeVisitor {
public:
	CppTransform(std::ostream& out) : out_(out) {
		writeProlog();
	}

	void writeClassForwardDeclarations() {
		for (int i = 0; i < classNames_.size(); i++) {
			out_ << "class " << classNames_[i] << ";" << std::endl;
		}
	}

	void writeProlog() {
		// out << "#define IFCPL_EXPORT" << std::endl;
		// out << "#include \"OpenInfraPlatfomIfcPL.h\"" << std::endl;
		out_ << "#include <string>" << std::endl;
		out_ << "#include <memory>" << std::endl;
		out_ << "#include \"IfcPLArray.h\"" << std::endl;
		out_ << "#include \"IfcPLString.h\"" << std::endl;
		out_ << "#include \"IfcPLException.h\"" << std::endl;
		out_ << "#include \"BlueFramework/Core/PropertySet.h\"" << std::endl;
	}

	void writeEpilog() {
		if (bIArbitraryTransitionCurveImport_) {
			out_ << "extern \"C\"" << std::endl;
			out_ << "::IArbitraryTransitionCurve*" << std::endl;
			out_ << "createIArbitraryTransitionCurve(buw::PropertySet& properties)" << std::endl;
			out_ << "{" << std::endl;
			out_ << "    return new " << moduleName_.c_str() << "(properties);" << std::endl;
			out_ << "}" << std::endl;
		}
	}

	virtual void visit(const ClassMember* cm, std::ostream& out) override {
		out << "ClassMember" << std::endl;
	}

	virtual void visit(const Node* node, std::ostream& out) override {
		out << "Node" << std::endl;
	}

	virtual void visit(const Integer* node, std::ostream& out) override {
		out << node->value;
	}

	virtual void visit(const Char* node, std::ostream& out) override {
		if (node->value == '\0') {
			out << "'\\0'";
		} else if (node->value == '\1') {
			out << "'\\1'";
		} else {
			out << "'" << node->value << "'";
		}
	}

	virtual void visit(const Identifier* node, std::ostream& out) override {
		out << node->name.c_str();
	}

	virtual void visit(const InterfaceMethod* node, std::ostream& out) override {
		out << "public: virtual " << std::endl;

		if (node->type.name == "string") {
			out << "IfcPLString " << std::endl;
		} else if (isReferenceType(node->type.name)) {
			out << "std::shared_ptr<";
			out << node->type.name.c_str() << "> ";
		} else {
			out << " " << node->type.name.c_str() << " ";

			if (node->returnValueIsReference) {
				out << " & ";
			}
		}

		if (node->isOperator) {
			switch (node->OperatorType) {
			case eOperation::Plus: out << node->id.name.c_str() << "+("; break;
			case eOperation::Index: out << node->id.name.c_str() << "[]("; break;
			default: assert(false); break;
			}
		} else {
			out << node->id.name.c_str() << "(";
		}

		for (int i = 0; i < node->arguments.size(); i++) {
			if (i > 0) {
				out << ", ";
			}

			if (node->arguments[i]->isConst) {
				out << "const ";
			}

			if (isReferenceType(node->arguments[i]->type.name)) {
				out << "std::shared_ptr<" << node->arguments[i]->type.name.c_str() << "> " << node->arguments[i]->id.name.c_str();
			} else {
				if (node->arguments[i]->type.name == "string") {
					out << "IfcPLString " << node->arguments[i]->id.name.c_str();
				} else {
					out << node->arguments[i]->type.name.c_str() << " " << node->arguments[i]->id.name.c_str();
				}
			}
		}

		out << ")";

		if (node->isConst) {
			out << " const";
		}

		out << "= 0;" << std::endl;
	}

	virtual void visit(const ArrayDeclaration* node, std::ostream& out) override {
		symbolDescription s;
		s.id = node->id.name;
		s.typeName = node->type.name;
		s.isArrayType = true;
		symbolTable_.add(s);

		out << "std::shared_ptr<IfcPLArray<" << node->type.name << ">> ";
		out << node->id.name;
		
		if (node->ace) {
			out << " = ";

			node->ace->accept(this, out);
		}

		out << ";" << std::endl;
	}

	virtual void visit(const ArrayCreationExpression* node, std::ostream& out) override {
		out << "std::make_shared<IfcPLArray<" << node->elementType.name << ">>(";
		node->expr.accept(this, out);
		out << ")";
	}

	virtual void visit(const ArrayAccess* node, std::ostream& out) override {
		if (node->id.name == "HasProperties") {
			out << node->id.name << "[";
			node->indexExpr.accept(this, out);
			out << "]";
		} 
		
		else if (node->id.name == "Segments") {
			out << node->id.name << "[";
			node->indexExpr.accept(this, out);
			out << "]";
		}
		
		else if (node->id.name == "ListValues") {
			out << node->id.name << "[";
			node->indexExpr.accept(this, out);
			out << "]";
		} else {
			// check if it a string, since this requires 'special' handling
			if ( symbolTable_.isStringArrayType(node->id.name) ) {
				out << "*" << node->id.name << "->operator[](";
				node->indexExpr.accept(this, out);
				out << ").get()";
			}
			else {
				out << node->id.name << "->operator[](";
				node->indexExpr.accept(this, out);
				out << ")";
			}
		}

		if (node->assignmentExpr) {
			out << " = ";
			node->assignmentExpr->accept(this, out);
		}
	}

	virtual void visit(const CollectionVariableDeclaration* node, std::ostream& out) override {
		out << "std::vector<";
		node->cpd.accept(this, out);
		out << ">";
		out << " ";
		node->id.accept(this, out);
		out << std::endl;
	}

	virtual void visit(const CollectionParamterDeclaration* node, std::ostream& out) override {
		if (node->ident != nullptr) {
			node->ident->accept(this, out);
		} else if (node->collection_type != nullptr) {
			if (node->type == eCollectionType::List) {
				out << "std::vector<";
			}

			node->collection_type->accept(this, out);

			if (node->type == eCollectionType::List) {
				out << ">";
			}
		}
	}

	virtual void visit(const VariableDeclaration* node, std::ostream& out) override {
		if (startsWith(node->type.name, "Ifc")) {
			// seems to be a Ifc embedded type - add it to sym table
			symbolTable_.add(node->id.name, node->type.name, true);
		}

		if (node->isConst)
			out << "const ";

		if (node->type.name == "XIfcPropertySet") {
			out << "const buw::PropertySet&";
		} else if (node->type.name == "string") {
			out << "IfcPLString ";
		} else {
			if (isReferenceType(node->type.name)) {
				out << "std::shared_ptr<" << node->type.name << ">" << std::endl;
			} else {
				if (node->isReferenceType) {
					out << node->type.name << " & ";
				} else
					out << node->type.name;
			}
		}

		out << " " << node->id.name;

		if (node->assignmentExpr) {
			out << " = ";

			node->assignmentExpr->accept(this, out);
		}
	}

	virtual void visit(const ThisExpr* node, std::ostream& out) override {
		out << "this";
	}

	virtual void visit(const Assignment* node, std::ostream& out) {
		node->lhs.accept(this, out);

		switch (node->aop) {
		case eAssignmentOperator::EQ: out << " = "; break;
		case eAssignmentOperator::ADDEQ: out << " += "; break;
		case eAssignmentOperator::MULEQ: out << " *= "; break;
		default: throw std::runtime_error("Unknown assignment operator.");
		}

		node->rhs.accept(this, out);
	}

	virtual void visit(const EnumTypeName* node, std::ostream& out) override {
		// Find out if lhs is enum
		bool isEnum = false;
		if (node->lhs.getType() == eNodeType::Identifier) {
			const Identifier& id = static_cast<Identifier&>(node->lhs);
			if (isEnumType(id.name)) {
				// out << id.name.c_str() << "::";
				isEnum = true;
			}

			if (id.name == "IfcBooleanOperator" || id.name == "IfcProfileTypeEnum") {
				isEnum = true;
			}
		}

		// check if this is an embedded type access to EXPRESS
		if (node->lhs.getType() == eNodeType::EnumTypeName) {
			EnumTypeName& etn = static_cast<EnumTypeName&>(node->lhs);

			if (etn.lhs.getType() == eNodeType::Identifier) {
				const Identifier& id = static_cast<Identifier&>(etn.lhs);

				if (symbolTable_.isExpressType(id.name)) {
					const Identifier& rhs_id = static_cast<Identifier&>(etn.rhs);

					if (rhs_id.name == "DirectionRatios") {
						etn.lhs.accept(this, out);

						out << "->m_DirectionRatios";

						if (node->rhs.getType() == eNodeType::MethodCall) {
							MethodOrFunctionCall& mcall = static_cast<MethodOrFunctionCall&>(node->rhs);
							if (mcall.id.name == "add") {
								out << ".push_back(";

								mcall.arguments[0]->accept(this, out);

								out << ")";
								return;
							}
						}

						return;
					}

					if (rhs_id.name == "Coordinates") {
						etn.lhs.accept(this, out);

						out << "->m_Coordinates";

						if (node->rhs.getType() == eNodeType::MethodCall) {
							MethodOrFunctionCall& mcall = static_cast<MethodOrFunctionCall&>(node->rhs);
							if (mcall.id.name == "add") {
								out << ".push_back(";

								mcall.arguments[0]->accept(this, out);

								out << ")";
								return;
							}
						}

						return;
					}

					else if (rhs_id.name == "Points") {
						etn.lhs.accept(this, out);

						out << "->m_Points";

						if (node->rhs.getType() == eNodeType::MethodCall) {
							MethodOrFunctionCall& mcall = static_cast<MethodOrFunctionCall&>(node->rhs);
							if (mcall.id.name == "add") {
								out << ".push_back(";

								mcall.arguments[0]->accept(this, out);

								out << ")";
								return;
							}
						}

						return;
					}

					else if (rhs_id.name == "ListValues") {
						etn.lhs.accept(this, out);

						out << "->m_ListValues";

						if (node->rhs.getType() == eNodeType::MethodCall) {
							MethodOrFunctionCall& mcall = static_cast<MethodOrFunctionCall&>(node->rhs);
							if (mcall.id.name == "add") {
								out << ".push_back(";

								mcall.arguments[0]->accept(this, out);

								out << ")";
								return;
							}
						}

						return;
					}

					else if (rhs_id.name == "HasProperties") {
						etn.lhs.accept(this, out);

						out << "->m_HasProperties";

						if (node->rhs.getType() == eNodeType::MethodCall) {
							MethodOrFunctionCall& mcall = static_cast<MethodOrFunctionCall&>(node->rhs);
							if (mcall.id.name == "add") {
								out << ".push_back(";

								mcall.arguments[0]->accept(this, out);

								out << ")";
								return;
							} else if (mcall.id.name == "count") {
								out << ".size()";
							}
						}

						return;
					} 
					



					else if (rhs_id.name == "Segments") {
						etn.lhs.accept(this, out);

						out << "->m_Segments";

						if (node->rhs.getType() == eNodeType::MethodCall) {
							MethodOrFunctionCall& mcall = static_cast<MethodOrFunctionCall&>(node->rhs);
							if (mcall.id.name == "add") {
								out << ".push_back(";

								mcall.arguments[0]->accept(this, out);

								out << ")";
								return;
							}
							else if (mcall.id.name == "count") {
								out << ".size()";
							}
						}

						return;
					}

					
					
					else if (rhs_id.name == "Name") {
						etn.lhs.accept(this, out);

						out << "->m_Name";

						if (node->rhs.getType() == eNodeType::Identifier) {
							out << "->m_value";
						}
						else
						{
							if (node->rhs.getType() == eNodeType::MethodCall)
							{
								MethodOrFunctionCall* m = (MethodOrFunctionCall*)&node->rhs;
								if (m->id.name == "getValue") {
									out << "->m_value";
								}
								else
								{

								}
							}
							else {
								assert(false);
							}

						}

						return;
					}
								
					
					
					
					else {
						std::cout << "muh";
					}
				}
			}
		}

		node->lhs.accept(this, out);

		// Add m_ before EXPRESS member variables...

		if (node->lhs.getType() == eNodeType::Identifier) {
			if (node->rhs.getType() == eNodeType::MethodCall) {
				out << "->";
				node->rhs.accept(this, out);
				return;
			}

			const Identifier& id = static_cast<Identifier&>(node->lhs);

			if (symbolTable_.isExpressType(id.name)) {
				// 				if (id.name == "writer") {
				// 					out << "->";
				// 					node->rhs.accept(this, out);
				// 					return;
				// 				} else {
				out << "->m_";
				node->rhs.accept(this, out);
				return;
				//}
			}
		}

		if (isEnum) {
			out << "::";
		} else if (node->lhs.getType() == eNodeType::ThisExpr) {
			out << "->";
		} else
			out << "->";

		node->rhs.accept(this, out);
	}

	virtual void visit(const Block* node, std::ostream& out) override {
		for (int i = 0; i < node->statements.size(); i++) {
			// out << "statement " << i << " " << typeid(node->statements[i]).name() << std::endl;

			node->statements[i]->accept(this, out);

			out << ";" << std::endl;
		}
	}

	virtual void visit(const ThrowStatement* node, std::ostream& out) override {
		out << "throw ";
		node->expr.accept(this, out);
		out << ";" << std::endl;
	}

	virtual void visit(const FunctionDeclaration* node, std::ostream& out) override {
		if (node->id.name == "main") {
			bFoundMainFunction = true;
		}

		if (node->OperationType == eOperation::Plus) {
			if (isReferenceType(node->type.name)) {
				out << "std::shared_ptr<" << node->type.name.c_str() << "> " << node->id.name.c_str() << "+(";
			} else
				out << node->type.name.c_str() << " " << node->id.name.c_str() << "+(";
		} else {
			out << node->type.name.c_str() << " " << node->id.name.c_str() << "(";
		}

		for (int i = 0; i < node->arguments.size(); i++) {
			if (node->id.name == "main") {
				out << "int argc,  char** argv";
				break;
			}
			
			if (i > 0) {
				out << ", ";
			}

			if (node->arguments[i]->isReferenceType) {
				out << node->arguments[i]->type.name.c_str() << "& " << node->arguments[i]->id.name.c_str();
			} 
			else if (isReferenceType(node->arguments[i]->type.name)) {
				out << "std::shared_ptr<" << node->arguments[i]->type.name.c_str() << "> " << node->arguments[i]->id.name.c_str();
			} else
				out << node->arguments[i]->type.name.c_str() << " " << node->arguments[i]->id.name.c_str();

			if ( startsWith( node->arguments[i]->type.name, "Ifc") ) {
				symbolDescription s;
				s.id = node->arguments[i]->id.name;
				s.typeName = node->arguments[i]->type.name;
				s.isExpressType = true;
				symbolTable_.add(s);
			}
		}

		out << ")" << std::endl;

		out << "{";

		// Copy arguments over to string[]
		if (node->id.name == "main" && node->arguments.size() == 1) {
			out << "std::shared_ptr<IfcPLArray<IfcPLString>> args = std::make_shared<IfcPLArray<IfcPLString>>(argc-1);" << std::endl;
			out << "for (int i = 1; i < argc; i++) {" << std::endl;
			out << "	args->operator[](i-1) = std::make_shared<IfcPLString>(argv[i]);" << std::endl;
			out << "}" << std::endl;
		}
		
		node->block.accept(this, out);
		out << "}";
	}

	virtual void visit(const PostfixExpression* node, std::ostream& out) override {
		out << node->id.name.c_str();

		if (node->op == eOperation::PlusPlus) {
			out << "++";
		}

		if (node->op == eOperation::MinusMinus) {
			out << "--";
		}
	}

	virtual void visit(const PrefixExpression* node, std::ostream& out) override {
		if (node->op == eOperation::PlusPlus) {
			out << "++";
		}

		if (node->op == eOperation::MinusMinus) {
			out << "--";
		}

		out << node->id.name.c_str();
	}

	virtual void visit(const CtorDeclaration* node, std::ostream& out) override {
		out << accessModfierToStr(node->access.value).c_str() << ":" << std::endl;

		out << node->id.name.c_str() << "(";

		for (int i = 0; i < node->arguments.size(); i++) {
			if (i > 0) {
				out << ", ";
			}

			node->arguments[i]->accept(this, out);
		}

		out << ")";

		if (node->hasBaseClassCtor) {
			assert(baseClasses_.contains(node->id.name));

			if (!baseClasses_.contains(node->id.name)) {
				throw std::runtime_error("base class not found");
			}

			out << ": " << baseClasses_.getValue(node->id.name) << "(";

			for (int i = 0; i < node->baseClassArguments.size(); i++) {
				if (i > 0) {
					out << ", ";
				}

				node->baseClassArguments[i]->accept(this, out);
			}

			out << ")";
		}

		out << std::endl;

		out << "{" << std::endl;

		node->block.accept(this, out);

		out << std::endl << "}" << std::endl;
	}

	virtual void visit(const MethodDeclaration* node, std::ostream& out) override {
		out << accessModfierToStr(node->access.value).c_str() << ":" << std::endl;

		if (node->isVirtual) {
			out << " virtual ";
		}

		if (node->isStatic) {
			out << " static";
		}

		if (node->type.name == "string") {
			out << "IfcPLString " << std::endl;
		} else if (isReferenceType(node->type.name)) {
			out << "std::shared_ptr<";
			out << node->type.name.c_str() << "> ";
		} else {
			out << " " << node->type.name.c_str() << " ";

			if (node->returnValueIsReference) {
				out << " & ";
			}
		}

		if (node->isOperator) {
			switch (node->OperatorType) {
			case eOperation::Plus: out << node->id.name.c_str() << "+("; break;
			case eOperation::Index: out << node->id.name.c_str() << "[]("; break;
			default: assert(false); break;
			}
		} else {
			out << node->id.name.c_str() << "(";
		}

		for (int i = 0; i < node->arguments.size(); i++) {
			if (startsWith(node->arguments[i]->id.name, "ifc")) {
				// seems to be a Ifc embedded type - add it to sym table
				symbolTable_.add(node->arguments[i]->id.name, node->type.name, true);
			}


			if (i > 0) {
				out << ", ";
			}

			if (node->arguments[i]->isConst) {
				out << "const ";
			}

			if (isReferenceType(node->arguments[i]->type.name)) {
				out << "std::shared_ptr<" << node->arguments[i]->type.name.c_str() << "> " << node->arguments[i]->id.name.c_str();
			} else {
				if (node->arguments[i]->isReferenceType) {
					out << node->arguments[i]->type.name.c_str() << "& " << node->arguments[i]->id.name.c_str();
				} else if (node->arguments[i]->type.name == "string") {
					out << "IfcPLString " << node->arguments[i]->id.name.c_str();
				} else {
					out << node->arguments[i]->type.name.c_str() << " " << node->arguments[i]->id.name.c_str();
				}
			}


			if (startsWith(node->arguments[i]->type.name, "Ifc")) {
				symbolDescription s;
				s.id = node->arguments[i]->id.name;
				s.typeName = node->arguments[i]->type.name;
				s.isExpressType = true;
				symbolTable_.add(s);
			}
		}

		out << ")";

		if (node->isConst) {
			out << " const";
		}

		if (node->isOverride) {
			out << " override";
		}

		out << std::endl;

		out << "{" << std::endl;

		node->block.accept(this, out);

		out << std::endl << "}" << std::endl;
	}

	virtual void visit(const ClassVariable* node, std::ostream& out) override {
		out << accessModfierToStr(node->access.value).c_str() << ":" << std::endl;

		if (isReferenceType(node->var_decl.type.name)) {
			out << "std::shared_ptr<" << node->var_decl.type.name << ">";
		} else {
			if (node->var_decl.type.name == "string") {
				out << "IfcPLString ";
			} else {
				if (node->isStatic) {
					out << "static ";
				}

				out << node->var_decl.type.name.c_str();
			}
		}

		if (node->var_decl.assignmentExpr) {
			if (node->isStatic) {
				out << " " << node->var_decl.id.name.c_str() << ";";
				// defer initialization....

				std::string lastClassName = classNames_.back();
				deferredOutput_ << node->var_decl.type.name << " " << lastClassName << "::" << node->var_decl.id.name.c_str() << " = ";
				node->var_decl.assignmentExpr->accept(this, deferredOutput_);
				deferredOutput_ << ";";

				// std::cout << deferedOutput_.str() << std::endl;
			} else {
				out << " " << node->var_decl.id.name.c_str() << " = ";
				node->var_decl.assignmentExpr->accept(this, out);
				out << ";";
			}
		} else {
			out << " " << node->var_decl.id.name.c_str() << ";" << std::endl;
		}

		if (startsWith(node->var_decl.type.name, "Ifc")) {
			symbolDescription s;
			s.id = node->var_decl.id.name;
			s.typeName = node->var_decl.type.name;
			s.isExpressType = true;
			symbolTable_.add(s);
		}
	}

	virtual void visit(const ClassArray* node, std::ostream& out) override {
		out << accessModfierToStr(node->access.value).c_str() << ":" << std::endl;

			out << "std::shared_ptr<IfcPLArray<" << node->array_decl.type.name << ">> " << node->array_decl.id.name;

			if (node->array_decl.ace) {
				out << " = ";

				node->array_decl.ace->accept(this, out);
			}

			out << ";" << std::endl;
	
	}

	virtual void visit(const IfStatement* node, std::ostream& out) override {
		out << "if(";

		node->conditionExpr->accept(this, out);

		out << ")" << std::endl;

		out << "{" << std::endl;

		node->thenExpr->accept(this, out);

		out << std::endl << "}" << std::endl;

		if (node->elseExpr) {
			out << "else" << std::endl;

			out << "{" << std::endl;

			node->elseExpr->accept(this, out);

			out << std::endl << "}" << std::endl;
		}
	}

	virtual void visit(const InstanceOfExpression* node, std::ostream& out) override {
		out << "IfcPLString(";
		node->instance_id.accept(this, out);
		out << "->classname())";
		out << " == ";
		out << "IfcPLString(\"";
		node->subtype_id.accept(this, out);
		out << "\")";
		out << std::endl;
	}

	virtual void visit(const CastExpression* node, std::ostream& out) override {
		out << "std::dynamic_pointer_cast<";
		node->id.accept(this, out);
		out << ">";
		out << "(";
		node->expression.accept(this, out);
		out << ")";
		out << std::endl;
	}

	virtual void visit(const BinaryOperator* node, std::ostream& out) override {
		node->lhs.accept(this, out);

		switch (node->op) {
		case eOperation::GE: out << ">="; break;
		case eOperation::EQ: out << "=="; break;
		case eOperation::Plus: out << "+"; break;
		case eOperation::Minus: out << "-"; break;
		case eOperation::Multiplication: out << "*"; break;
		case eOperation::Divde: out << "/"; break;
		case eOperation::GT: out << ">"; break;
		case eOperation::LT: out << "<"; break;
		case eOperation::Modulo: out << "%"; break;
		case eOperation::Assign: out << "="; break;
		case eOperation::NOTEQ: out << "!="; break;

		default:
			throw std::runtime_error("Operation not implemented");
			/* TODO comparison */
		}

		node->rhs.accept(this, out);
	}

	virtual void visit(const ReturnStatement* node, std::ostream& out) override {
		out << "return ";

		if (node->expression) {
			node->expression->accept(this, out);
		}
		
		out << ";";
	}

	virtual void visit(const Double* node, std::ostream& out) override {
		out << std::fixed << node->value;
	}

	virtual void visit(const Float* node, std::ostream& out) override {
		out << std::fixed << node->value << "f";
	}

	virtual void visit(const MethodOrFunctionCall* node, std::ostream& out) override {

		if (node->id.name == "getValue") {   // another hack - it would be better if oipExpress would provide a getValue method...
			out << "m_value";
			return;
		}

		if (node->id.name == "max" && node->arguments.size() == 2) {
			out << "std::max"
			    << "(";
		} else
			out << node->id.name.c_str() << "(";

		// Again - a hack - write method expcets std::string not a IFCPL string - currently ever write method with 2 arguments is changed
		if (node->id.name == "write" && node->arguments.size() == 2) {
			// write detected
			String* argument0 = static_cast<String*>(node->arguments[0]);
			out << argument0->value;
			out << ",";
			Identifier* argument1 = static_cast<Identifier*>(node->arguments[1]);
			out << argument1->name;
			out << ")";
			return;
		}

		for (int i = 0; i < node->arguments.size(); i++) {
			if (i > 0) {
				out << ", ";
			}

			node->arguments[i]->accept(this, out);
		}

		out << ")";
	}

	virtual void visit(const NewExpression* node, std::ostream& out) override {
		if (node->expr.getType() == eNodeType::MethodCall) {
			const MethodOrFunctionCall& mc = static_cast<const MethodOrFunctionCall&>(node->expr);

			out << "std::make_shared<" << mc.id.name << ">(";

			for (int i = 0; i < mc.arguments.size(); i++) {
				if (i > 0) {
					out << ", ";
				}

				mc.arguments[i]->accept(this, out);
			}

			out << ")";

			return;
		}
		assert(false);
		node->expr.accept(this, out);
	}

	virtual void visit(const ImportStatement* node, std::ostream& out) override {
		if (node->name == "ArbitraryTransitionCurve") {
			bIArbitraryTransitionCurveImport_ = true;
		}

		if (hasEnding(node->name, ".exp")) {
			// its an express schema
			if (g_SourceDirectory != "") {
				// run oipExpress

				std::string cmd = "C:/build/vs2017/x64/oipExpress/Debug/oipExpress.exe " + g_SourceDirectory + "/" + node->name + " -o " + g_OutputDirectory;

				std::string schemaName = node->name.substr(0, node->name.size() - 4);
				g_schemaName = schemaName;

				//"/model/Model.h\""
				out << "#include \"OpenInfraPlatform/" << schemaName << "/model/Model.h\"" << std::endl;
				out << "#include \"OpenInfraPlatform/" << schemaName << "/model/Exception.h\"" << std::endl;
				out << "#include \"OpenInfraPlatform/" << schemaName << "/reader/IfcStepReader.h\"" << std::endl;
				out << "#include \"OpenInfraPlatform/" << schemaName << "/writer/IfcStepWriter.h\"" << std::endl;
				out << "#include \"OpenInfraPlatform/" << schemaName << "/" << schemaName << "Entities.h\"" << std::endl;
				out << "#include \"OpenInfraPlatform/" << schemaName << "/" << schemaName << "Types.h\"" << std::endl;
				out << "#include \"OpenInfraPlatform/" << schemaName << "/reader/IfcStepReader.h\"" << std::endl;
				out << "#include \"OpenInfraPlatform/" << schemaName << "/reader/Reader.h\"" << std::endl;
				out << "#include \"OpenInfraPlatform/" << schemaName << "/reader/IfcStepReader.h\"" << std::endl;
				out << "#include \"OpenInfraPlatform/" << schemaName << "/model/UnitConverter.h\"" << std::endl;
				out << "#include \"OpenInfraPlatform/" << schemaName << "/entity/IfcCartesianPoint.h\"" << std::endl;
				out << "#include \"OpenInfraPlatform/" << schemaName << "/" << schemaName << "EntityEnums.h\"" << std::endl;

				out << std::endl;
				out << "using namespace OpenInfraPlatform::" << g_schemaName << ";" << std::endl;

				system(cmd.c_str());
			}
		} else {
			if (node->name == "Math") {
				out << "#include \""
				    << "IfcPLMath"
				    << ".h\"" << std::endl;
				bUseIfcPLMath = true;
			} else
				out << "#include \"" << node->name.c_str() << ".h\"" << std::endl;
		}
	}

	virtual void visit(const ModuleStatement* node, std::ostream& out) override {
		moduleName_ = node->name;
		out << "\/\/ module " << node->name.c_str() << ";\"" << std::endl;
	}

	virtual void visit(const ExpressionStatement* node, std::ostream& out) override {
		node->expression.accept(this, out);

		out << ";" << std::endl;
	}

	virtual void visit(const EnumerationDeclaration* node, std::ostream& out) override {
		enumNames_.push_back(node->id.name);

		out << "enum class " << node->id.name.c_str() << " {" << std::endl;

		for (int i = 0; i < node->body->memberDeclarations.size(); ++i) {
			if (i > 0) {
				out << ", ";
			}

			out << node->body->memberDeclarations[i].name.c_str() << std::endl;
		}

		out << "};" << std::endl;
	}

	virtual void visit(const InterfaceDeclaration* node, std::ostream& out) override {
		out << "class " << node->id.name.c_str();
		out << "{" << std::endl;

		if (node->body) {
			for (int i = 0; i < node->body->members_.size(); i++) {
				node->body->members_[i]->accept(this, out);
			}
		}

		out << "};" << std::endl;
	}

	virtual void visit(const ClassDeclaration* node, std::ostream& out) override {
		// collect all class names since they are needed for forward declarations
		classNames_.push_back(node->id.name);

		out << "class " << node->id.name.c_str();

		if (node->hasBaseClass) {
			out << " : public " << node->baseClass->name.c_str();

			baseClasses_.insert(node->id.name, node->baseClass->name);
		}

		out << std::endl;

		out << "{" << std::endl;

		if (node->body) {
			for (int i = 0; i < node->body->members_.size(); i++) {
				node->body->members_[i]->accept(this, out);
			}
		}

		out << "};" << std::endl;

		out << deferredOutput_.str();
		deferredOutput_.str("");
		deferredOutput_.clear();
	}

	virtual void visit(const String* node, std::ostream& out) override {
		out << "IfcPLString(" << node->value.c_str() << ")";
	}

	virtual void visit(const UsingStatement* node, std::ostream& out) override {
		out << "using namespace " << node->name.c_str() << ";" << std::endl;
	}

	virtual void visit(const ForStatement* node, std::ostream& out) override {
		out << "for (";

		if (node->initStatement) {
			node->initStatement->accept(this, out);
		}

		out << ";";

		if (node->condition) {
			node->condition->accept(this, out);
		}

		out << ";";

		if (node->counterStmt) {
			node->counterStmt->accept(this, out);
		}

		out << ")";

		out << std::endl;
		out << "{" << std::endl;

		if (node->body) {
			node->body->accept(this, out);
		}

		out << "}";
	}

	virtual void visit(const WhileStatement* node, std::ostream& out) override {
		out << "while (";

		if (node->condition) {
			node->condition->accept(this, out);
		}

		out << ")";

		out << std::endl;
		out << "{" << std::endl;

		if (node->body) {
			node->body->accept(this, out);
		}

		out << "}";
	}

	virtual void visit(const DoWhileStatement* node, std::ostream& out) override {
		out << "do {";

		if (node->body) {
			node->body->accept(this, out);
		}

		out << "}";

		out << std::endl;
		out << "while (" << std::endl;

		if (node->condition) {
			node->condition->accept(this, out);
		}

		out << ");";
	}

	virtual void visit(const ContinueStatement* node, std::ostream& out) override {
		out << "continue" << std::endl;
	}

	virtual void visit(const BreakStatement* node, std::ostream& out) override {
		out << "break" << std::endl;
	}

	virtual void visit(const SwitchStatement* node, std::ostream& out) override {
		out << "switch(";

		if (node->switchExpression) {
			node->switchExpression->accept(this, out);
		}

		out << ")";

		out << std::endl;
		out << "{" << std::endl;

		if (node->switchSections) {
			for (int i = 0; i < node->switchSections->switch_sections_.size(); i++) {
				if (node->switchSections->switch_sections_[i]->switchLabel->type == switchLabelType::Case) {
					out << "case ";

					node->switchSections->switch_sections_[i]->switchLabel->expression->accept(this, out);

					out << ":" << std::endl;
				} else {
					out << "default:" << std::endl;
				}

				if (node->switchSections->switch_sections_[i]->body) {
					node->switchSections->switch_sections_[i]->body->accept(this, out);
				}
			}
		}

		out << "};";
	}

	virtual void visit(const UnaryExpression* node, std::ostream& out) override {
		switch (node->operation) {
		case eOperation::Minus: out << "-"; break;

		case eOperation::LogicalNot: out << "!"; break;

		default: throw std::runtime_error("Undefined unary operation");
		}

		node->expression->accept(this, out);
	}

	virtual void visit(const TrinaryExpression* node, std::ostream& out) override {
		node->conditionExpr->accept(this, out);

		out << " ? ";

		node->thenExpr->accept(this, out);

		out << " : ";

		node->elseExpr->accept(this, out);
	}

	virtual void visit(const NullExpr* node, std::ostream& out) override {
		out << "nullptr" << std::endl;
	}

	std::string getModuleName() const {
		return moduleName_;
	}

	std::ostream& getCurrenOutputStream() {
		return out_;
	}

public:
	std::vector<std::string> getClassNames() const {
		return classNames_;
	}
	void setClassNames(std::vector<std::string> cs) {
		classNames_ = cs;
	}
	std::vector<std::string> getEnumNames() const {
		return enumNames_;
	}
	void setEnumNames(std::vector<std::string> cs) {
		enumNames_ = cs;
	}
	bool isEnumType(const std::string& name) {
		return std::find(enumNames_.begin(), enumNames_.end(), name) != enumNames_.end();
	}

	bool isReferenceType(const std::string& id) const {
		std::set<std::string> valueTypes = {"char", "int", "float", "double"};

		if (std::find(valueTypes.begin(), valueTypes.end(), id) != valueTypes.end()) {
			return false;
		}

		if (startsWith(id, "Ifc") || startsWith(id, "IFC")) // hack! all IFC Datatypes should be added to classNames_
		{
			return true;
		}

		if (id == "IVector2D")
			return true;

		if (id == "Vector2d")
			return true;

		if (id == "Vector3d")
			return true;

		if (id == "Vector4d")
			return true;

		if (id == "Matrix44d")
			return true;

		return std::find(classNames_.begin(), classNames_.end(), id) != classNames_.end();
	}

	void generateCMakeListsFile(const std::string& rootDirectory) {
		std::string name = "CMakeLists.txt";

		std::stringstream ssFilename;
		ssFilename << rootDirectory << "/" << name;
		std::ofstream file(ssFilename.str());

		file << "cmake_minimum_required(VERSION 3.10.0)" << std::endl;
		file << "project(" << moduleName_.c_str() << ")" << std::endl;
		file << "set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake/)" << std::endl;
		file << std::endl;
		file << "# Compiler settings" << std::endl;
		file << "if (WIN32)" << std::endl;
		file << "	# Standard: / DWIN32 / D_WINDOWS / W3 / Zm1000 / EHsc / GR" << std::endl;
		file << "	set(CMAKE_CXX_FLAGS \"/DWIN32 /D_WINDOWS /W4 /Zi /EHsc /GR /MP /openmp /bigobj\")" << std::endl;
		file << "endif(WIN32)" << std::endl;

		file << "set(Boost_USE_STATIC_LIBS   ON)" << std::endl;
		file << "find_package(Boost 1.65.1 COMPONENTS signals system filesystem  REQUIRED)" << std::endl;

		file << "find_package(Qt5Core 			5.10.0 REQUIRED)" << std::endl;
		file << "find_package(Qt5Xml 			5.10.0 REQUIRED)" << std::endl;
		file << "find_package(Qt5XmlPatterns 	5.10.0 REQUIRED)" << std::endl;
		file << "find_package(Qt5Widgets	 	5.10.0 REQUIRED)" << std::endl;
		file << "find_package(Qt5Network	 	5.10.0 REQUIRED)" << std::endl;
		file << "find_package(Qt5Svg		 	5.10.0 REQUIRED)" << std::endl;
		file << "find_package(Qt5PrintSupport 	5.10.0 REQUIRED)" << std::endl;
		file << "find_package(Qt5LinguistTools	5.10.0 REQUIRED)" << std::endl;

		file << "find_package(BlueFramework     REQUIRED)" << std::endl;

		file << "find_package(Eigen3 3.3.4 REQUIRED)" << std::endl;

		file << "# BlueFramework Copy Binaries" << std::endl;
		file << "BLUEFRAMEWORK_COPY_BINARIES(${CMAKE_BINARY_DIR})" << std::endl;

		if (g_schemaName != "") {
			file << "add_subdirectory(" << g_schemaName << ")" << std::endl;
		}

		file << std::endl;
		file << "include_directories(" << std::endl;
		file << "   "
		     << "${EIGEN3_INCLUDE_DIR}" << std::endl;
		file << "	" << g_schemaName << "/src/OpenInfraPlatform/IFC4X1" << std::endl;
		file << "	" << g_schemaName << "/src" << std::endl;

		file << "${Boost_INCLUDE_DIR}" << std::endl;
		file << "${BLUEFRAMEWORK_INCLUDE_DIR}" << std::endl;

		file << ")" << std::endl;

		file << std::endl;

		if (bFoundMainFunction) {
			file << "# Create a target for executable" << std::endl;
			file << "add_executable(" << moduleName_.c_str() << std::endl;
		} else {
			file << "add_library(" << moduleName_.c_str() << " SHARED" << std::endl;
			// file << "add_executable(" << moduleName_.c_str() << std::endl;
		}

		file << "\t" << moduleName_.c_str() << ".cpp" << std::endl;

		file << "\t"
		     << "IfcPLString.cpp" << std::endl;

		file << "\t"
		     << "Core.cpp" << std::endl;

		if (bUseIfcPLMath) {
			file << "\t"
			     << "IfcPLMath.cpp" << std::endl;
		}

		file << ")" << std::endl;

		file << "target_link_libraries(" << moduleName_.c_str() << std::endl;
		file << "# BlueFramework" << std::endl;
		file << "${BLUEFRAMEWORK_LIBRARIES}" << std::endl;
		file << ")" << std::endl;

		if (g_schemaName != "") {
			file << "target_link_libraries(" << moduleName_.c_str() << std::endl;
			file << "OpenInfraPlatform." << g_schemaName << ")" << std::endl;
		}
	}

private:
	bool bFoundMainFunction = false;

	stringstream deferredOutput_;

	SymbolTable symbolTable_;

	std::ostream& out_;

	bool bIArbitraryTransitionCurveImport_ = false;
	std::string moduleName_ = "main";

	Map<string, string> baseClasses_;
	std::vector<std::string> classNames_;
	std::vector<std::string> enumNames_;
	bool bUseIfcPLMath = false;
};

#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

int main(int argc, char** argv) {
	try {
		buw::CmdLine cmd("IfcPL Transpiler", ' ', "0.1");

		buw::UnlabeledValueArg<std::string> sourceFiles("source_files", "IFCPL Source files that should be compiled.", true, "./rectified", "string");
		cmd.add(sourceFiles);

		buw::ValueArg<std::string> outputDirectory("o", "outputDir", "The output directory.", false, "", "string");
		cmd.add(outputDirectory);

		// Parse the args.
		cmd.parse(argc, argv);

		const char* filename = sourceFiles.getValue().c_str();
		fs::path p(filename);
		g_SourceDirectory = p.parent_path().string();

		std::string outputDirectoryName = outputDirectory.getValue();
		g_OutputDirectory = outputDirectoryName;

		FILE* fp = fopen(filename, "r");

		if (!fp) {
			printf("couldn't open file for reading\n");
			exit(0);
		}

		yyin = fp;

		yyparse();
		fclose(fp);

		CppTransform cppTransformPass1(std::cout);
		programBlock->accept(&cppTransformPass1, cppTransformPass1.getCurrenOutputStream());
		cppTransformPass1.writeEpilog();

		{
			std::ofstream out;

			std::string filename = outputDirectoryName + "/" + cppTransformPass1.getModuleName() + ".cpp";

			out.open(filename);
			CppTransform cppTransformPass2(out);
			cppTransformPass2.setClassNames(cppTransformPass1.getClassNames());
			cppTransformPass2.setEnumNames(cppTransformPass1.getEnumNames());
			cppTransformPass2.writeClassForwardDeclarations();
			programBlock->accept(&cppTransformPass2, cppTransformPass2.getCurrenOutputStream());
			cppTransformPass2.writeEpilog();
			cppTransformPass2.generateCMakeListsFile(outputDirectoryName);
		}

		return 0;
	} catch (std::exception& ex) {
		std::cout << ex.what() << std::endl;
	}

	return 0;
}
