#ifndef WS_H
#define WS_H

#include <iostream>
#include <string>

#define WS_VERSION_MAJOR "0"
#define WS_VERSION_MINOR "1"
#define WS_VERSION "ws " WS_VERSION_MAJOR "." WS_VERSION_MINOR
#define WS_COPYRIGHT WS_VERSION " Copyright 2012-2013, Kyle Morgan"

#define DEFAULT_STACK_LIMIT 1e6

class ws_error {
 public:
  virtual std::string& get_error(void) = 0;
};

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

unsigned long get_stack_limit(void);

std::ostream& operator<<(std::ostream& os, ws_error& err);

#endif
