#include <iostream>
#include <cmath>
#include <string>
#include <fstream>
#include <stack>
#include <cstdlib>
#include <cctype>
#include <vector>
using namespace std;

class calculator{
	public:
		calculator(string temp): exp(temp){}
		void invalidChar(bool& valid);
		void checkParen(bool& valid);
		void CheckOperand(bool& valid);
		void evaluate();
		void execute(stack<double>& operand, stack<char>& op);
		int precedence(char c);
		double getresult(){ return result;}
		string getExp(){return exp;}
	private:
		double result;
		string exp;
};
void evaluateAndOutput(ostream& out, ostream& err, vector<calculator>& infix, int total);
namespace Error {
	struct unbalanced { }; 
	struct invalidchar{};
	struct invalidoperand { }; 
	struct invalidoperator { };
}


int main(int argc, char* argv[]){
	ifstream input;
	vector<calculator> infix;
	input.open(argv[1]);
	string temp;
	int total=0;
	if(input.fail()){
		cout<<"File could not be opened!\n";
		cout<<"Please enter your expressions (Enter ctrl + d to end):\n";
		while(getline(cin, temp)){
			infix.push_back(temp);
			total++;
		}
	}
	else{	
		while(getline(input, temp)){
			infix.push_back(temp);
			total++;
		}
	}
	input.close();
	evaluateAndOutput(cout, cerr, infix, total);
	return 0;
}

void calculator::invalidChar(bool& valid){
	int pos=0;
	while(pos<exp.length()){
		if(!isdigit(exp[pos]) && exp.at(pos)!=' ' && exp.at(pos)!='*' &&
			exp.at(pos)!='/' && exp.at(pos)!='.' && exp.at(pos)!='+' && 
			exp.at(pos)!='-' && exp.at(pos)!='^' && exp.at(pos)!='(' && 
			exp.at(pos)!=')' ){
				valid= false;
				throw Error::invalidchar();
			}
		pos++;
	}
	valid= true;
}

void calculator::checkParen(bool& valid){
	stack<char> paren;
	int pos= 0;
	while(pos<exp.length()){
		if(exp.at(pos)=='(')
			paren.push('(');
		else if(exp.at(pos)==')'){
			if(paren.empty()){
				valid= false;
				throw Error::unbalanced();
			}
			else
				paren.pop();
		}
		pos++;
	}
	if (!paren.empty()){
		valid= false;
		throw Error::unbalanced();
	}
	else 
		valid= true;
}

void calculator::CheckOperand(bool& valid){
	int pos=0;	
	int dot=0;
	bool hasDigit= false;
	string temp;
	stack<char> c;
	if(exp.length()==0){
		valid= false;
		throw Error::invalidoperator();
	}
	if(exp.at(pos)=='*'||exp.at(pos)=='/'||exp.at(pos)=='^'||
	  exp.at(pos)=='+'|| exp.at(pos)=='-'){
		valid= false;
		throw Error::invalidoperator();
	}
	while(pos<exp.length()){
		if (isdigit(exp[pos])||exp.at(pos)=='.'){
			if(exp.at(pos)=='.'){
				if(dot==1){
					valid= false;
					throw Error::invalidoperand();
				}
				else 
					dot=1;
			}
			temp.push_back(exp.at(pos));
			if ((pos<exp.length()-1)&& (exp.at(pos+1)==' '||exp.at(pos+1)=='*'||
			exp.at(pos+1)=='/'||exp.at(pos+1)=='+'|| exp.at(pos+1)=='-'||
			exp.at(pos+1)=='^'||exp.at(pos+1)=='('||exp.at(pos+1)==')')){	
				if((!c.empty()) &&(c.top()=='o')||exp.at(pos)=='.'){
					valid= false;
					throw Error::invalidoperand();
				}
				else{
					c.push('o');
					dot= 0;
					temp.clear();
					hasDigit= true;
				}					
			}
			else if ((pos==exp.length()-1)){
				if((!c.empty()) &&(c.top()=='o')||exp.at(pos)=='.'){
					valid= false;
					throw Error::invalidoperand();
				}
				else{
					c.push('o');
					dot= 0;
					temp.clear();
				}			
			}
		}
		else if (exp.at(pos)=='*'||exp.at(pos)=='/'||exp.at(pos)=='^'||
			exp.at(pos)=='+'|| exp.at(pos)=='-'){

			if((!c.empty()) &&(c.top()=='(')&&(exp.at(pos)=='*'||
			exp.at(pos)=='/'||exp.at(pos)=='^'||exp.at(pos)=='+'|| 
			exp.at(pos)=='-')){
					valid= false;
					throw Error::invalidoperator();
			}
			if((!c.empty()) &&(c.top()=='O')){
					valid= false;
					throw Error::invalidoperator();
			}
			else
				c.push('O');			
		}
		else if (exp.at(pos)==')'){
			if((!c.empty()) &&(c.top()=='O')){
					valid= false;
					throw Error::invalidoperator();
			}
		}
		else if (exp.at(pos)=='('){
			c.push('(');
		}
		pos++;
	}
	
	if(c.empty()||c.top()=='O'|| !hasDigit ){
		valid= false;
		throw Error::invalidoperator();
	}
	else
		valid= true;
}


void calculator::evaluate(){
	stack<double> operand;
	stack<char> op;
	int pos=0;	
	string temp;
	while(pos<exp.length()){
		if (isdigit(exp[pos])||exp.at(pos)=='.'){
			
			temp.push_back(exp.at(pos));
			if (pos==exp.length()-1){
				double tempf= stod(temp);
				operand.push(tempf);
			}
			else if (exp.at(pos+1)==' '||exp.at(pos+1)=='*'||
			exp.at(pos+1)=='/'||exp.at(pos+1)=='+'|| exp.at(pos+1)=='-'||
			exp.at(pos+1)=='^'||exp.at(pos+1)=='('||exp.at(pos+1)==')'){
				double tempf= stod(temp);
				operand.push(tempf);
				temp.clear();
			}
		}
		else if (exp.at(pos)=='(')
			op.push(exp.at(pos));
		else if (exp.at(pos)=='*'||exp.at(pos)=='/'||exp.at(pos)=='^'||
			exp.at(pos)=='+'|| exp.at(pos)=='-'){
			if((op.empty())||(precedence(exp.at(pos)) > 
									(precedence(op.top()))))
				op.push(exp.at(pos));
			else{
				while((!op.empty()) && (precedence(exp.at(pos)) <= 
									(precedence(op.top())))){
					if(op.top()=='(')
						break;
					execute(operand, op);
				}
				op.push(exp.at(pos));
			}
		}
		else if(exp.at(pos)==')'){
			while(op.top()!='(')
				execute(operand, op);
			op.pop();
		}
		pos++; 
	}
	while(!op.empty()){
		execute(operand, op);
	}
	result= operand.top();
}

void calculator::execute(stack<double>& operand, stack<char>& op){
	double temp2= operand.top();
	operand.pop();
	char oper= op.top();
	op.pop();
	double temp1= operand.top();
	operand.pop();

	if(oper=='*'){
		result= temp1 * temp2;
	}		
	else if(oper=='/'){
		result= temp1/temp2;
	}
	else if(oper=='^'){
		result= pow(temp1,temp2);
	}
	else if(oper=='+'){
		result= temp1 + temp2;
	}
	else if(oper=='-'){
		result= temp1 - temp2;
	}
	operand.push(result);	
}

int calculator::precedence(char c){
	if (c=='(')
		return 4;
	else if(c=='^')
		return 3;
	else if(c=='*')
		return 2;
	else if(c=='/')
		return 2;
	else if(c=='+')
		return 1;
	else if(c=='-')
		return 1;
	else 
		return 0;
}

void evaluateAndOutput(ostream& out, ostream& err, vector<calculator>& infix, int total){
	bool valid;
	cout<<"Following are the results:\n";
	for(int i=0; i< total; i++){
		try{
			infix[i].invalidChar(valid);
		}
		catch(Error::invalidchar){
			err<<infix[i].getExp()<<"\n";		
		}
		if(valid){
			try{
				infix[i].checkParen(valid);
			}
			catch(Error::unbalanced){
				err<<infix[i].getExp()<<"\n";		
			}
		}
		if(valid){
			try{
				infix[i].CheckOperand(valid);
			}
			catch(Error::invalidoperand){
				err<<infix[i].getExp()<<"\n";
			}
			catch(Error::invalidoperator){
				err<<infix[i].getExp()<<"\n";
			}
		}
		if (valid){
			infix[i].evaluate();
			out.setf(ios::fixed);
			out.setf(ios::showpoint);
			out.precision(3);
			out<<infix[i].getresult()<<" = "<<infix[i].getExp()<<endl;
		}
	}
}
