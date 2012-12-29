#ifndef PARSER_H
#define PARSER_H

#include <map>
#include <stack>
#include <vector>

#include "lexer.h"
#include "ws_heap.h"
#include "ws_stack.h"
#include "ws.h"

class generic_error : public ws_error {
 public:
  generic_error(const char* str) {
    err_ = std::string("Error: ") + str;
  }

  std::string& get_error(void) {
    return err_;
  }

 private:
  std::string err_;
};

class parser {
 public:
  parser(const char*, unsigned long = 1e6);
  ~parser(void);

  void parse(void);
  void build_label_index(void);
  void execute(void);

 private:
  void handle_stack(token);
  void handle_arithmetic(token);
  void handle_heap(token);
  void handle_flow_control(token);
  void handle_io(token);

  void jump(mpz_class);
  void jump(std::vector<token>::iterator);
  void exit(void);

  bool exited;
  const char* fname;
  unsigned long stack_limit;
  ws_heap heap;
  ws_stack stack;
  std::vector<token> parse_vec;
  std::stack<std::vector<token>::iterator> call_stack;
  std::map<mpz_class,std::vector<token>::iterator> labels;
  std::vector<token>::iterator command;
};

#endif
