#include "netcp.h"


// Function that shows help message
void print_usage() {
  std::cout << "Usage: netcp [-h] FILE\n";
}

// Function that opens the input file
std::expected<int, std::error_code> open_file(const std::string& path, int flags, mode_t mode) {
  int fd{open(path.c_str(), flags, mode)};
  if (fd == -1) {
    std::error_code error(errno, std::system_category());
    return std::unexpected(error);
  }

  return fd;
}

// Function that reads the input file
std::error_code read_file(int fd, std::vector<uint8_t>& buffer) {
  // Creation of the buffer
  //ssize_t read(int fd, void* buf, size_t count);
  ssize_t bytes_read = read(fd, buffer.data(), buffer.size());
  if (bytes_read < 0) {
      return std::error_code(errno, std::system_category());
  }
  buffer.resize(bytes_read);
  return std::error_code(0, std::system_category());
}


// Function that parses the command line arguments
std::optional<program_options> parse_args(int argc, char* argv[]) {
  // Creates std::string_view in order to avoid copying strings
  std::vector<std::string_view> args (argv + 1, argv + argc);
  // Creates a struct variable
  program_options options;
  
  // Loop that compares the input with the available options
  for (auto it = args.begin(), end=args.end(); it != end; ++it) {
    if (*it == "-h" || *it == "--help") {
      options.show_help = true;
    }

    if (*it == "-o" || *it == "--output") {
      if (++it != end) {
        options.output_filename = *it;
      } else {
        std::cerr << "Error...\n";
        return std::nullopt;
      }
    }

    // Additional options
  }

  return options;
}