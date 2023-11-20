#ifndef NETCP_H
#define NETCP_H
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <array>
#include <cstdint>
#include <cstdlib>
#include <expected>
#include <experimental/scope>
#include <iostream>
#include <optional>
#include <string>
#include <system_error>
#include <vector>

// Function that shows help message
void print_usage();

// Function that opens the input file
std::expected<int, std::error_code> open_file(const std::string& path,
                                              int flags, mode_t mode);

// Function that reads the input file
std::error_code read_file(int fd, const std::vector<uint8_t>& buffer);

std::optional<sockaddr_in> make_ip_address(
    const std::optional<std::string> ip_address, uint16_t port = 0);

std::expected<int, std::error_code> make_socket(
    std::optional<sockaddr_in> address = std::nullopt);

std::error_code send_to(int fd, const std::vector<uint8_t>& message,
                        const sockaddr_in& address);

// Struct that defines the program options
struct program_options {
  bool show_help{false};
  std::string output_filename;
  // ...
};

// Function that parses the command line arguments
std::optional<program_options> parse_args(int argc, char* argv[]);

#endif