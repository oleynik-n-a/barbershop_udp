# barbershop_udp
Test program to work with local network based on udp connection:
  1. **server.c** illustrates barber sleeping without clients and working with them.
  2. **client.c** illustrates cliet visiting barbershop.
  3. **log.c** illustrates logger program that outputs to the console all actions in local network.

Rules:
  1. Working server must be one.
  2. May be more then one client.
  3. Clients work independently of each other.
  4. Logger server must be one.
  5. Running order: server -> logger -> clients.
