#include "ws_heap.h"

ws_heap::ws_heap(void) { }

ws_heap::~ws_heap(void) { }

void ws_heap::store(mpz_class key, mpz_class val) {
  heap[key] = val;
}

mpz_class ws_heap::get(mpz_class key) {
  std::map<mpz_class,mpz_class>::iterator it = heap.find(key);
  if(it == heap.end()) {
    throw heap_error("tried to retrieve nonexistent value from heap");
  }

  return it->second;
}
