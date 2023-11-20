#include <iostream>

struct options {
  bool show_help;
  std::string input_filename;
};

std::optional<program_options> parse_args(int argc, char* argv[]) {
  return program_options;
}

std::error_code read_file(int fd, std::vector<uint8_t>& buffer) {
  ssize_t bytes_read = read(fd, buffer.data(), buffer.size());
  if (bytes_read < 0) {
    return std::error_code(errno, std::system_category());
  }
  buffer.resize(bytes_read);
  return std::error_code(0, std::system_category());
}

int main(int argc, char* argv[]) {
  auto options = parse_args(argc, argv);
  if (!options) {
    print_usage();
    return EXIT_FAILURE;
  }

  if (options.value().show_help) {
    print_usage();
    return EXIT_SUCCESS;
  }

  // Abrir fichero
  int fd = open(options.value().input_filename.c_str(), O_RDONLY);
  if (fd < 0) {
    std::cerr << "Error. File could not be opened: " << std::strerror(errno);
    return EXIT_FAILURE;
  } 

  // Comprobar que el archivo no ocupa más de 1KB
  stat st;
  int result = fstat(fd, &st);
  if (result < 0) {
    // Error management
    return EXIT_FAILURE;
  }

  /// Leer el archivo
  std::vector<uint8_t> buffer[1024];
  std::error_code error = read_file(fd, buffer);
  if (error) {
    std::cerr << "Error. File could not be read: " << std::format("Error ({}): {}\n", error.value(), error.message());
    close(fd);
    return EXIT_FAILURE;
  }

  // int fd = socket(AF_INET, SOCK_DGRAM, 0);
  // if (fd < 0) {
  // // Error al crear el socket
  // }

  sock_fd = make_socket();


  std::error_code error = send_to(sock_fd, buffer, dest_address);
  if (error) {
    // Error management
    return EXIT_FAILURE;
  }

  close(fd);

  return EXIT_SUCCESS;
}