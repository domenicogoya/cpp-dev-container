#include "program.h"

int main(int argc, char* argv[]) {
  // Creates object
  Program program;
  // Parses options and check if no options are given
  auto options = program.ParseArgs(argc, argv);
  if (!options) {
    return EXIT_FAILURE;
  } else if (options.value().GetShowHelp()) {
    options.value().PrintUsage();
    return EXIT_SUCCESS;
  }

  // Opens the file and gets its file descriptor, checks if an error occurred
  int file_fd = open(options.value().GetInputFilename().c_str(), O_RDONLY);
  if (file_fd < 0) {
    std::cerr << "Error. File could not be opened: " << std::strerror(errno) << std::endl;
    return EXIT_FAILURE;
  }
  // Checks that the file is not bigger than 1KB
  struct stat st;
  int result = fstat(file_fd, &st);
  if (result < 0) {
    std::cerr << "Error. File data could not be retrieved: " << std::strerror(errno) << std::endl;
    close(file_fd);
    return EXIT_FAILURE;
  }
  if (st.st_size > 1024) {
    std::cerr << "Error. File size cannot be bigger than 1KB.\n";
    close(file_fd);
    return EXIT_FAILURE;
  }

  // Reads the file
  std::vector<uint8_t> buffer(1024);
  std::error_code error = options.value().ReadFile(file_fd, buffer);
  if (error) {
    std::cerr << "Error. File could not be read: " << std::strerror(errno) << std::endl;
    close(file_fd);
    return EXIT_FAILURE;
  }

  // Creates the socket
  

  // Closes the file descriptor and ends the program
  close(file_fd);
  return EXIT_SUCCESS;
}