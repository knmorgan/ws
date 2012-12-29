#include "parser.h"

#include <gmpxx.h>
#include <iostream>

parser::parser(const char* file, unsigned long limit) :
  exited(false),
  fname(file),
  stack_limit(limit) { }

parser::~parser() { }

void parser::parse(void) {
  lexer lex(fname);
  token t;

  while((t = lex.next_token()).type() != token::END) {
    parse_vec.push_back(t);
  }
}

void parser::build_label_index(void) {
  token t;
  std::vector<token>::iterator it;
  for(it = parse_vec.begin(); it != parse_vec.end(); ++it) {
    t = *it;
    if(t.op() == token::MARK) {
      if(labels.find(t.arg()) != labels.end()) {
        // Initially, an error was thrown here, meaning the
        // Whitespace source marked the same label twice.
        // However, it seems that some Whitespace programs
        // intentionally override labels during the lifetime
        // of the program, so we mark the first instance and
        // overwrite it in handle_flow_control.
      } else {
        labels[t.arg()] = it;
      }
    }
  }
}

void parser::execute(void) {
  parse();
  build_label_index();

  token t;
  for(command = parse_vec.begin(); command != parse_vec.end() && !exited; ++command) {
    t = *command; 
    switch(t.type()) {
      case token::STACK_MANIPULATION:
        handle_stack(t);
        break;
      case token::ARITHMETIC:
        handle_arithmetic(t);
        break;
      case token::HEAP_ACCESS:
        handle_heap(t);
        break;
      case token::FLOW_CONTROL:
        handle_flow_control(t);
        break;
      case token::IO:
        handle_io(t);
        break;
      default:
        throw generic_error("an unknown error occurred");
    }
  }
}

void parser::handle_stack(token t) {
  mpz_class temp1, temp2;
  switch(t.op()) {
    case token::PUSH:
      stack.push(t.arg());
      break;
    case token::DUPLICATE:
      stack.push(stack.top());
      break;
    case token::COPY_NTH:
      stack.copy(t.arg().get_si());
      break;
    case token::SWAP:
      stack.swap();
      break;
    case token::DISCARD:
      stack.pop();
      break;
    case token::SLIDE:
      stack.slide(t.arg().get_si());
      break;
    default:
      throw generic_error("an unknown error occurred");
  }
}

void parser::handle_arithmetic(token t) {
  mpz_class op1 = stack.top();
  stack.pop();
  mpz_class op2 = stack.top();
  stack.pop();

  switch(t.op()) {
    case token::ADD:
      stack.push(op2 + op1);
      break;
    case token::SUB:
      stack.push(op2 - op1);
      break;
    case token::MUL:
      stack.push(op2 * op1);
      break;
    case token::DIV:
      if(op1 == 0) throw generic_error("attempt to divide by zero");
      stack.push(op2 / op1);
      break;
    case token::MOD:
      if(op1 == 0) throw generic_error("attempt to divide by zero");
      stack.push(op2 % op1);
      break;
    default:
      throw generic_error("an unknown error occurred");
  }
}

void parser::handle_heap(token t) {
  mpz_class val, address;
  switch(t.op()) {
    case token::STORE:
      val = stack.top();
      stack.pop();
      address = stack.top();
      stack.pop();
      heap.store(address, val);
      break;
    case token::RETRIEVE:
      val = heap.get(stack.top());
      stack.pop();
      stack.push(val);
      break;
    default:
      throw generic_error("an unknown error occurred");
  }
}

void parser::handle_flow_control(token t) {
  switch(t.op()) {
    case token::MARK:
      labels[t.arg()] = command;
      break;
    case token::CALL:
      if(call_stack.size() > stack_limit) throw generic_error("stack overlow");
      call_stack.push(command);
      jump(t.arg());
      break;
    case token::JUMP:
      jump(t.arg());
      break;
    case token::JUMPIFZ:
      if(stack.top() == 0) {
        jump(t.arg());
      }
      stack.pop();
      break;
    case token::JUMPIFN:
      if(stack.top() < 0) {
        jump(t.arg());
      }
      stack.pop();
      break;
    case token::RETURN:
      jump(call_stack.top());
      call_stack.pop();
      break;
    case token::EXIT:
      exit();
      break;
    default:
      throw generic_error("an unknown error occurred");
  }
}

void parser::handle_io(token t) {
  mpz_class n;
  char c;
  switch(t.op()) {
    case token::OUTC:
      std::cout << static_cast<char>(stack.top().get_si());
      break;
    case token::OUTN:
      std::cout << stack.top();
      break;
    case token::READC:
      c = std::cin.get();
      heap.store(stack.top(), c);
      break;
    case token::READN:
      std::cin >> n;
      heap.store(stack.top(), n);
      break;
    default:
      throw generic_error("an unknown error occurred");
  }
  stack.pop();
}

void parser::jump(mpz_class l) {
  std::map<mpz_class,std::vector<token>::iterator>::iterator it = labels.find(l);
  if(it == labels.end()) {
    throw generic_error("attempting to jump to nonexistent label");
  }
  command = it->second;
}

void parser::jump(std::vector<token>::iterator pos) {
  command = pos;
}

void parser::exit(void) {
  exited = true;
}
