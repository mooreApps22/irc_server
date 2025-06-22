# Server tasks:
- [x] Setup -setup()-:
  - [x] Create a new socket .
  - [x] Bind the socket to the port provided.
  - [x] Start listening for new connections.
  - [x] Create a event poll (epoll).
  - [x] Register the listening socket to the event poll -invoke register_fd()-.
- [ ] Run -run()-:
  - [x] Invoke setup()
  - [x] Loop for new events:
    - [x] If listening socket:
      - [x] Invoke accept_connection().
    - [x] If client socket:
      - [x] Invoke -get_message()-.

- [x] Register fd to event poll -register_fd()-

- [x] Accept new connection -accept_connection()-:
  - [x] Register client to the event poll -register_fd()-.
  - [x] Add client as new user.

- [x] Read message -get_message()-:
  - [x] Deal with partial messages.
  - [x] Deal with complete messages.

- [x] Add new user

- [x] Delete user

# User tasks:

# Commands:
## PASS:
## NICK:
- [ ] Change nick when already registered
## USER:
- [ ] Any user mode?
- [ ] Parse username
- [ ] Parse host
- [ ] Parse 

