#include "parser.h"

#include <gmpxx.h>
#include <iostream>

parser::parser(const char* file, unsigned long limit) :
  exited_(false),
  fname_(file),
  stack_limit_(limit) { }

parser::~parser() { }

void parser::parse(void) {
  lexer lex(fname_);
  token t;

  while((t = lex.next_token()).type() != token::END) {
    parse_vec_.push_back(t);
  }
}

void parser::build_label_index(void) {
  token t;
  std::vector<token>::iterator it;
  for(it = parse_vec_.begin(); it != parse_vec_.end(); ++it) {
    t = *it;
    if(t.op() == token::MARK) {
      if(labels_.find(t.arg()) != labels_.end()) {
        // Initially, an error was thrown here, meaning the
        // Whitespace source marked the same label twice.
        // However, it seems that some Whitespace programs
        // intentionally override labels_ during the lifetime
        // of the program, so we mark the first instance and
        // overwrite it in handle_flow_control.
      } else {
        labels_[t.arg()] = it;
      }
    }
  }
}

void parser::execute(void) {
  parse();
  build_label_index();

  token t;
  for(command_ = parse_vec_.begin(); command_ != parse_vec_.end() && !exited_; ++command_) {
    t = *command_; 
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
      stack_.push(t.arg());
      break;
    case token::DUPLICATE:
      stack_.push(stack_.top());
      break;
    case token::COPY_NTH:
      stack_.copy(t.arg().get_si());
      break;
    case token::SWAP:
      stack_.swap();
      break;
    case token::DISCARD:
      stack_.pop();
      break;
    case token::SLIDE:
      stack_.slide(t.arg().get_si());
      break;
    default:
      throw generic_error("an unknown error occurred");
  }
}

void parser::handle_arithmetic(token t) {
  mpz_class op1 = stack_.top();
  stack_.pop();
  mpz_class op2 = stack_.top();
  stack_.pop();

  switch(t.op()) {
    case token::ADD:
      stack_.push(op2 + op1);
      break;
    case token::SUB:
      stack_.push(op2 - op1);
      break;
    case token::MUL:
      stack_.push(op2 * op1);
      break;
    case token::DIV:
      if(op1 == 0) throw generic_error("attempt to divide by zero");
      stack_.push(op2 / op1);
      break;
    case token::MOD:
      if(op1 == 0) throw generic_error("attempt to divide by zero");
      stack_.push(op2 % op1);
      break;
    default:
      throw generic_error("an unknown error occurred");
  }
}

void parser::handle_heap(token t) {
  mpz_class val, address;
  switch(t.op()) {
    case token::STORE:
      val = stack_.top();
      stack_.pop();
      address = stack_.top();
      stack_.pop();
      heap_.store(address, val);
      break;
    case token::RETRIEVE:
      val = heap_.get(stack_.top());
      stack_.pop();
      stack_.push(val);
      break;
    default:
      throw generic_error("an unknown error occurred");
  }
}

void parser::handle_flow_control(token t) {
  switch(t.op()) {
    case token::MARK:
      labels_[t.arg()] = command_;
      break;
    case token::CALL:
      if(call_stack_.size() > stack_limit_) throw generic_error("stack overlow");
      call_stack_.push(command_);
      jump(t.arg());
      break;
    case token::JUMP:
      jump(t.arg());
      break;
    case token::JUMPIFZ:
      if(stack_.top() == 0) {
        jump(t.arg());
      }
      stack_.pop();
      break;
    case token::JUMPIFN:
      if(stack_.top() < 0) {
        jump(t.arg());
      }
      stack_.pop();
      break;
    case token::RETURN:
      jump(call_stack_.top());
      call_stack_.pop();
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
      std::cout << static_cast<char>(stack_.top().get_si());
      break;
    case token::OUTN:
      std::cout << stack_.top();
      break;
    case token::READC:
      c = std::cin.get();
      heap_.store(stack_.top(), c);
      break;
    case token::READN:
      std::cin >> n;
      heap_.store(stack_.top(), n);
      break;
    default:
      throw generic_error("an unknown error occurred");
  }
  stack_.pop();
}

void parser::jump(mpz_class l) {
  std::map<mpz_class,std::vector<token>::iterator>::iterator it = labels_.find(l);
  if(it == labels_.end()) {
    throw generic_error("attempting to jump to nonexistent label");
  }
  command_ = it->second;
}

void parser::jump(std::vector<token>::iterator pos) {
  command_ = pos;
}

void parser::exit(void) {
  exited_ = true;
}
