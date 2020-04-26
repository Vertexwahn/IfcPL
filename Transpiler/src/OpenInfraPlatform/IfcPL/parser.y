%{
  #include "OpenInfraPlatform/IfcPL/node.h"
  #include <cstdio>
  #include <cstdlib>
  #include <sstream>
  using namespace OpenInfraPlatform::IfcPL;

  Block* programBlock; /* the top level root node of our final AST */

  extern int yylex();
  void yyerror(const char *s);

%}

/* Represents the many different ways we can access our data */
%union {
  OpenInfraPlatform::IfcPL::Node*                               node;
  OpenInfraPlatform::IfcPL::AccessModifier*                     access_modifier;
  OpenInfraPlatform::IfcPL::Block*                              block;
  OpenInfraPlatform::IfcPL::Expression*                         expr;
  OpenInfraPlatform::IfcPL::Statement*                          stmt;
  OpenInfraPlatform::IfcPL::Identifier*                         ident;
  OpenInfraPlatform::IfcPL::ClassMember*                        class_member;
  OpenInfraPlatform::IfcPL::MethodDeclaration*                  meth_decl;
  OpenInfraPlatform::IfcPL::ClassDeclaration*                   class_decl;
  OpenInfraPlatform::IfcPL::InterfaceDeclaration*				interface_decl;
  OpenInfraPlatform::IfcPL::InterfaceMethod*					interface_method;
  OpenInfraPlatform::IfcPL::EnumerationDeclaration*             enum_decl;
  OpenInfraPlatform::IfcPL::ClassBody*                          class_body;
  OpenInfraPlatform::IfcPL::InterfaceBody*                      interface_body;
  OpenInfraPlatform::IfcPL::EnumerationBody*                    enum_body;
  OpenInfraPlatform::IfcPL::VariableDeclaration*                var_decl;
  //OpenInfraPlatform::IfcPL::ListVariableDeclaration*
  OpenInfraPlatform::IfcPL::CollectionParamterDeclaration*      collection_parameter_decl;   
  OpenInfraPlatform::IfcPL::ArrayDeclaration*                   array_decl;
  OpenInfraPlatform::IfcPL::ArrayCreationExpression*            array_creation_expr;

  // switch
  OpenInfraPlatform::IfcPL::SwitchLabel*                        switch_label;
  OpenInfraPlatform::IfcPL::SwitchSection*                      switch_section;
  OpenInfraPlatform::IfcPL::SwitchSections*                     switch_sections;
  // enum

  std::vector<OpenInfraPlatform::IfcPL::VariableDeclaration*>*  varvec;
  std::vector<OpenInfraPlatform::IfcPL::Expression*>*           exprvec;
  std::string*                                                  string;
  int                                                           token;
}

/* Define our terminal symbols (tokens). This should
   match our tokens.l lex file. We also define the node type
   they represent.
 */
%token <string> TIDENIFIER INTEGER_LITERAL DOUBLE_LITERAL FLOAT_LITERAL ACCESS_MODIFIER TSTRING CHRLIT
%token <token> EQ NE LT LE GT GE '=' '?'
%token <token> '(' ')' '{' '}' ',' '.' ';'
%token <token> '+' '-' '*' '/'
%token <token> RETURN TEXTERN
%token <token> VIRTUAL REF BASE ENUM IF ELSE CLASS TSTATIC INTERFACE OVERRIDE CONST IMPORT MODULE CASE DEFAULT BREAK CONTINUE FOR WHILE DO SWITCH PLUSPLUS MINUSMINUS THROW CAST INSTANCEOF
%token <token> BOOL DOUBLE FLOAT ADDEQ SUBEQ MULEQ DIVEQ MODEQ NOTEQ THIS TNEW TNULL LIST SET BAG ARRAY

/* Define the type of node our nonterminal symbols represent.
   The types refer to the %union declaration above. Ex: when
   we call an ident (defined by union type ident) we are really
   calling an (NIdenIFier*). It makes the compiler happy.
 */
%type <ident> ident
%type <expr> numeric_literal expr string_literal char_literal prefix_expr postfix_expr unary_expr ternary_conditional_exp array_access_expr cast_expr instanceof_expr
%type <varvec> args_decl
%type <exprvec> call_args
%type <block> program stmts block
%type <class_body> class_body;
%type <interface_body> interface_body;
%type <stmt> stmt func_decl if_stmt import_stmt module_stmt  for_iterator  for_stmt while_stmt do_while_stmt switch_stmt break_stmt continue_stmt throw_stmt for_init_stmt collection_parameter_decl 
%type <class_decl> class_decl
%type <interface_decl> interface_decl;
%type <interface_method> interface_method;
%type <enum_decl> enum_decl
%type <enum_body> enum_body
%type <access_modifier> access_modifier
%type <var_decl> var_decl method_var_decl
%type <meth_decl> meth_decl
%type <class_member> class_member
%type <array_decl> array_decl
%type <array_creation_expr> array_creation_expr
%type <switch_section> switch_section
%type <switch_sections> switch_sections
%type <switch_label> switch_label

/* Operator precedence for mathematical operators */
%left EQ /* == */
%left '='
%left '%'
%left '+' '-'
%left '*' '/'
%left INSTANCEOF
%left '.'


%start program

%%

char_literal:
  CHRLIT {
    std::string charLiteral = $1->c_str();

    if( $1->c_str()[1] == '\\' ) {
        std::cout << charLiteral << std::endl;
        if(charLiteral == "'\\0'") {
            $$ = new Char( '\0' );
        } else if(charLiteral == "'\\1'") {
            $$ = new Char( '\1' );
        } else {
            assert(false);
        }
    }
    else {
        $$ = new Char( $1->c_str()[1] );
    }
  }
;

interface_decl:
        INTERFACE ident '{' '}' {
            $$ = new InterfaceDeclaration(*$2, nullptr);
        }
    |
        INTERFACE ident '{' interface_body '}' {
            $$ = new InterfaceDeclaration(*$2, std::shared_ptr<InterfaceBody>($4));
        }
;

class_decl:
    CLASS ident '{' '}' {
      $$ = new ClassDeclaration(*$2, nullptr);
    }
  |  CLASS ident '{' class_body '}' {
      $$ = new ClassDeclaration(*$2, std::shared_ptr<ClassBody>($4) );
    }
  |  CLASS ident ':' ident '{' '}' {
      $$ = new ClassDeclaration(*$2, nullptr );
      $$->hasBaseClass = true;
      $$->baseClass = std::shared_ptr<Identifier>($4);
    }
  |  CLASS ident ':' ident '{' class_body '}' {
      $$ = new ClassDeclaration(*$2, std::shared_ptr<ClassBody>($6) );
      $$->hasBaseClass = true;
      $$->baseClass = std::shared_ptr<Identifier>($4);
      }
;

throw_stmt:
    THROW TNEW expr {
        $$ = new ThrowStatement(*$3);
    }
;

stmt:
      array_decl ';'
    | RETURN expr ';' {
        $$ = new ReturnStatement($2);
      }
    | RETURN ';' {
        $$ = new ReturnStatement(nullptr);
      }
    | block
    | break_stmt ';'
    | class_decl
    | interface_decl
    | continue_stmt ';'
    | enum_decl
    | expr ';' {
        $$ = new ExpressionStatement(*$1);
      }
    | for_stmt
    | func_decl
    | if_stmt
    | import_stmt ';'
    | var_decl ';'
    | module_stmt ';'
    | while_stmt
    | do_while_stmt ';'
    | switch_stmt
    | throw_stmt ';'
;

program:
    stmts {
      programBlock = $1;
    }
;

stmts:
    stmt {
      $$ = new Block();
      $$->statements.push_back($<stmt>1);
    }
    |  stmts stmt {
      $1->statements.push_back($<stmt>2);
    }
;

module_stmt:
    MODULE ident {
      Identifier& id = *$2;
      $$ = new ModuleStatement(id.name);
    }
;

import_stmt:
    IMPORT ident {
      Identifier& id = *$2;
      $$ = new ImportStatement(id.name);
    }
  |  IMPORT ident '.' ident {
      Identifier& id = *$2;
      Identifier& id2 = *$4;
      std::stringstream ss;
      ss << id.name << "." << id2.name;
      $$ = new ImportStatement(ss.str());
    }
;

cast_expr:
    CAST '<' ident '>' '(' expr ')' {
        $$ = new CastExpression(*$3, *$6);
    }
;

instanceof_expr:
    expr INSTANCEOF ident {
        $$ = new InstanceOfExpression(*$1, *$3);
    } 
;

break_stmt:
  BREAK {
    $$ = new BreakStatement();
  }
;

continue_stmt:
  CONTINUE {
    $$ = new ContinueStatement();
  }
;

prefix_expr:
  PLUSPLUS ident {
    $$ = new PrefixExpression(*$2, eOperation::PlusPlus);
  }
;

postfix_expr:
  ident PLUSPLUS {
    $$ = new PostfixExpression(*$1, eOperation::PlusPlus);
  }
| ident MINUSMINUS {
    $$ = new PostfixExpression(*$1, eOperation::MinusMinus);
  }
;

class_body:
    class_member {
      $$ = new ClassBody();
      $$->members_.push_back(std::shared_ptr<ClassMember>($<class_member>1));
    }
    |  class_body class_member {
       $1->members_.push_back(std::shared_ptr<ClassMember>($<class_member>2));
    }
;

interface_body:
    interface_method {
      $$ = new InterfaceBody();
      $$->members_.push_back(std::shared_ptr<InterfaceMethod>($<interface_method>1));
    }
    |  interface_body interface_method {
       $1->members_.push_back(std::shared_ptr<InterfaceMethod>($<interface_method>2));
    }
;

interface_method:
        ident ident '(' args_decl ')' ';' {
          $$ = new InterfaceMethod(*$1, *$2, *$4);
        }
    |
        ident ident '(' args_decl ')' CONST ';' {
          $$ = new InterfaceMethod(*$1, *$2, *$4, true);
        }
;

class_member:
      meth_decl {
          $$ = $<meth_decl>1;
      }
    | access_modifier ident '(' args_decl ')' block {
          $$ = new CtorDeclaration(*$1, *$2, *$4, *$6);
    }
    | access_modifier ident '(' args_decl ')' ':' BASE '(' call_args ')' block {
          $$ = new CtorDeclaration(*$1, *$2, *$4, *$11, *$9);
    }
    | access_modifier TSTATIC var_decl ';' {
          $$ = new ClassVariable(*$1, *$3, true);
      }
    | access_modifier var_decl ';' {
          $$  = new ClassVariable(*$1, *$2);
      }
    | access_modifier array_decl ';' {
          $$  = new ClassArray(*$1, *$2);
      }
;

for_iterator:
    postfix_expr
  | prefix_expr
;

for_init_stmt:
      var_decl 
    | array_decl 
    | expr {
        $$ = new ExpressionStatement(*$1);
      }
;

for_stmt:
    FOR '(' for_init_stmt ';' expr ';' for_iterator ')' stmt {
      std::shared_ptr<Statement> initStatement = std::shared_ptr<Statement>($3);
      std::shared_ptr<Expression> condition = std::shared_ptr<Expression>($5);
      std::shared_ptr<Statement> counterStatement = std::shared_ptr<Statement>($7);
      std::shared_ptr<Statement> body = std::shared_ptr<Statement>($9);
      $$ = new ForStatement( initStatement, condition, counterStatement, body);
    }
  |  FOR '(' for_init_stmt ';' expr ';' for_iterator ')' '{' '}' {
      std::shared_ptr<Statement> initStatement = std::shared_ptr<Statement>($3);
      std::shared_ptr<Expression> condition = std::shared_ptr<Expression>($5);
      std::shared_ptr<Statement> counterStatement = std::shared_ptr<Statement>($7);
      std::shared_ptr<Block> body = nullptr;
      $$ = new ForStatement( initStatement, condition, counterStatement, body);
    }
;

while_stmt:
    WHILE '(' expr ')' stmts {
      std::shared_ptr<Expression> condition = std::shared_ptr<Expression>($3);
      std::shared_ptr<Block> body = std::shared_ptr<Block>($5);
      $$ = new WhileStatement( condition, body);
    }
  |   WHILE '(' expr ')' '{' '}' {
      std::shared_ptr<Expression> condition = std::shared_ptr<Expression>($3);
      std::shared_ptr<Block> body = nullptr;
      $$ = new WhileStatement( condition, body);
    }
;

do_while_stmt:
    DO '{' '}' WHILE '(' expr ')' {
      std::shared_ptr<Block> body = nullptr;
      std::shared_ptr<Expression> condition = std::shared_ptr<Expression>($6);
      $$ = new DoWhileStatement( body, condition);
    }
  |  DO '{' stmts '}' WHILE '(' expr ')' {
      std::shared_ptr<Block> body = std::shared_ptr<Block>($3);
      std::shared_ptr<Expression> condition = std::shared_ptr<Expression>($7);
      $$ = new DoWhileStatement( body, condition);
    }
;

switch_stmt:
    SWITCH '(' expr ')' '{' '}' {
      std::shared_ptr<Expression> switchExpression = std::shared_ptr<Expression>($3);
      $$ = new SwitchStatement(switchExpression, nullptr);
    }
  |  SWITCH '(' expr ')' '{' switch_sections '}' {
    std::shared_ptr<Expression> switchExpression = std::shared_ptr<Expression>($3);
    std::shared_ptr<SwitchSections> switchSections = std::shared_ptr<SwitchSections>($6);
    $$ = new SwitchStatement(switchExpression, switchSections);
    }
;

switch_sections:
    switch_section {
      $$ = new SwitchSections();
      $$->addSwitchSection(std::shared_ptr<SwitchSection>($1));
    }
  |  switch_sections switch_section {
      $1->addSwitchSection(std::shared_ptr<SwitchSection>($2));
    }
;

switch_section:
  switch_label stmts {
    std::shared_ptr<SwitchLabel> switchLabel = std::shared_ptr<SwitchLabel>($1);
    std::shared_ptr<Block> body = std::shared_ptr<Block>($2);
    $$ = new SwitchSection(switchLabel, body);
  }
;

switch_label:
    CASE expr ':' {
      std::shared_ptr<Expression> switchExpression = std::shared_ptr<Expression>($2);
      $$ = new SwitchLabel(switchLabelType::Case, switchExpression);
    }
  |  DEFAULT ':' {
      $$ = new SwitchLabel(switchLabelType::Default, nullptr);
    }
;

if_stmt:
    IF '(' expr ')' stmt ELSE stmt {
      $$ = new IfStatement(
        std::unique_ptr<Expression>($3),
        std::unique_ptr<Statement>($5),
        std::unique_ptr<Statement>($7)
      );
    }
  |  IF '(' expr ')' stmt  {
      $$ = new IfStatement(
        std::unique_ptr<Expression>($3),
        std::unique_ptr<Statement>($5),
        std::unique_ptr<Statement>(nullptr)
      );
    }
;

block: 
    '{' stmts '}' { 
	   $$ = $2; 
	}
  | '{' '}' { 
		$$ = new Block();
	}
;

method_var_decl:
    ident ident {
      $$ = new VariableDeclaration(*$1, *$2);
    }
   |  CONST ident ident {
      auto vd =  new VariableDeclaration(*$2, *$3);
      vd->isConst = true;
      $$ = vd;
    }
   |  REF ident ident {
      auto vd =  new VariableDeclaration(*$2, *$3);
      vd->isConst = false;
	  vd->isReferenceType = true;
      $$ = vd;
    }
   | ident '[' ']' ident {
	  $$ = new ArrayDeclaration(*$1, *$4, nullptr);
   }
;

var_decl:
        ident ident {
            $$ = new VariableDeclaration(*$1, *$2);
        }
   |    ident ident '=' expr {
            $$ = new VariableDeclaration(*$1, *$2, $4);
        }
   |    REF ident ident '=' expr {
            $$ = new VariableDeclaration(*$2, *$3, $5);
            $$->isReferenceType = true;
        }
   |    CONST ident ident '=' expr {
            auto vd = new VariableDeclaration(*$2, *$3, $5);
            vd->isConst = true;
            $$ = vd;
        }
   |   LIST '<' collection_parameter_decl '>' ident '=' TNEW LIST '<' collection_parameter_decl '>' '(' ')'  {
			$$ = new CollectionVariableDeclaration(*$3, *$5, eCollectionType::List);
		}
   |   BAG '<' collection_parameter_decl '>' ident '=' TNEW BAG '<' collection_parameter_decl '>' '(' ')'  {
			$$ = new CollectionVariableDeclaration(*$3, *$5, eCollectionType::Bag);
		}
	|  SET '<' collection_parameter_decl '>' ident '=' TNEW SET '<' collection_parameter_decl '>' '(' ')'  {
			$$ = new CollectionVariableDeclaration(*$3, *$5, eCollectionType::Set);
		}
	|  ARRAY '<' collection_parameter_decl '>' ident '=' TNEW ARRAY '<' collection_parameter_decl '>' '(' ')'  {
		$$ = new CollectionVariableDeclaration(*$3, *$5, eCollectionType::Array);
	}
;

collection_parameter_decl:
		ident {
			$$ = new CollectionParamterDeclaration(std::shared_ptr<Identifier>($1));
		}
	|   LIST '<' collection_parameter_decl '>' {
			$$ = new CollectionParamterDeclaration(std::shared_ptr<Statement>($3), eCollectionType::List );
		}
	|   BAG '<' collection_parameter_decl '>' {
		$$ = new CollectionParamterDeclaration(std::shared_ptr<Statement>($3), eCollectionType::Bag );
	}
	|   SET '<' collection_parameter_decl '>' {
		$$ = new CollectionParamterDeclaration(std::shared_ptr<Statement>($3), eCollectionType::Set);
	}
	|   ARRAY '<' collection_parameter_decl '>' {
		$$ = new CollectionParamterDeclaration(std::shared_ptr<Statement>($3), eCollectionType::Array );
	}
;

array_decl:
    ident '[' ']' ident {
        $$ = new ArrayDeclaration(*$1, *$4, nullptr);
    }
  | ident '[' ']' ident '=' array_creation_expr {
        $$ = new ArrayDeclaration(*$1, *$4, $6);
    }
;

array_creation_expr:
    TNEW ident '[' expr ']' {
        $$ = new ArrayCreationExpression(true, *$2, *$4);
    }
;

array_access_expr:
        ident '[' expr ']' '=' expr {
            $$ = new ArrayAccess(*$1, *$3, $6);
        }
    |	ident '[' expr ']' {
            $$ = new ArrayAccess(*$1, *$3, nullptr);
        }
;

func_decl:
        ident ident '(' args_decl ')' block {
          $$ = new FunctionDeclaration(*$1, *$2, *$4, *$6);
        }
    |	ident ident '+' '(' args_decl ')' block {
            FunctionDeclaration* fd = new FunctionDeclaration(*$1, *$2, *$5, *$7);
            fd->OperationType = eOperation::Plus;
            $$ = fd;
        }
;

meth_decl:
    access_modifier VIRTUAL ident ident '(' args_decl ')' block {
      $$ = new MethodDeclaration(*$1, *$3, *$4, *$6, *$8, false); delete $6;
      $$->isVirtual = true;
    }
  | access_modifier REF ident ident '(' args_decl ')' block {
      $$ = new MethodDeclaration(*$1, *$3, *$4, *$6, *$8, false); delete $6;
      $$->returnValueIsReference = true;
    }
  | access_modifier REF ident ident '[' ']' '(' args_decl ')' block {
      $$ = new MethodDeclaration(*$1, *$3, *$4, *$8, *$10, false); delete $8;
      $$->returnValueIsReference = true;
      $$->isOperator = true;
      $$->OperatorType = eOperation::Index;
    }
  | access_modifier ident ident '+' '(' args_decl ')' block {
      $$ = new MethodDeclaration(*$1, *$2, *$3, *$6, *$8, false); delete $6;
      $$->isOperator = true;
      $$->OperatorType = eOperation::Plus;
    }
  | access_modifier ident ident '-' '(' args_decl ')' block {
      $$ = new MethodDeclaration(*$1, *$2, *$3, *$6, *$8, false); delete $6;
      $$->isOperator = true;
      $$->OperatorType = eOperation::Minus;
    }
  | access_modifier ident ident '*' '(' args_decl ')' block {
      $$ = new MethodDeclaration(*$1, *$2, *$3, *$6, *$8, false); delete $6;
      $$->isOperator = true;
      $$->OperatorType = eOperation::Multiplication;
    }
  | access_modifier ident ident '(' args_decl ')' block {
      $$ = new MethodDeclaration(*$1, *$2, *$3, *$5, *$7, false); delete $5;
    }
  |  access_modifier TSTATIC ident ident '(' args_decl ')' block {
      $$ = new MethodDeclaration(*$1, *$3, *$4, *$6, *$8, true); delete $6;
    }
  |  access_modifier ident ident '(' args_decl ')' OVERRIDE block {
      $$ = new MethodDeclaration(*$1, *$2, *$3, *$5, *$8, false); delete $5;
      $$->isOverride = true;
    }
  |  access_modifier ident ident '(' args_decl ')' CONST OVERRIDE block {
      $$ = new MethodDeclaration(*$1, *$2, *$3, *$5, *$9, false); delete $5;
      $$->isConst = true;
      $$->isOverride = true;
    }
  |  access_modifier ident ident '(' args_decl ')' CONST block {
    $$ = new MethodDeclaration(*$1, *$2, *$3, *$5, *$8, false); delete $5;
    $$->isConst = true;
  }
;

args_decl:
    /*intentionally_blank*/  {
      $$ = new VariableList();
    }
  |  method_var_decl {
      $$ = new VariableList(); $$->push_back($<var_decl>1);
    }
  |  args_decl ',' method_var_decl {
      $1->push_back($<var_decl>3);
    }
;

ident:
    TIDENIFIER {
      $$ = new Identifier(*$1); delete $1;
    }
;

numeric_literal:
    INTEGER_LITERAL {
      $$ = new Integer(atol($1->c_str())); delete $1;
    }
  |  FLOAT_LITERAL {
      $$ = new Float(atof($1->c_str())); delete $1;
    }
  |  DOUBLE_LITERAL {
      $$ = new Double(atof($1->c_str())); delete $1;
    }
;

string_literal:
    TSTRING {
      $$ = new String( $1->c_str() );
    }
;

access_modifier:
    ACCESS_MODIFIER {
      $$ = new AccessModifier( strToAccessModfier($1->c_str()) );
    };

unary_expr:
    '-' expr {
      auto p = std::shared_ptr<Expression>($2);
      $$ = new UnaryExpression(eOperation::Minus, p);
    }
  | '!' expr {
      auto p = std::shared_ptr<Expression>($2);
      $$ = new UnaryExpression(eOperation::LogicalNot, p);
    }
;

expr:
    TNEW expr {
      $$ = new NewExpression(*$2);
    }
  |
    TNULL {
      $$ = new NullExpr();
    }
  |  THIS {
      $$ = new ThisExpr();
    }
  | expr '.' expr {
      $$ = new EnumTypeName(*$1, *$3);
    }

  |  expr ADDEQ expr {
      $$ = new Assignment(*$1, eAssignmentOperator::ADDEQ,*$3);
    }
  |   expr MULEQ expr {
      $$ = new Assignment(*$1, eAssignmentOperator::MULEQ,*$3);
    }
  |  ident '(' call_args ')' {
      $$ = new MethodOrFunctionCall(*$1, *$3); delete $3;
    }
  |  ident {
      $<ident>$ = $1;
      //$$ = new Identifier(*$1); 
    }
  |  numeric_literal
  |  string_literal
  |  char_literal
  |  expr '%' expr {
      $$ = new BinaryOperator(*$1, eOperation::Modulo, *$3);
    }

  |  expr '=' expr {
      $$ = new BinaryOperator(*$1, eOperation::Assign, *$3);
    }
  |  expr '*' expr {
      $$ = new BinaryOperator(*$1, eOperation::Multiplication, *$3);
    }
  |  expr '/' expr {
      $$ = new BinaryOperator(*$1, eOperation::Divde, *$3);
    }
  |  expr '+' expr {
      $$ = new BinaryOperator(*$1, eOperation::Plus, *$3);
    }
  |  expr '-' expr {
      $$ = new BinaryOperator(*$1, eOperation::Minus, *$3);
    }
  |  expr EQ expr {
      $$ = new BinaryOperator(*$1, eOperation::EQ, *$3);
    }
  |  expr NOTEQ expr {
      $$ = new BinaryOperator(*$1, eOperation::NOTEQ, *$3);
    }
  |  expr GE expr {
      $$ = new BinaryOperator(*$1, eOperation::GE, *$3);
    }
  |  expr '<' expr {
      $$ = new BinaryOperator(*$1, eOperation::LT, *$3);
    }
  |  expr '>' expr {
      $$ = new BinaryOperator(*$1, eOperation::GT, *$3);
    }
  |  '(' expr ')' {
      $$ = $2;
    }
  | ternary_conditional_exp
  | unary_expr
  | postfix_expr
  | prefix_expr
  | array_access_expr
  | cast_expr 
  | instanceof_expr
;

ternary_conditional_exp:
  expr '?' expr ':' expr {
    auto conditionExpr = std::shared_ptr<Expression>($1);
    auto thenExpr = std::shared_ptr<Expression>($3);
    auto elseExpr = std::shared_ptr<Expression>($5);
    $$ = new TrinaryExpression(conditionExpr, thenExpr, elseExpr);
  }
;

call_args : /*blank*/  { $$ = new ExpressionList(); }
  |  expr {
      $$ = new ExpressionList(); $$->push_back($1);
    }
    |  call_args ',' expr  {
      $1->push_back($3);
    }
;

enum_decl:
  ENUM ident '{' enum_body '}' {
    $$ = new EnumerationDeclaration( *$2, std::shared_ptr<EnumerationBody>($4) );
  }
;

enum_body:
    ident {
      $$ = new EnumerationBody();
      $$->addMemberDeclaration( *$1 );
    }
  |  enum_body ',' ident  {
      $1->addMemberDeclaration( *$3 );
    }
;

%%
