#include "lexer.h"

lexer::lexer(const char* file, int size) {
  stream_.open(file, std::ifstream::in | std::ifstream::binary);
  if(!stream_) {
    throw file_not_found(file);
  }

  buf_ = new char[size];
  buf_size_ = size;
  ptr_ = size;
}

lexer::~lexer(void) {
  stream_.close();
  delete[] buf_;
}

token lexer::next_token(void) {
  switch(next()) {
    case SPACE: return next_stack();
    case LF: return next_flow_control();
    case TAB:
      switch(next()) {
        case SPACE: return next_operator();
        case LF: return next_io();
        case TAB: return next_heap();
      }
  }
  return token(token::END, token::INVALID);
}

mpz_class lexer::next_integer(void) {
  char c = next();
  if(c == LF) return 0;
  bool negative = c == TAB;

  mpz_class num = 0;
  while((c = next()) != LF) {
    if(c == EOF) throw unexpected_eof("integer");
    num <<= 1;
    if(c == TAB) num |= 1;
  }
  return negative ? -1 * num : num;
}

token lexer::next_stack(void) {
  token::command type = token::STACK_MANIPULATION;
  switch(next()) {
    case SPACE: return token(type, token::PUSH, next_integer());
    case TAB:
      switch(next()) {
        case SPACE: return token(type, token::COPY_NTH, next_integer());
        case TAB: throw undefined_command("[Tab][Tab]", "stack manipulation");
        case LF: return token(type, token::SLIDE, next_integer());
      }
      break;
    case LF:
      switch(next()) {
        case SPACE: return token(type, token::DUPLICATE);
        case TAB: return token(type, token::SWAP);
        case LF: return token(type, token::DISCARD);
      }
      break;
  }
  throw unexpected_eof("stack manipulation command");
}

token lexer::next_operator(void) {
  token::command type = token::ARITHMETIC;
  switch(next()) {
    case SPACE:
      switch(next()) {
        case SPACE: return token(type, token::ADD);
        case TAB: return token(type, token::SUB);
        case LF: return token(type, token::MUL);
      }
      break;
    case TAB:
      switch(next()) {
        case SPACE: return token(type, token::DIV);
        case TAB: return token(type, token::MOD);
        case LF: throw undefined_command("[Tab][LF]", "arithmetic");
      }
      break;
    case LF: throw undefined_command("[LF]", "arithmetic");
  }
  throw unexpected_eof("arithmetic command");
}

token lexer::next_heap(void) {
  token::command type = token::HEAP_ACCESS;
  switch(next()) {
    case SPACE: return token(type, token::STORE);
    case TAB: return token(type, token::RETRIEVE);
    case LF: throw undefined_command("[LF]", "heap access");
  }
  throw unexpected_eof("heap access command");
}

token lexer::next_flow_control(void) {
  token::command type = token::FLOW_CONTROL;
  switch(next()) {
    case SPACE:
      switch(next()) {
        case SPACE: return token(type, token::MARK, next_integer());
        case TAB: return token(type, token::CALL, next_integer());
        case LF: return token(type, token::JUMP, next_integer());
      }
    case TAB:
      switch(next()) {
        case SPACE: return token(type, token::JUMPIFZ, next_integer());
        case TAB: return token(type, token::JUMPIFN, next_integer());
        case LF: return token(type, token::RETURN);
      }
    case LF:
      switch(next()) {
        case SPACE: throw undefined_command("[LF][Space]", "flow control");
        case TAB: throw undefined_command("[LF][Tab]", "flow control");
        case LF: return token(type, token::EXIT);
      }
  }
  throw unexpected_eof("flow control command");
}

token lexer::next_io(void) {
  token::command type = token::IO;
  switch(next()) {
    case SPACE:
      switch(next()) {
        case SPACE: return token(type, token::OUTC);
        case TAB: return token(type, token::OUTN);
        case LF: throw undefined_command("[Space][LF]", "I/O");
      }
    case TAB:
      switch(next()) {
        case SPACE: return token(type, token::READC);
        case TAB: return token(type, token::READN);
        case LF: throw undefined_command("[Tab][LF]", "I/O");
      }
    case LF: throw undefined_command("[LF]", "I/O");
  }
  throw unexpected_eof("I/O command");
}

char lexer::next(void) {
  char c;
  do {
    if(ptr_ == buf_size_) {
      ptr_ = 0;
      stream_.read(buf_, buf_size_);
      if(stream_.eof()) {
        buf_size_ = static_cast<int>(stream_.gcount());
      }
      if(buf_size_ == 0) {
        return EOF;
      }
    }
    c = buf_[ptr_++];
  } while(c != SPACE && c != TAB && c != LF);
  return c;
}
