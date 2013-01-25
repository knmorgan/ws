#include "ws.h"

#include "lexer.h"
#include "parser.h"

#include <cstdlib>
#include <cstring>
#include <sstream>

static unsigned long stack_limit = DEFAULT_STACK_LIMIT;
static char* fname = 0;

unsigned long get_stack_limit(void) {
  return stack_limit;
}

static void print_usage(void) {
  std::cout << "usage: ws [<args>] <file>" << std::endl;
}

static void print_version(void) {
  std::cout << WS_COPYRIGHT << std::endl;
}

static void handle_args(int argc, char** argv) {
  while(argc > 0) {
    char* arg = *argv;
    if(!strcmp(arg, "-v") || !strcmp(arg, "--version")) {
      print_version();
      break;
    } else if(!strcmp(arg, "-h") || !strcmp(arg, "--help")) {
      break;
    } else if(!strcmp(arg, "-s") || !strcmp(arg, "--stack-size")) {
      if(argc < 2) throw generic_error("no value given for --stack-size");
      stack_limit = atol(argv[1]);
      if(!(std::stringstream(argv[1]) >> stack_limit)) {
        throw generic_error("invalid value for --stack-size");
      }
      argc--;
      argv++;
    } else if(*arg == '-' && strlen(arg) > 1) {
      // TODO: Should this be an error?
      std::cout << "Unrecognized argument: '" << arg << "'" << std::endl;
    } else {
      // Assume that the first argument that does not begin with a hyphen
      // is intended to be the file.
      fname = fname ? fname : arg;
    }
    argc--;
    argv++;
  }
}

std::ostream& operator<<(std::ostream& os, ws_error& err) {
  return os << err.get_error();
}

int main(int argc, char** argv) {
  if(argc < 2) {
    print_usage();
  } else {
    try {
      argc--;
      argv++;
      handle_args(argc, argv);

      if(!fname) {
        print_usage();
        return EXIT_SUCCESS;
      }

      parser p(fname);
      p.execute();
    } catch(ws_error& e) {
      std::cerr << e << std::endl;
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}
