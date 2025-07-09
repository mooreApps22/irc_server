# Server tasks:
- [x] Setup -setup()-:
  - [x] Create a new socket .
  - [x] Bind the socket to the port provided.
  - [x] Start listening for new connections.
  - [x] Create a event poll (epoll).
  - [x] Register the listening socket to the event poll -invoke registerFd()-.
- [ ] Run -run()-:
  - [x] Invoke setup()
  - [x] Loop for new events:
	- [x] If listening socket:
	  - [x] Invoke acceptConnection().
	- [x] If client socket:
	  - [x] Invoke -getMessage()-.

- [x] Register fd to event poll -registerFd()-

- [x] Accept new connection -acceptConnection()-:
  - [x] Register client to the event poll -registerFd()-.
  - [x] Add client as new user.

- [x] Read message -getMessage()-:
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

