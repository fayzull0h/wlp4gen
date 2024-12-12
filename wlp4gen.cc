#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <map>
using namespace std;

#define NONE_T -1
#define INT_T 0
#define POINTER_T 1

ifstream file{"basic.ti"};

enum Token {
  LPAREN = -13,
  RPAREN,
  BOF,
  _EOF,
  INT,
  NUM,
  ID,
  LBRACE,
  RBRACE,
  LBRACK,
  RBRACK,
  WAIN,
  RETURN,
  COMMA,
  BECOMES,
  PLUS,
  MINUS,
  STAR,
  _NULL,
  AMP,
  NEW,
  DELETE,
  SLASH,
  PCT,
  IF,
  ELSE,
  WHILE,
  EQ,
  NE,
  LT,
  LE,
  GT,
  GE,
  PRINTLN,
  SEMI,
  start,
  arglist,
  procedures,
  procedure,
  _main,
  type,
  dcl,
  dcls,
  statement,
  statements,
  expr,
  factor,
  term,
  lvalue,
  test,
  
  EMPTY = 403,
  nothing = 404
};

Token str_token(const string &s) {
  if (s == "INT") return INT;
  else if (s == "LPAREN") return LPAREN;
  else if (s == "RPAREN") return RPAREN;
  else if (s == "LBRACE") return LBRACE;
  else if (s == "RBRACE") return RBRACE;
  else if (s == "LBRACK") return LBRACK;
  else if (s == "RBRACK") return RBRACK;
  else if (s == "ID") return ID;
  else if (s == "IF") return IF;
  else if (s == "ELSE") return ELSE;
  else if (s == "WHILE") return WHILE;
  else if (s == "NE") return NE;
  else if (s == "EQ") return EQ;
  else if (s == "GE") return GE;
  else if (s == "LE") return LE;
  else if (s == "LT") return LT;
  else if (s == "GT") return GT;
  else if (s == "WAIN") return WAIN;
  else if (s == "RETURN") return RETURN;
  else if (s == "SEMI") return SEMI;
  else if (s == "NUM") return NUM;
  else if (s == "BOF") return BOF;
  else if (s == "EOF") return _EOF;
  else if (s == "BECOMES") return BECOMES;
  else if (s == "MINUS") return MINUS;
  else if (s == "PLUS") return PLUS;
  else if (s == "STAR") return STAR;
  else if (s == "NULL") return _NULL;
  else if (s == "NEW") return NEW;
  else if (s == "DELETE") return DELETE;
  else if (s == "AMP") return AMP;
  else if (s == "SLASH") return SLASH;
  else if (s == "PCT") return PCT;
  else if (s == "COMMA") return COMMA;
  else if (s == "PRINTLN") return PRINTLN;
  else if (s == "start") return start;
  else if (s == "arglist") return arglist;
  else if (s == "procedures") return procedures;
  else if (s == "procedure") return procedure;
  else if (s == "main") return _main;
  else if (s == "type") return type;
  else if (s == "dcl") return dcl;
  else if (s == "dcls") return dcls;
  else if (s == "statement") return statement;
  else if (s == "statements") return statements;
  else if (s == "expr") return expr;
  else if (s == "lvalue") return lvalue;
  else if (s == "factor") return factor;
  else if (s == "term") return term;
  else if (s == "test") return test;
  else if (s == ".EMPTY") return EMPTY;
  return nothing;
}

class Tree {
 public:
  vector<Token> rule;
  vector<Tree *> children;
  string leaf;
  int number;
  static size_t id_count;
  int tree_id;
  int type;
  Tree(const vector<Token> &rule): rule{rule} { 
    number = 0;
    tree_id = ++id_count;
    type = NONE_T;
  }
  Tree(const vector<Token> &rule, const vector<Tree *> children): rule{rule}, children{children} {}

  ~Tree() {
    for (auto &child: children) {
      delete child;
    }
  }
};

size_t Tree::id_count = 0;

Tree * readInput(const string &rule, map<string, pair<int, int>> &vr_table) {
  // make the rule into a vector of words
  stringstream ss{rule};
  string curr;
  string next_terminal;
  vector<Token> rule_expansion;
  int type = NONE_T;
  while (ss >> curr) {
    if (curr == ":") {
      ss >> curr;
      if (curr == "int") type = INT_T;
      else type = POINTER_T;
      break;
    }
    rule_expansion.push_back(str_token(curr));
    next_terminal = curr;
  }

  Tree * child = new Tree{rule_expansion};
  child->type = type;

  // if rule is NUM x
  if (rule_expansion[0] == Token::NUM) {
    for (size_t i = 0; i < next_terminal.size(); ++i) {
      child->number = child->number * 10;
      child->number += (next_terminal[i] - '0');
    }
    child->leaf = next_terminal;
  }
  // if first word leads to a terminal, base case
  if (rule_expansion[0] <= Token::SEMI) {
    child->leaf = next_terminal;
  } 
  // or if the rule expands to empty
  else if (rule_expansion[1] == Token::EMPTY) {
    child->leaf = next_terminal;
  }
  /* otherwise, build a tree for every word in the expansion */
  else {
    for (size_t i = 1; i < rule_expansion.size(); ++i) {
      string next_rule;
      getline(cin, next_rule);

      Tree * next_child = readInput(next_rule, vr_table);
      child->children.push_back(next_child);
    }
  }

  // if curr rule is main, add param1 and 2 as reg 1 and 2 into var-reg table
  if (rule_expansion[0] == Token::_main) {
    string p1 = child->children[3]->children[1]->leaf;
    string p2 = child->children[5]->children[1]->leaf;
    vr_table[p1] = pair{1, 0};
    vr_table[p2] = pair{2, 0};
    child->type = child->children[3]->children[1]->type;
  }
  // if curr rule is dcl -> type ID, add ID to var-reg table
  else if (rule_expansion[0] == Token::dcls && rule_expansion[1] != Token::EMPTY) {
    string p = child->children[1]->children[1]->leaf;
    int num = child->children[3]->number;
    if (child->children[1]->children[1]->type == POINTER_T) num = 1;
    vr_table[p] = pair{-1, num};
  }
  return child;
}

void printTree(Tree * t) {
  if (t->leaf != "") cout << t->leaf << endl;
  for (auto &child: t->children) {
    printTree(child);
  }
}

void push(const int &reg) {
  cout << "sw $" << reg << ", -4($30)\n";
  cout << "sub $30, $30, $4\n";
}

void pop(const int &reg) {
  cout << "add $30, $30, $4\n";
  cout << "lw $" << reg << ", -4($30)\n";
}

void genCode(Tree * node, map<string, pair<int, int>> &vr_table) {
  if (node->rule[0] == Token::start) genCode(node->children[1], vr_table);
  else if (node->rule[0] == Token::procedures) genCode(node->children[0], vr_table);
  else if (node->rule[0] == Token::procedure) {
    cout << "FE" << node->children[1]->leaf << ":\n";

    // prologue section
    cout << "sub $29, $30, $4 ; setup frame pointer\n";
    int counter = 0;
    for (auto &x: vr_table) {
      // skip any declarations from the function parameters
      // they're already on the stack
      if (x.second.first == -2) continue;

      // push local function variables to the stack, make frame pointers
      int num = x.second.second;
      x.second.second = counter;

      cout << "lis $5\n";
      cout << ".word " << num << endl;
      cout << "sw $5, " << counter << "($29)";
      cout << " ; push variable " << x.first << endl;
      cout << "sub $30, $30, $4 ; update stack pointer\n";
      counter -= 4;
    }

    // push registers to save
    push(1);
    push(2);
    push(5);

    // code(dcls) (not a real thing)
    // genCode(node->children[6], vr_table);
    cout << endl;
    genCode(node->children[7], vr_table);
    genCode(node->children[9], vr_table);
    cout << endl;

    // pop saved registers
    pop(5);
    pop(2);
    pop(1);

    // pop local variables
    cout << "; epilogue\n";
    for (int i = counter; i < 0; i += 4) {
      cout << "add $30, $30, $4\n";
    }
    
    cout << "jr $31\n";
  }
  else if (node->rule[0] == Token::_main) {
    // prologue section
    cout << "; begin prologue\n";
    cout << ".import init\n";
    cout << ".import new\n";
    cout << ".import delete\n";
    cout << "lis $4\n";
    cout << ".word 4\n";
    cout << "lis $10\n";
    cout << ".word println\n";
    cout << "lis $11\n";
    cout << ".word 1\n";
    cout << "sub $29, $30, $4 ; setup frame pointer\n";
    int counter = 0;
    for (auto &x: vr_table) {
      int num = x.second.second;
      x.second.second = counter;
      // if it has a register
      if (x.second.first != -1) {
        cout << "sw $" << x.second.first << ", " << counter << "($29)";
      } 
      // if no register (ie declared variable)
      else {
        cout << "lis $5\n";
        cout << ".word " << num << endl;
        cout << "sw $5, " << counter << "($29)";
      }
      cout << " ; push variable " << x.first << endl;
      cout << "sub $30, $30, $4 ; update stack pointer\n";
      counter -= 4;
    }

    // initialize heap allocation mechanisms
    cout << endl;
    if (node->type == INT_T) {
      push(2);
      cout << "lis $2\n";
      cout << ".word 0\n";
    }
    push(29);
    push(31);
    cout << "lis $5\n";
    cout << ".word init\n";
    cout << "jalr $5\n";
    pop(31);
    pop(29);
    if (node->type == INT_T) pop(2);

    cout << endl;
    genCode(node->children[9], vr_table);
    genCode(node->children[11], vr_table);
    cout << endl;

    // epilogue section
    cout << "; begin epilogue\n";
    for (size_t i = 0; i < vr_table.size(); ++i) {
      cout << "add $30, $30, $4\n";
    }
    cout << "jr $31\n\n";
  }
  else if (node->rule[0] == Token::ID) {
    cout << "lw $3, " << vr_table.at(node->leaf).second << "($29) ; $3 <- " << node->leaf << endl;
  }
  else if (node->rule[0] == Token::NUM) {
    cout << "lis $3\n";
    cout << ".word " << node->leaf << endl;
  }
  else if (node->rule[0] == Token::term) { 
    if (node->children.size() == 1) genCode(node->children[0], vr_table);
    else {
      genCode(node->children[0], vr_table);
      push(3);
      genCode(node->children[2], vr_table);
      pop(5);
      if (node->rule[2] == Token::SLASH || node->rule[2] == Token::PCT) cout << "div $5, $3\n";
      else if (node->rule[2] == Token::STAR) cout << "mult $5, $3\n";

      cout << ((node->rule[2] == Token::PCT) ? "mfhi" : "mflo") << " $3\n";
    }
  }
  else if (node->rule[0] == Token::factor) {
    if (node->rule[1] == Token::LPAREN) genCode(node->children[1], vr_table);
    else if (node->rule[1] == Token::ID && node->rule.size() > 2) {
      push(29);
      push(31);

      // if there's arguments, push them to stack
      int argcount = 0;
      if (node->children.size() == 4) { 
        Tree * arglist = node->children[2];

        while (arglist->children.size() > 1) {
          genCode(arglist->children[0], vr_table);
          push(3);
          ++argcount;
          arglist = arglist->children[2];
        } 
        genCode(arglist->children[0], vr_table);
        push(3);
        ++argcount;
      }

      cout << "lis $5\n";
      cout << ".word FE" << node->children[0]->leaf << endl;
      cout << "jalr $5\n";

      // pop any arguments that were pushed to stack
      for (int i = 0; i < argcount; ++i) cout << "add $30, $30, $4\n";
      pop(31);
      pop(29);
    }
    else if (node->rule[1] == Token::_NULL) cout << "add $3, $0, $11 ; NULL\n";
    else if (node->rule[1] == Token::STAR) {
      genCode(node->children[1], vr_table);
      cout << "lw $3, 0($3)\n";
    }
    else if (node->rule[1] == Token::AMP) {
      Tree * lvalue = node->children[1];
      while (lvalue->rule[1] == Token::LPAREN) lvalue = lvalue->children[1];
      if (lvalue->rule[1] == Token::ID) {
        int offset = vr_table.at(lvalue->children[0]->leaf).second;
        cout << "lis $3\n";
        cout << ".word " << offset << endl;
        cout << "add $3, $3, $29\n";
      }
      else if (lvalue->rule[1] == Token::STAR) genCode(lvalue->children[1], vr_table);
    }
    else if (node->rule[1] == Token::NEW) {
      genCode(node->children[3], vr_table);
      cout << "add $1, $3, $0\n";
      push(31);
      cout << "lis $5\n.word new\njalr $5\n";
      pop(31);
      cout << "bne $3, $0, 1\n";
      cout << "add $3, $11, $0\n";
    }
    else genCode(node->children[0], vr_table);
  }
  else if (node->rule[0] == Token::expr) {
    if (node->children.size() == 1) genCode(node->children[0], vr_table);
    else {
      if (node->children[0]->type == node->children[2]->type) {
        genCode(node->children[0], vr_table);
        push(3);
        genCode(node->children[2], vr_table);
        pop(5);
      }
      else if (node->children[0]->type == POINTER_T && node->children[2]->type == INT_T) {
        genCode(node->children[0], vr_table);
        push(3);
        genCode(node->children[2], vr_table);
        cout << "mult $3, $4\n";
        cout << "mflo $3\n";
        pop(5);
      }
      else if (node->children[0]->type == INT_T && node->children[2]->type == POINTER_T) {
        genCode(node->children[0], vr_table);
        cout << "mult $3, $4\n";
        cout << "mflo $3\n";
        push(3);
        genCode(node->children[2], vr_table);
        pop(5);
      }

      if (node->rule[2] == Token::PLUS) cout << "add $3, $5, $3\n";
      else if (node->rule[2] == Token::MINUS) cout << "sub $3, $5, $3\n";

      if (node->children[0]->type == POINTER_T && node->children[2]->type == POINTER_T) {
        cout << "div $3, $4\n";
        cout << "mflo $3\n";
      }
    }
  }
  else if (node->rule[0] == Token::statement) {
    // IF statement
    if (node->rule[1] == Token::IF) {
      cout << "\n; evaluating if\n";
      genCode(node->children[2], vr_table);
      cout << "beq $3, $0, FEelse" << node->children[0]->tree_id << endl;
      genCode(node->children[5], vr_table);
      cout << "beq $0, $0, FEendif" << node->children[0]->tree_id << endl;
      cout << "FEelse" << node->children[0]->tree_id << ":\n";
      genCode(node->children[9], vr_table);
      cout << "FEendif" << node->children[0]->tree_id<< ":\n";
    }
    // WHILE statement
    else if (node->rule[1] == Token::WHILE) {
      cout << "FEloop" << node->children[0]->tree_id << ":\n";
      genCode(node->children[2], vr_table);
      cout << "beq $3, $0, FEendWhile" << node->children[0]->tree_id << endl;
      genCode(node->children[5], vr_table);
      cout << "beq $0, $0, FEloop" << node->children[0]->tree_id << endl;
      cout << "FEendWhile" << node->children[0]->tree_id << ":\n";
    }
    // PRINTLN statement
    else if (node->rule[1] == Token::PRINTLN) {
      // $3 <- result we want to print
      genCode(node->children[2], vr_table);

      // store $1 in $6
      cout << "add $6, $1, $0\n";

      // $1 <- result to print
      cout << "add $1, $3, $0\n";

      // save return address (?)
      cout << "sw $31, -4($30)\n";
      cout << "sub $30, $30, $4\n";

      // call println proc
      cout << "jalr $10\n";

      // reattain return address (?)
      cout << "add $30, $30, $4\n";
      cout << "lw $31, -4($30)\n";

      // put $1 back
      cout << "add $1, $6, $0\n";
    }
    else if (node->rule[1] == Token::DELETE) {
      genCode(node->children[3], vr_table);
      cout << "beq $3, $11, FEskipDelete" << node->tree_id << endl;
      cout << "add $1, $3, $0\n";
      push(31);
      cout << "lis $5\n.word delete\njalr $5\n";
      pop(31);
      cout << "FEskipDelete" << node->tree_id << ":\n";
    }
    // BECOMES statement
    else {
      Tree * lvalue = node->children[0];
      while (lvalue->rule[1] == Token::LPAREN) lvalue = lvalue->children[1];

      genCode(node->children[2], vr_table);

      // if lvalue -> ID
      if (lvalue->rule[1] == Token::ID) {
        int offset = vr_table.at(lvalue->children[0]->leaf).second;
        cout << "sw $3, " << offset << "($29)\n";
      }
      // if lvalue -> STAR factor
      else if (lvalue->rule[1] == Token::STAR) {
        push(3);
        genCode(lvalue->children[1], vr_table);
        pop(5);
        cout << "sw $5, 0($3)\n";
      }
    }
  }
  else if (node->rule[0] == Token::statements) {
    if (node->rule[1] == Token::EMPTY) return;
    genCode(node->children[0], vr_table);
    genCode(node->children[1], vr_table);
  }
  else if (node->rule[0] == Token::test) {
    Token second = node->rule[2];
    int idx1, idx2;
    if (second == Token::GT || second == Token::LE) {
      idx1 = 2;
      idx2 = 0;
    } else {
      idx1 = 0;
      idx2 = 2;
    }
    
    genCode(node->children[idx1], vr_table);
    push(3);
    genCode(node->children[idx2], vr_table);
    pop(5);

    string comparator = (node->children[0]->type == POINTER_T) ? "sltu" : "slt";

    if (second == Token::LT || second == Token::GT) cout << comparator << " $3, $5, $3\n";
    else if (second == Token::LE || second == Token::GE) {
      cout << comparator << " $3, $5, $3\n";
      cout << "sub $3, $11, $3\n";
    }
    else if (second == Token::NE || second == Token::EQ) {
      cout << comparator << " $6, $3, $5\n";
      cout << comparator << " $7, $5, $3\n";
      cout << "add $3, $6, $7\n";
      if (second == Token::EQ) {
        cout << "sub $3, $11, $3\n";
      }
    }
  }
}

int realFixVRTable(Tree * paramlist, map<string, pair<int, int>> &vr_table) {
  string varname = paramlist->children[0]->children[1]->leaf;
  
  // base case
  if (paramlist->children.size() == 1) {
    vr_table[varname] = pair{-2, 4};
    return 1;
  } else {
    int paramsleft = realFixVRTable(paramlist->children[2], vr_table);
    vr_table[varname] = pair{-2, 4 * (paramsleft + 1)};
    return paramsleft + 1;
  }
}

void fixVRTable(Tree * proc, map<string, pair<int, int>> &vr_table) {
  if (proc->children[3]->children.size() == 0) return;

  Tree * paramlist = proc->children[3]->children[0];
  realFixVRTable(paramlist, vr_table);
}

int main() {
  string input;
  
  // get rid of first two lines
  getline(cin, input);
  getline(cin, input);

  vector<pair<Tree *, map<string, pair<int, int>>>> all_procs;

  while (getline(cin, input)) {
    // if at the end
    if (input[0] == 'E') break;

    // get the procedure -> ... / main -> ... rule
    getline(cin, input);

    map<string, pair<int, int>> vr_table;
    Tree * proc = readInput(input, vr_table);
    all_procs.push_back(pair{proc, vr_table});
  }
  swap(all_procs[0], all_procs[all_procs.size()-1]);

  for (auto &x: all_procs) {
    if (x.first->rule[0] != Token::_main) fixVRTable(x.first, x.second);
    genCode(x.first, x.second);
  }

  for (auto &x: all_procs) {
    delete x.first;
  }

  ifstream printfile{"print.asm"};
  while (getline(printfile, input)) {
    cout << input << endl;
  }
}
