#include "ws.h"

#include "lexer.h"
#include "parser.h"

std::ostream& operator<<(std::ostream& os, ws_error& err) {
  return os << err.get_error();
}

void print_usage(void) {
  std::cout << "ws: no input files." << std::endl;
}

int main(int argc, char** argv) {
  if(argc < 2) {
    print_usage();
  } else {
    for(int i = 1; i < argc; ++i) {
      parser p(argv[i]);

      try {
        p.execute();
      } catch(ws_error& e) {
        std::cerr << e << std::endl;
		return EXIT_FAILURE;
      }
    }
  }

  return EXIT_SUCCESS;
}
