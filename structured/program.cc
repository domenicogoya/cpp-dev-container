#include "program.h"

// Constructor
Program::Program() {}

// Getters
bool Program::GetShowHelp() {
  return show_help_;
}
std::string Program::GetInputFilename() {
  return input_filename_;
}

//Setters
void Program::SetShowHelp(bool show_help) {
  show_help_ = show_help;
}
void Program::SetInputFilename(std::string_view input_filename) {
  input_filename_ = input_filename;
}

// Function that shows the usage of the program
void Program::PrintUsage() {
  std::cout << "Usage: netcp [-h] FILE\n";
}

// Function that parses the command line arguments
std::optional<Program> Program::ParseArgs(int argc, char* argv[]) {
  // Create the returned object
  Program options;
  // Create a vector with the references to the arguments strings
  std::vector<std::string_view> args(argv + 1, argv + argc);

  // Parse the arguments
  if (args.empty()) {
    std::cerr << "Error. No input file provided: " << std::strerror(errno) << std::endl;
    options.PrintUsage();
    return std::nullopt;
  } else {
    for (auto it = args.begin(), end = args.end(); it != end; ++it) {
      if (*it == "-h" || *it == "--help") {
        options.SetShowHelp(true);
        std::cerr << "Warning. The option '-h | --help' cannot be combined with other arguments.\n";
      } else {
        options.SetInputFilename(*it);
        if (it != end) {
          std::cerr << "Warning. Only the first argument (INPUT FILENAME) will be taken.\n";
        }
      }
    }
    return options;
  }
}

// Function that reads the input file
std::error_code Program::ReadFile(int file_fd, std::vector<uint8_t>& buffer) {
  // Resizes the buffer if it's shorter than 1KB checking the read bytes
  ssize_t bytes_read = read(file_fd, buffer.data(), buffer.size());
  // Returns error if the read operation returned any error
  if (bytes_read < 0) {
    return std::error_code(errno, std::system_category());
  }
  buffer.resize(bytes_read);
  return std::error_code(0, std::system_category());
}

// Function that creates and ip address for the socket
std::optional<sockaddr_in> Program::MakeIpAddress(const std::optional<std::string> ip_address, uint16_t port=0) {
  struct sockaddr_in sock_address;
  // If the address is nullopt, the socket is in any ip address
  if (!ip_address) {
    sock_address.sin_addr.s_addr = INADDR_ANY;
  } else {
    // Checks if the format of the address is ip:port and creates 2 strings
    std::string ip_address_str = ip_address.value(), port_str = std::to_string(port);
    std::istringstream ip_address_ss(ip_address.value());
    if (std::getline(ip_address_ss, ip_address_str, ':')) {
      if (std::getline(ip_address_ss, port_str)) {
        // Checks if the user has entered ':' but not a port
        if (port_str.empty()) {
          std::cerr << "Error. Invalid format provided. Please use only one format at a time.\n";
          return std::nullopt;
        }
        // Checks if the user has entered an invalid port
        if (std::stoi(port_str) < 0 || std::stoi(port_str) > 65535) {
          std::cerr << "Error. Invalid port provided. Please use only ports from 1 to 65535.\n";
          return std::nullopt;
        }
        // Checks if both formats are used at the same time ip:port, port (we suppose that the port 0 is not available)
        if (port != 0) {
          std::cerr << "Error. Incompatible formats and arguments provided. Please use only one format at a time (ip:port or ip, port).\n";
          return std::nullopt;
        }
      } else {
        std::cerr << "Error. Cannot retrieve the IP Address from the input: " << std::strerror(errno) << std::endl;
        return std::nullopt;
      }
    } else {
      std::cerr << "Error. Cannot retrieve the Port from the input: " << std::strerror(errno) << std::endl;
      return std::nullopt;
    }
    // If the ip address does not contains ':' it will be assumed that the format is ip, port

    // Converts the ip address to a valid format and checks if any error occurred
    int ip_conversion = inet_aton(ip_address_str.c_str(), &sock_address.sin_addr);
    if (!ip_conversion) {
      std::cerr << "Error. The IP Address '" << ip_address.value() << "' is not valid: " << std::strerror(errno) << std::endl;
      return std::nullopt;
    }

    sock_address.sin_port = htons(port);
  }
  // ...

  return sock_address;
}

// Function that converts a sockaddr_in address to a string
std::string Program::IpAddressToString(const sockaddr_in& address) {
  std::string str_ip = inet_ntoa(address.sin_addr);
  std::string str_port = std::to_string(ntohs(address.sin_port));
  std::string str_address = str_ip + ":" + str_port;
  
  return str_address;
}

//// Function that creates the socket
//std::expected<int, std::error_code> Program::MakeSocket(std::optional<sockaddr_in> address = std::nullopt) {
//  // ...
//}

// // Function that sends the file through the socket
// std::error_code Program::SendTo(int sock_fd, const std::vector<uint8_t>& message, const sockaddr_in& address) {
//   // ...
// }