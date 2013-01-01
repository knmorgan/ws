#ifndef WS_HEAP_H
#define WS_HEAP_H

#include <gmpxx.h>
#include <map>

#include "ws.h"

struct heap_error : public ws_error {
 public:
  heap_error(const char* desc) {
    err_ = std::string("Error: ") + desc;
  }

  std::string& get_error(void) {
    return err_;
  }

 private:
  std::string err_;
};

class ws_heap {
 public:
  ws_heap(void);
  ~ws_heap(void);

  void store(mpz_class, mpz_class);
  mpz_class get(mpz_class);

 private:
  ws_heap(const ws_heap&);
  ws_heap& operator=(const ws_heap&);

  std::map<mpz_class,mpz_class> heap_;
};

#endif
