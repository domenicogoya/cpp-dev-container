struct program_options {
  bool show_help;
  std::string filename;
  bool listen_mode;
}

std::atomic_bool quit = false;

std::optional<std::string> getenv(const std::string name) {
  getenv();  // el de c++
}

int netcp_listen() {
  // Abrir archivo destino filename (ESCRITURA)
  // source_address = make_ip_address(std::nullopt, port)
  // make_socket(source_address)

  while (!buffer.empty() && !quit) {
    receive_from(buffer);  // -1 errno = EINTR // no es error realmente, si el
                           // error es este es que han pedido salir
    // if (buffer.empty()) {
    //   break;
    // }
    write_file(buffer);
  }
}

// Hecho en la primera parte de la práctica
int netcp_sender() {
  // abrir archivo de origen filename
  // make_socket()
  // remote_address = make_ip_address(ip_address, port)

  while (!buffer.empty() && !quit) {
    // read_file(buffer)
    // send_to(buffer, remote_address)
  }

  // NOTA: Cerrar todo lo que hayas abierto salgas por donde salgas

  return EXIT_SUCCESS;
}

void signal_handler(int signum) {
  // No se puede poner un std::cout << aquí
  write(STDOUT_FILENO, message, ...);
  quit = true;
}

int main() {
  // https://github.com/ull-esit-sistemas-operativos/ssoo-ejemplos/blob/master/src/cap10/signals.c
  sigaction signal_action{};
  signal_action.sa_handler = &signal_handler;
  sigaction(SIGHUP, &signal_action, NULL);
  sigaction(SIGTERM, &signal_action, NULL);
  sigaction(SIGINT, &signal_action, NULL);
  /////// NO PONER signal_action.sa_flags = SA_RESTART ////////
  // Parsear la línea de comandos: "-h", "-l", "filename" (program_options)

  // Leer variables de entorno
  // port = existe "NETCP_PORT" ? getenv("NETCP_PORT") : 8080
  // ip_address = existe "NETCP_IP" ? getenv("NETCP_IP") : 127.0.0.1 //
  // Ignora el modo listen

  if (program_options.listen_mode) {
    return netcp_listen();
  } else {
    return netcp_sender();
  }
}