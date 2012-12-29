#ifndef WS_H
#define WS_H

#include <iostream>
#include <string>

class ws_error {
 public:
  virtual std::string& get_error(void) = 0;
};

std::ostream& operator<<(std::ostream& os, ws_error& err);

#endif
