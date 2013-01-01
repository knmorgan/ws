#include "ws_stack.h"

ws_stack::ws_stack(void) { }

ws_stack::~ws_stack(void) { }

void ws_stack::push(mpz_class val) {
  stack_.push_back(val);
}

void ws_stack::pop(void) {
  if(stack_.empty()) {
    throw stack_error("tried to pop empty stack.");
  }

  stack_.pop_back();
}

mpz_class ws_stack::top(void) {
  if(stack_.empty()) {
    throw stack_error("tried to access top of empty stack.");
  }

  return stack_.back();
}

void ws_stack::swap(void) {
  mpz_class temp1, temp2;
  temp1 = top();
  pop();
  temp2 = top();
  pop();
  push(temp1);
  push(temp2);
}

void ws_stack::copy(int n) {
  push(stack_[n]);
}

void ws_stack::slide(int n) {
  mpz_class t = top();
  for(int i = 0; i < n; ++i) {
    pop();
  }
  push(t);
}
