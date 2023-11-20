#include "netcp.h"

int protected_main(int argc, char* argv[]) {
  socket_t sock;
}

int main(int argc, char* argv[]) {
  // Gets the options that correspond to the user input
  auto options = parse_args(argc, argv);
  if (!options) { return EXIT_FAILURE; }

  // Uses options.value() for accessing the options
  if (options.value().show_help) { print_usage(); }

  // ...

  return EXIT_SUCCESS;
}