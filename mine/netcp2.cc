#include "netcp.h"

std::error_code Socket::read_file(int fd, const std::vector<uint8_t>& buffer);

std::optional<sockaddr_in> Socket::make_ip_address(const std::optional<std::string> ip_address, uint16_t port=0);

using make_socket_result = std::expected<int, std::error_code>;
make_socket_result Socket::make_socket(std::optional<sockaddr_in> address = std::nullopt);

std::error_code Socket::send_to(int fd, const std::vector<uint8_t>& message, const sockaddr_in& address);