#ifndef PARSER_H
#define PARSER_H

#include <map>
#include <stack>
#include <vector>

#include "lexer.h"
#include "ws_heap.h"
#include "ws_stack.h"
#include "ws.h"

class parser {
 public:
  parser(const char*);
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

  bool exited_;
  const char* fname_;
  unsigned long stack_limit_;
  ws_heap heap_;
  ws_stack stack_;
  std::vector<token> parse_vec_;
  std::stack<std::vector<token>::iterator> call_stack_;
  std::map<mpz_class,std::vector<token>::iterator> labels_;
  std::vector<token>::iterator cur_;
};

#endif
