#ifndef PROGRAM_H
#define PROGRAM_H

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include "expected.hpp"
#include <iostream>
#include <optional>
#include <sstream>
#include <vector>

class Program {
  public:
    // Constructor
    Program();

    // Getters
    bool GetShowHelp();
    std::string GetInputFilename();

    //Setters
    void SetShowHelp(bool);
    void SetInputFilename(std::string_view);

    // Function that shows the usage of the program
    void PrintUsage();

    // Function that parses the command line arguments
    std::optional<Program> ParseArgs(int, char*[]);

    // Function that reads the input file
    std::error_code ReadFile(int, std::vector<uint8_t>&);

    // Function that creates and ip address for the socket
    std::optional<sockaddr_in> MakeIpAddress(const std::optional<std::string>, uint16_t);

    // Function that converts a sockaddr_in address to a string
    std::string IpAddressToString(const sockaddr_in&);

    // Function that creates the socket
    //std::expected<int, std::error_code> MakeSocket(std::optional<sockaddr_in>);

    // Function that sends the file through the socket
    //std::error_code SendTo(int, const std::vector<uint8_t>&, const sockaddr_in&);

  private:
    // Define necessary previous data
    bool show_help_{false};
    std::string input_filename_;
};

#endif