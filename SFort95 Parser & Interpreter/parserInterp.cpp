/* Implementation of Interpreter for the SFort95 Language
 * parserInterp.cpp
 * Programming Assignment 3
 * Spring 2024
*/

#include <stack>
#include "parserInterp.h"

map<string, bool> defVar; 
map<string, bool> initVar;
map<string, Token> SymTable;
map<string, Value> TempsResults; //Container of temporary locations of Value objects for results of expressions, variables values and constants 

queue <Value> * ValQue; //declare a pointer variable to a queue of Value objects
stack <Value> powStack; //A stack for use in performing POW operations.

namespace Parser {
	bool pushed_back = false;
	LexItem	pushed_token;

	static LexItem GetNextToken(istream& in, int& line) {
		if( pushed_back ) {
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(LexItem & t) {
		if( pushed_back ) {
			abort();
		}
		pushed_back = true;
		pushed_token = t;	
	}

}

static int error_count = 0;

int ErrCount()
{
    return error_count;
}

void ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}

bool IdentList(istream& in, int& line);


//Program is: Prog = PROGRAM IDENT {Decl} {Stmt} END PROGRAM IDENT
bool Prog(istream& in, int& line)
{
	bool dl = false, sl = false;
	LexItem tok = Parser::GetNextToken(in, line);
		
	if (tok.GetToken() == PROGRAM) {
		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == IDENT) {
			dl = Decl(in, line);
			if( !dl  )
			{
				ParseError(line, "Incorrect Declaration in Program");
				return false;
			}
			sl = Stmt(in, line);
			if( !sl  )
			{
				ParseError(line, "Incorrect Statement in program");
				return false;
			}	
			tok = Parser::GetNextToken(in, line);
			
			if (tok.GetToken() == END) {
				tok = Parser::GetNextToken(in, line);
				
				if (tok.GetToken() == PROGRAM) {
					tok = Parser::GetNextToken(in, line);
					
					if (tok.GetToken() == IDENT) {
						cout << "(DONE)" << endl;
						return true;
					}
					else
					{
						ParseError(line, "Missing Program Name");
						return false;
					}	
				}
				else
				{
					ParseError(line, "Missing PROGRAM at the End");
					return false;
				}	
			}
			else
			{
				ParseError(line, "Missing END of Program");
				return false;
			}	
		}
		else
		{
			ParseError(line, "Missing Program name");
			return false;
		}
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	
	else
	{
		ParseError(line, "Missing Program keyword");
		return false;
	}
}

//Decl ::= Type :: VarList 
//Type ::= INTEGER | REAL | CHARARACTER [(LEN = ICONST)] 
bool Decl(istream& in, int& line) {
	bool status = false;
	LexItem tok;
	LexItem idtok;
	string strLen;
	
	LexItem t = Parser::GetNextToken(in, line);
	
	if(t == INTEGER || t == REAL || t == CHARACTER ) {
		tok = t;
		//Keep track of the type.
		idtok = t;
		
		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == DCOLON) {
			status = VarList(in, line, idtok);
			
			if (status)
			{
				status = Decl(in, line);
				if(!status)
				{
					ParseError(line, "Declaration Syntactic Error.");
					return false;
				}
				return status;
			}
			else
			{
				ParseError(line, "Missing Variables List.");
				return false;
			}
		}
		else if(t == CHARACTER && tok.GetToken() == LPAREN)
		{
			tok = Parser::GetNextToken(in, line);
			
			if(tok.GetToken() == LEN)
			{
				tok = Parser::GetNextToken(in, line);
				
				if(tok.GetToken() == ASSOP)
				{
					tok = Parser::GetNextToken(in, line);
					
					if(tok.GetToken() == ICONST)
					{ 
						strLen = tok.GetLexeme();
						
						tok = Parser::GetNextToken(in, line);
						if(tok.GetToken() == RPAREN)
						{
							tok = Parser::GetNextToken(in, line);
							if(tok.GetToken() == DCOLON)
							{
								status = VarList(in, line, idtok, stoi(strLen));
								
								if (status)
								{
									//cout << "Definition of Strings with length of " << strLen << " in declaration statement." << endl;
									status = Decl(in, line);
									if(!status)
									{
										ParseError(line, "Declaration Syntactic Error.");
										return false;
									}
									return status;
								}
								else
								{
									ParseError(line, "Missing Variables List.");
									return false;
								}
							}
						}
						else
						{
							ParseError(line, "Missing Right Parenthesis for String Length definition.");
							return false;
						}
					
					}
					else
					{
						ParseError(line, "Incorrect Initialization of a String Length");
						return false;
					}
				}
			}
		}
		else
		{
			ParseError(line, "Missing Double Colons");
			return false;
		}
			
	}
		
	Parser::PushBackToken(t);
	return true;
}//End of Decl

//Stmt ::= AssigStmt | BlockIfStmt | PrintStmt | SimpleIfStmt
bool Stmt(istream& in, int& line) {
	bool status;
	
	LexItem t = Parser::GetNextToken(in, line);
	
	switch( t.GetToken() ) {

	case PRINT:
		status = PrintStmt(in, line);
		
		if(status)
			status = Stmt(in, line);
		break;

	case IF:
		status = BlockIfStmt(in, line);
		if(status)
			status = Stmt(in, line);
		break;

	case IDENT:
		Parser::PushBackToken(t);
        status = AssignStmt(in, line);
		if(status)
			status = Stmt(in, line);
		break;
		
	
	default:
		Parser::PushBackToken(t);
		return true;
	}

	return status;
}//End of Stmt

bool SimpleStmt(istream& in, int& line) {
	bool status;
	
	LexItem t = Parser::GetNextToken(in, line);
	
	switch( t.GetToken() ) {

	case PRINT:
		status = PrintStmt(in, line);
		
		if(!status)
		{
			ParseError(line, "Incorrect Print Statement");
			return false;
		}	
		cout << "Print statement in a Simple If statement." << endl;
		break;

	case IDENT:
		Parser::PushBackToken(t);
        status = AssignStmt(in, line);
		if(!status)
		{
			ParseError(line, "Incorrect Assignent Statement");
			return false;
		}
		cout << "Assignment statement in a Simple If statement." << endl;
			
		break;
		
	
	default:
		Parser::PushBackToken(t);
		return true;
	}

	return status;
}//End of SimpleStmt

//VarList ::= Var [= Expr] {, Var [= Expr]}
bool VarList(istream& in, int& line, LexItem & idtok, int strlen){
	bool status = false, exprstatus = false;
	string identstr;
	Value retVal;
	
	LexItem tok = Parser::GetNextToken(in, line);
	if(tok == IDENT)
	{
		
		identstr = tok.GetLexeme();
		if (!(defVar.find(identstr)->second))
		{
			defVar[identstr] = true;
		}	
		else
		{
			ParseError(line, "Variable Redefinition");
			return false;
		}
		
	}
	else
	{
		
		ParseError(line, "Missing Variable Name");
		return false;
	}

	//Set retVal type to the idtok type.
	switch (idtok.GetToken()){
		case INTEGER:
			retVal.SetType(VINT);
			break;
		case REAL:
			retVal.SetType(VREAL);
			break;
		case CHARACTER:
			retVal.SetType(VSTRING);
			break;
		default:
			cout << "Incorrect Type Error, Type is: " << idtok.GetLexeme() << endl;
			break;
	}
		
	tok = Parser::GetNextToken(in, line);
	if(tok == ASSOP)
	{
		exprstatus = Expr(in, line, retVal);
		if(!exprstatus)
		{
			ParseError(line, "Incorrect initialization for a variable.");
			return false;
		}

		//If retVal is of type string, adjust its string till its length matches the designated string length.
		if (retVal.GetType() == VSTRING){
			int retValLen = retVal.GetString().length();

			if (strlen > retValLen){
				for (int i = retValLen; i < strlen; i++){
					retVal.SetString(retVal.GetString() + ' ');
				}
			}
			else if (strlen < retValLen) {retVal.SetString((retVal.GetString()).substr(0, strlen));}
		}

		//cout << "retVal in VarList 1: " << retVal << endl;
		// cout<< "Initialization of the variable " << identstr <<" in the declaration statement." << endl;
		
		//Add variable name and value to TempsResults. Also add the variable to the list of initialized variables.
		TempsResults[identstr] = retVal;
		initVar[identstr] = true;
		
		//cout << identstr << " added to TempsResults with type " << retVal.GetType() << " and value " << retVal << " with strlen " << strlen << endl;

		tok = Parser::GetNextToken(in, line);
		
		if (tok == COMMA) {
			
			status = VarList(in, line, idtok, strlen);
			
		}
		else
		{
			Parser::PushBackToken(tok);
			return true;
		}
	}
	else if (tok == COMMA) {
		//Add variable name and value to TempsResults. If the type is of string make sure it has the proper length.
		if (retVal.GetType() == VSTRING){
			for (int i = 0; i < strlen; i++){
				retVal.SetString(retVal.GetString() + ' ');
			}
			initVar[identstr] = true;
		}
		TempsResults[identstr] = retVal;
		//cout << identstr << " added to TempsResults with type " << retVal.GetType() << " and value " << retVal << " with strlen " << strlen << endl;
		status = VarList(in, line, idtok, strlen);
	}
	else if(tok == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");
		
		return false;
	}
	else {
		//Add variable name and value to TempsResults. If the type is of string make sure it has the proper length.
		if (retVal.GetType() == VSTRING){
			for (int i = 0; i < strlen; i++){
				retVal.SetString(retVal.GetString() + ' ');
			}
			initVar[identstr] = true;
		}
		TempsResults[identstr] = retVal;
		//cout << identstr << " added to TempsResults with type " << retVal.GetType() << " and value " << retVal << " with strlen " << strlen << endl;

		Parser::PushBackToken(tok);
		return true;
	}
	
	return status;
	
}//End of VarList
	


//PrintStmt:= PRINT *, ExpreList 
bool PrintStmt(istream& in, int& line) {
	LexItem t;
	ValQue = new queue<Value>;
	
	
	t = Parser::GetNextToken(in, line);
	
 	if( t != DEF ) {
		
		ParseError(line, "Print statement syntax error.");
		return false;
	}
	t = Parser::GetNextToken(in, line);
	
	if( t != COMMA ) {
		
		ParseError(line, "Missing Comma.");
		return false;
	}
	bool ex = ExprList(in, line);
	
	if( !ex ) {
		ParseError(line, "Missing expression after Print Statement");
		return false;
	}
	
	while (!(*ValQue).empty())
	{
		Value nextVal = (*ValQue).front();
		cout << nextVal;
		ValQue->pop();
	}
	cout << endl;

	return ex;
}//End of PrintStmt

//BlockIfStmt:= if (Expr) then {Stmt} [Else Stmt]
//SimpleIfStatement := if (Expr) Stmt
bool BlockIfStmt(istream& in, int& line) {
	bool ex=false, status ; 
	static int nestlevel = 0;
	//int level;
	LexItem t;
	Value retVal;
	
	t = Parser::GetNextToken(in, line);
	if( t != LPAREN ) {
		
		ParseError(line, "Missing Left Parenthesis");
		return false;
	}
	
	ex = RelExpr(in, line, retVal);
	if( !ex ) {
		ParseError(line, "Missing if statement condition");
		return false;
	}
	
	t = Parser::GetNextToken(in, line);
	if(t != RPAREN ) {
		
		ParseError(line, "Run-Time Error-Illegal Type for If statement condition");
		return false;
	}
	
	t = Parser::GetNextToken(in, line);
	if(t != THEN)
	{
		Parser::PushBackToken(t);
		
		status = SimpleStmt(in, line);
		if(status)
		{
			return true;
		}
		else
		{
			ParseError(line, "If-Stmt Syntax Error");
			return false;
		}
		
	}
	nestlevel++;
	//level = nestlevel;

	//If the IF condition was not passed, skip tokens until reaching an ELSE. Otherwise proceed accordingly.
	//cout << "Value of retVal before if check: " << retVal << endl;
	if (!retVal.GetBool()){
		while (t.GetToken() != ELSE){
			t = Parser::GetNextToken(in, line);
		}
	}
	else{
		status = Stmt(in, line);
		if(!status)
		{
			ParseError(line, "Missing Statement for If-Stmt Then-Part");
			return false;
		}
		t = Parser::GetNextToken(in, line);
	}

	if( t == ELSE ) {
		status = Stmt(in, line);
		if(!status)
		{
			ParseError(line, "Missing Statement for If-Stmt Else-Part");
			return false;
		}
		else
		  t = Parser::GetNextToken(in, line);
		
	}
	
	
	if(t != END ) {
		
		ParseError(line, "Missing END of IF");
		return false;
	}
	t = Parser::GetNextToken(in, line);
	if(t == IF ) {
		//cout << "End of Block If statement at nesting level " << level << endl;
		return true;
	}	
	
	Parser::PushBackToken(t);
	ParseError(line, "Missing IF at End of IF statement");
	return false;
}//End of IfStmt function

//Var:= ident
bool Var(istream& in, int& line, LexItem & idtok)
{
	string identstr;
	
	if (idtok == IDENT){
		identstr = idtok.GetLexeme();
		
		if (!(defVar.find(identstr)->second))
		{
			ParseError(line, "Undeclared Variable");
			return false;
		}	
		return true;
	}
	else if(idtok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << idtok.GetLexeme() << ")" << endl;
		return false;
	}
	return false;
}//End of Var

//AssignStmt:= Var = Expr
bool AssignStmt(istream& in, int& line) {
	bool varstatus = false, status = false;
	LexItem idtok;
	LexItem t;
	Value retVal;
	Value varType;
	
	idtok = Parser::GetNextToken(in, line);
	varstatus = Var(in, line, idtok);
	
	if (varstatus){
		//Set retVal's type to that of the var. Save the type in varType.
		retVal.SetType(TempsResults.find(idtok.GetLexeme())->second.GetType());
		varType = retVal;

		//cout << retVal.GetType() << " <- retVal type in AssignStmt" << endl;

		t = Parser::GetNextToken(in, line);
		
		if (t == ASSOP){
			status = Expr(in, line, retVal);
			if(!status) {
				ParseError(line, "Missing Expression in Assignment Statment");
				return status;
			}

			//If the var type and expr type are different, return error.
			if (varType.GetType() != retVal.GetType()){
				line-=2;
				ParseError(line, "Illegal mixed-mode assignment operation");
				return false;
			}
			
		}
		else if(t.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << t.GetLexeme() << ")" << endl;
			return false;
		}
		else {
			ParseError(line, "Missing Assignment Operator");
			return false;
		}
	}
	else {
		ParseError(line, "Missing Left-Hand Side Variable in Assignment statement");
		return false;
	}

	//Update the variable value in TempsResults. If its a string adjust the string length accordingly.
	if (retVal.GetType() == VSTRING){
		int strlen = TempsResults.find(idtok.GetLexeme())->second.GetString().length();
		int retValLen = retVal.GetString().length();

		if (strlen > retValLen){
			for (int i = retValLen; i < strlen; i++){
				retVal.SetString(retVal.GetString() + ' ');
			}
		}
		else if (strlen < retValLen) {retVal.SetString((retVal.GetString()).substr(0, strlen));}
	}
	TempsResults[idtok.GetLexeme()] = retVal;
	initVar[idtok.GetLexeme()] = true;
	//cout << "Updated value of " << idtok.GetLexeme() << " in TempsResults: " << retVal << endl;

	return status;	
}//End of AssignStmt

//ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line) {
	bool status = false;
	Value retVal;

	status = Expr(in, line, retVal);
	if(!status){
		ParseError(line, "Missing Expression");
		return false;
	}
	ValQue->push(retVal);

	LexItem tok = Parser::GetNextToken(in, line);
	if (tok == COMMA) {

		status = ExprList(in, line);
		
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else{
		Parser::PushBackToken(tok);
		return true;
	}

	return status;
}//End of ExprList

//RelExpr ::= Expr  [ ( == | < | > ) Expr ]
bool RelExpr(istream& in, int& line, Value & retVal) {
	bool t1 = Expr(in, line, retVal);
	//cout << "retVal in RelExpr 1: " << retVal << endl;
	LexItem tok;
	LexItem opTok;
	Value leftVal;

	
	if( !t1 ) {
		return false;
	}
	
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	if ( tok == EQ || tok == LTHAN || tok == GTHAN) 
	{
		//Record left operand and operator.
		leftVal = retVal;
		opTok = tok;

		t1 = Expr(in, line, retVal);
		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		//cout << "retVal in RelExpr 2: " << retVal << endl;

		//Perform operation:
		switch (opTok.GetToken()){
			case EQ:
				retVal = leftVal==retVal;
				break;
			case LTHAN:
				retVal = leftVal<retVal;
				break;
			case GTHAN:
				retVal = leftVal>retVal;
				break;
			default:
				break;
		}
		//If retVal is not of type VBOOL, return ERROR.
		if (retVal.GetType() != VBOOL){
			ParseError(line, "Illegal operand types for a Relational operation");
			return false;
		}
		//cout << "retVal after boolean operation: " << retVal << endl;
	}
	
	return true;
}//End of RelExpr

//Expr ::= MultExpr { ( + | - | // ) MultExpr }
bool Expr(istream& in, int& line, Value & retVal) {
	
	bool t1 = MultExpr(in, line, retVal);
	LexItem tok;
	LexItem opTok;
	Value leftVal;
	
	if( !t1 ) {
		return false;
	}
	
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while ( tok == PLUS || tok == MINUS || tok == CAT) 
	{
		//Save the operator sign and value if initialized.
		opTok = tok;
		leftVal = retVal;

		t1 = MultExpr(in, line, retVal);
		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}

		//Perform the operation between values and save it as the new retVal.
		switch (opTok.GetToken()){
			case PLUS:
				retVal = leftVal+retVal;
				break;
			case MINUS:
				retVal = leftVal-retVal;
				break;
			case CAT:
				retVal = leftVal.Catenate(retVal);
				break;
			default:
				break;
		}
		
		tok = Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}		
		
	}
	Parser::PushBackToken(tok);

	return true;
}//End of Expr

//MultExpr ::= TermExpr { ( * | / ) TermExpr }
bool MultExpr(istream& in, int& line, Value & retVal) {
	
	bool t1 = TermExpr(in, line, retVal);
	LexItem tok;
	LexItem opTok;
	Value leftVal;
	
	if( !t1 ) {
		return false;
	}
	
	tok	= Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
	}
	while ( tok == MULT || tok == DIV  )
	{
		//Save the operator and left operand.
		opTok = tok;
		leftVal = retVal;

		t1 = TermExpr(in, line, retVal);
		if( !t1 ) {
			ParseError(line, "Missing operand after operator");
			return false;
		}

		//Perform the operation between values and save it as the new retVal.
		switch (opTok.GetToken()){
			case MULT:
				retVal = leftVal*retVal;
				break;
			case DIV:
				//If the right value is a 0, return error for dividing by 0.
				if ((retVal.GetType() == VREAL && retVal.GetReal() == 0.00) || (retVal.GetType() == VINT && retVal.GetInt() == 0)){
					line--;
					ParseError(line, "Run-Time Error-Illegal division by Zero");
					return false;
				}
				retVal = leftVal/retVal;
				break;
			default:
				break;
		}
		//cout << "retVal after operation: " << retVal << endl;
		
		tok	= Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}
		
	}
	Parser::PushBackToken(tok);
	return true;
}//End of MultExpr

//TermExpr ::= SFactor { ** SFactor }
bool TermExpr(istream& in, int& line, Value & retVal) {
	
	bool t1 = SFactor(in, line, retVal);
	LexItem tok;
	Value leftVal;
	Value rightVal;

	if( !t1 ) {
		return false;
	}
	
	tok	= Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
	}
	while ( tok == POW  )
	{
		//Push the values to the stack for as long as there are POW operations.
		//cout << "Pushing " << retVal << " to the stack." << endl;
		powStack.push(retVal);

		t1 = SFactor(in, line, retVal);
		if( !t1 ) {
			ParseError(line, "Missing exponent operand");
			return false;
		}

		tok	= Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}
	}

	//If a power operation took place: 
	while (!powStack.empty()){
		leftVal = powStack.top();
		powStack.pop();

		retVal = leftVal.Power(retVal);
	}

	Parser::PushBackToken(tok);
	return true;
}//End of TermExpr

//SFactor = Sign Factor | Factor
bool SFactor(istream& in, int& line, Value & retVal)
{
	LexItem t = Parser::GetNextToken(in, line);
	bool illegalType = (retVal.GetType() == VSTRING);

	//If the retVal is of type VSTRING it should not have a sign.
	bool status;
	int sign = 1;
	if(t == MINUS )
	{
		if (illegalType){
			ParseError(line, "Illegal Operand Type for Sign Operator");
			return false;
		}
		sign = -1;
	}
	else if(t == PLUS)
	{
		if (illegalType){
			ParseError(line, "Illegal Operand Type for Sign Operator");
			return false;
		}
		sign = 1;
	}
	else
		Parser::PushBackToken(t);
		
	status = Factor(in, line, sign, retVal);
	
	return status;
}//End of SFactor

//Factor := ident | iconst | rconst | sconst | (Expr)
bool Factor(istream& in, int& line, int sign, Value & retVal) {
	
	LexItem tok = Parser::GetNextToken(in, line);
	
	if(tok == IDENT) {
		
		//Check if the variable has defined and initialized.
		string lexeme = tok.GetLexeme();
		if (!(defVar.find(lexeme)->second))
		{
			ParseError(line, "Using Undefined Variable");
			return false;	
		}
		if (!(initVar.find(lexeme)->second))
		{
			ParseError(line, "Using Uninitialized Variable");
			return false;
		}
		
		switch ((TempsResults.find(tok.GetLexeme())->second).GetType()){
            case VINT:
                retVal.SetType(VINT);
                retVal.SetInt(TempsResults.find(tok.GetLexeme())->second.GetInt() * sign);
                break;
            case VREAL:
                retVal.SetType(VREAL);
                retVal.SetReal(TempsResults.find(tok.GetLexeme())->second.GetReal() * sign);
                break;
            case VSTRING:
                retVal.SetType(VSTRING);
                retVal.SetString(TempsResults.find(tok.GetLexeme())->second.GetString());
                break;
            default:
                break;
        }

		return true;
	}
	else if(tok == ICONST) {
		if (retVal.GetType() == VINT) {retVal.SetInt(stoi(tok.GetLexeme()) * sign);}
		else if (retVal.GetType() == VREAL) {retVal.SetReal(stod(tok.GetLexeme()) * sign);}
		//cout << "retVal in Factor ICONST: " << retVal << endl;
		return true;
	}
	else if(tok == SCONST) {
		retVal.SetType(VSTRING);
		retVal.SetString(tok.GetLexeme());
		return true;
	}
	else if(tok == RCONST) {
		retVal.SetType(VREAL);
		retVal.SetReal(stod(tok.GetLexeme()) * sign);
		return true;
	}
	else if( tok == LPAREN ) {
		bool ex = Expr(in, line, retVal);
		//cout << "retVal in Factor LPAREN: " << retVal << endl;
		if( !ex ) {
			ParseError(line, "Missing expression after (");
			return false;
		}
		if( Parser::GetNextToken(in, line) == RPAREN )
			return ex;
		else 
		{
			Parser::PushBackToken(tok);
			ParseError(line, "Missing ) after expression");
			return false;
		}
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}

	return false;
}



