#ifndef LEXER_H
#define LEXER_H

#include <gmpxx.h>
#include <fstream>

#include "ws.h"

#define SPACE ' '
#define TAB '\t'
#define LF '\n'
#define BUFFER_LENGTH 1024

class file_not_found : public ws_error {
 public:
  file_not_found(const char* fname) {
    err_ = std::string("Error: could not open ") + fname;
  }

  std::string& get_error(void) {
    return err_;
  }

 private:
  std::string err_;
};

class undefined_command : public ws_error {
 public:
  undefined_command(const char* seq, const char* type) {
    err_ += std::string("Error: undefined ") + type + " command: " + seq;
  }

  std::string& get_error(void) {
    return err_;
  }

 private:
  std::string err_;
};

class unexpected_eof : public ws_error {
 public:
  unexpected_eof(const char* desc) {
    err_ += std::string("Error: unexpected end of file while reading ") + desc;
  }

  std::string& get_error(void) {
    return err_;
  }

 private:
  std::string err_;
};

class token {
 public:
  enum command {
    STACK_MANIPULATION, ARITHMETIC, HEAP_ACCESS, FLOW_CONTROL, IO, END
  };

  enum operation {
    /* Stack manipulation*/
    PUSH, DUPLICATE, COPY_NTH, SWAP, DISCARD, SLIDE,

    /* Arithmetic */
    ADD, SUB, MUL, DIV, MOD,

    /* Heap access */
    STORE, RETRIEVE,

    /* Flow control */
    MARK, CALL, JUMP, JUMPIFZ, JUMPIFN, RETURN, EXIT,

    /* I/O */
    OUTC, OUTN, READC, READN,

    /* Invalid command */
    INVALID
  };

  token(void) { }
  token(command c, operation o, mpz_class val = 0) : type_(c), op_(o), arg_(val) { }

  command type() {
    return type_;
  }

  operation op() {
    return op_;
  }

  mpz_class arg() {
    return arg_;
  }

 private:
  command type_;
  operation op_;
  mpz_class arg_;
};

class lexer {
 public:
  lexer(const char*, int = BUFFER_LENGTH);
  ~lexer(void);

  token next_token(void);

 private:
  token next_stack(void);
  token next_operator(void);
  token next_heap(void);
  token next_flow_control(void);
  token next_io(void);
  mpz_class next_integer(void);
  char next(void);
  
  std::ifstream stream;
  char* buf;
  int buf_size;
  int ptr;
};
#endif
