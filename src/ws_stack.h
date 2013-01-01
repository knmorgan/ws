#ifndef WS_STACK_H
#define WS_STACK_H

#include <gmpxx.h>
#include <deque>
#include "ws.h"

struct stack_error : public ws_error {
 public:
  stack_error(const char* desc) {
    err_ = std::string("Error: ") + desc;
  }

  std::string& get_error(void) {
    return err_;
  }

 private:
  std::string err_;
};

class ws_stack {
 public:
  ws_stack(void);
  ~ws_stack(void);

  void push(mpz_class);
  void pop(void);
  void swap(void);
  void copy(int);
  void slide(int);
  mpz_class top(void);

 private:
  ws_stack(const ws_stack&);
  ws_stack& operator=(const ws_stack&);

  std::deque<mpz_class> stack_;
};

#endif
