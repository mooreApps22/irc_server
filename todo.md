# Server tasks:
- [x] Setup -setup()-:
  - [x] Create a new socket .
  - [x] Bind the socket to the port provided.
  - [x] Start listening for new connections.
  - [x] Create a event poll (epoll).
  - [x] Register the listening socket to the event poll -invoke register_fd()-.
- [x] Run -run()-:
  - [x] Invoke setup()
  - [x] Loop for new events:
    - [x] If listening socket:
      - [x] Invoke accept_connection().
    - [x] If client socket:
      - [x] Invoke -get_message()-.
- [x] Register fd to event poll -register_fd()-
- [x] Accept new connection -accept_connection()-:
  - [x] Register client to the event poll -register.
  - [ ] Add client as unsubscribed user.
- [ ] Read message -get_message()-.
- [ ] 

# User tasks:

