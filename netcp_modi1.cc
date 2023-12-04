#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <expected>
#include <experimental/scope>
#include <iostream>
#include <optional>
#include <sstream>
#include <vector>

// Struct that defines the program options
struct program_options {
  bool show_help{false};
  std::string input_filename;
};

// Function that shows the usage of the program
void print_usage() { std::cout << "Usage: netcp [-h] FILE\n"; }

// Function that parses the command line arguments
std::optional<program_options> parse_args(int argc, char* argv[]) {
  // Create a vector with the references to the arguments strings
  std::vector<std::string_view> args(argv + 1, argv + argc);
  program_options options;

  // Parse the arguments
  if (args.empty()) {
    std::cerr << "Error. No input file provided.\n";
    print_usage();
    return std::nullopt;
  } else {
    for (auto it = args.begin(), end = args.end(); it != end; ++it) {
      if (*it == "-h" || *it == "--help") {
        options.show_help = true;
        std::cerr << "Warning. The option '-h | --help' cannot be combined "
                     "with other arguments.\n";
      } else {
        options.input_filename = *it;
        break;
      }
    }
    return options;
  }
}

// Function that reads the input file
std::error_code read_file(int file_fd, std::vector<uint8_t>& buffer) {
  // Resizes the buffer if it is shorter than 1KB checking the read bytes
  ssize_t bytes_read = read(file_fd, buffer.data(), buffer.size());
  // Returns error if the read operation returned any error
  if (bytes_read < 0) {
    return std::error_code(errno, std::system_category());
  }
  buffer.resize(bytes_read);
  return std::error_code(0, std::system_category());
}

// Function that checks if the port is within the common range
bool check_port(int port) {
  // Returns if the user has entered an invalid port
  return port < 0 || port > 65535;
}

// Function that creates and ip address for the socket
std::optional<sockaddr_in> make_ip_address(
    const std::optional<std::string> ip_address, uint16_t port = 0) {
  struct sockaddr_in sock_address;
  // If the address is nullopt, the socket is in any ip address
  if (!ip_address) {
    sock_address.sin_addr.s_addr = INADDR_ANY;
    sock_address.sin_port = htons(port);
  } else {
    // Checks if the format of the address is ip:port and creates 2 strings
    std::string ip_address_str = ip_address.value(),
                port_str = std::to_string(port);
    std::istringstream ip_address_ss(ip_address.value());
    if (std::getline(ip_address_ss, ip_address_str, ':')) {
      if (std::getline(ip_address_ss, port_str)) {
        // Checks if the user has entered ':' but not a port
        if (port_str.empty()) {
          return std::nullopt;
        }
        // Checks if the user has entered an invalid port
        if (check_port(std::stoi(port_str))) {
          return std::nullopt;
        }
        // Checks if both formats are used at the same time ip:port, port
        if (port != 0) {
          return std::nullopt;
        }
        sock_address.sin_port = htons(std::stoi(port_str));
      } else {
        return std::nullopt;
      }
    } else {
      // Checks if the ip address value is empty
      if (ip_address.value().empty()) {
        return std::nullopt;
      }
      // Checks if the user has entered an invalid port
      if (check_port(std::stoi(port_str))) {
        return std::nullopt;
      }
      // Converts the ip address to a valid format and checks if any error
      // occurred
      int ip_conversion =
          inet_aton(ip_address_str.c_str(), &sock_address.sin_addr);
      sock_address.sin_port = htons(port);
      if (!ip_conversion) {
        // Returns the error if anything else goes wrong
        std::cerr << "Error. Cannot retrieve the IP Address from the input: "
                  << std::strerror(errno) << std::endl;
        return std::nullopt;
      }
    }
    // If the ip address does not contains ':' it will be assumed that the
    // format is ip, port
  }

  return sock_address;
}

// Function that converts a sockaddr_in address to a string
std::string ip_address_to_string(const sockaddr_in& address) {
  std::string str_ip = inet_ntoa(address.sin_addr);
  std::string str_port = std::to_string(ntohs(address.sin_port));
  std::string str_address = str_ip + ":" + str_port;

  return str_address;
}

// Function that creates the socket
std::expected<int, std::error_code> make_socket(
    std::optional<sockaddr_in> address = std::nullopt) {
  // Creates the socket
  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd < 0) {
    return std::unexpected{std::error_code(errno, std::system_category())};
  }

  // Assign the socket an address
  int assign_address = bind(fd, reinterpret_cast<const sockaddr*>(&(*address)),
                            sizeof(*address));
  if (assign_address < 0) {
    return std::unexpected{std::error_code(errno, std::system_category())};
  }

  return fd;
}

// Function that sends the file through the socket
std::error_code send_to(int sock_fd, const std::vector<uint8_t>& message,
                        const sockaddr_in& address) {
  // Stores the number of sent bytes or if there was an error
  ssize_t bytes_read =
      sendto(sock_fd, message.data(), message.size(), 0,
             reinterpret_cast<const sockaddr*>(&address), sizeof(address));
  // Returns error if the read operation returned any error
  if (bytes_read < 0) {
    return std::error_code(errno, std::system_category());
  }

  return std::error_code(0, std::system_category());
}

// Main function
int main(int argc, char* argv[]) {
  // Parses options and check if no options are given
  auto options = parse_args(argc, argv);
  if (!options) {
    return EXIT_FAILURE;
  } else if (options.value().show_help) {
    print_usage();
    return EXIT_SUCCESS;
  }

  // Opens the file and gets its file descriptor, checks if an error occurred
  int file_fd = open(options.value().input_filename.c_str(), O_RDONLY);
  if (file_fd < 0) {
    std::cerr << "Error. File could not be opened: " << std::strerror(errno)
              << std::endl;
    return EXIT_FAILURE;
  }
  // Creates a variable that destroys the file_fd once they are done
  auto file_guard =
      std::experimental::scope_exit([file_fd] { close(file_fd); });

  // Checks that the file is not bigger than 1KB
  struct stat st;
  int file_size = fstat(file_fd, &st);
  if (file_size < 0) {
    std::cerr << "Error. File data could not be retrieved: "
              << std::strerror(errno) << std::endl;
    return EXIT_FAILURE;
  }

  // Creates the socket
  auto address = make_ip_address("127.0.0.1", 8080);
  auto sock_made = make_socket(address.value());
  if (!sock_made) {
    std::cerr << "Error. The socket could not be created: " << strerror(errno)
              << std::endl;
    return EXIT_FAILURE;
  }
  int sock_fd = *sock_made;

  // Creates a variable that destroys the sock_fd once they are done
  auto sock_guard =
      std::experimental::scope_exit([sock_fd] { close(sock_fd); });

  // Reads the file and sends the data
  while (true) {
    std::vector<uint8_t> buffer(1024);
    std::error_code error = read_file(file_fd, buffer);
    if (error) {
      std::cerr << "Error. File could not be read: " << std::strerror(errno)
                << std::endl;
      return EXIT_FAILURE;
    }

    if (buffer.empty()) {
      break;
    }

    auto send_file = send_to(sock_fd, buffer, *address);
    if (send_file) {
      std::cerr << "Error. File could not be send: " << std::strerror(errno)
                << std::endl;
      return EXIT_FAILURE;
    }
  }

  // Ends the program
  return EXIT_SUCCESS;
}