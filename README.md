# This is a IRC server build with C++ 98. (Work in process.)

## Subject Notes: 

### External Functions:

- [x] socket,
- [x] close,
- [ ] setsockopt,
- [ ] getsockname,
- [ ] getprotobyname,
- [ ] gethostbyname,
- [x] getaddrinfo,
- [x] freeaddrinfo,
- [x] bind,
- [x] connect,
- [x] listen,
- [x] accept,
- [ ] htons,
- [ ] htonl,
- [ ] ntohs,
- [ ] ntohl,
- [ ] inet_addr,
- [ ] inet_ntoa,
- [x] send,
- [x] recv,
- [ ] signal,
- [x] sigaction,
- [ ] lseek,
- [ ] fstat,
- [ ] fcntl,
- [x] poll (or select or kqueue or **epoll**)

	DO NOT:
		develop an IRC client (just the server)
		implement server-to-server comm.

### Requirements:

- [ ] The server has to be able to handle client(s) w/o hanging
- [x] No Forking
- [x] I/O must be non-blocking: this means the program needs to be able to multitask.
- [x] Only 1 poll (or select or kqueue or epoll) can be used for handling all these operations (read, write, listen, etc.) 

		WARNING: Because you have to use non-blocking FDs, you can use read/recv or write/send w/o poll, and your server wouldn't be blocking
			BUT this uses more system resources
			IF you try read/recv or write/send w/o poll, you'll be FAILED.

- Several IRC clients exist. You have to choose one as a **reference** and the reference client will be used during the evaluation:
  - [ ] HexCHat*
  - [ ] WeeChat*
  - [ ] Polari(GNOME)
  - [ ] Konversation
  - [x] irssi*
  - [ ] netcat(nc)*
  - [ ] telnet

- [x] The reference client must be able to connect to your server w/o error
- [x] Use the TCP/IP protocol (v4 or v6)
- Your server has to be similar to using an official IRC server, but you only have to implement:
  - [ ] Must be able to authenticate
  - [ ] Set a nickname
  - [ ] Set a username
  - [ ] Join a channel
  - [ ] Send and receive private messages using your reference client
  - [ ] All the messages sent from one client to a channel have to be forwarded to every other client that joined the channel
  - [ ] You must have **operators** and regualr users
  - Command specific **channel operators**:
    - [ ] KICK	- Eject a client from the channel
    - [ ] INVITE	- Invite a client to a channel
    - [ ] TOPIC	- Change or view the channel topic
    - [ ] MODE	- Change the channels's Mode
    - [ ] * i: Set/remove Invite-only channel
    - [ ] * t: Set/remove the restrictins of the TOPIC command to channel operators
    - [ ] * k: Set/remove the channel key (password)
    - [ ] * o: Give/take channel operator privilege
    - [ ] * l: Set/remove the user limit to channel

### Test Example:

- Verfiy every possible error and issue:
  - [ ] receiving partial data
  - [ ] low bandwidth
  - [ ] buffer incomplete data before parsing
  - [ ] slow client(delayed send)
  - [ ] Malformed/invalid messages
  - [ ] missing parameters
  - [ ] wrong number of arguments
  - [ ] channel issues
  - [ ] handle appropriate numeric error code (RFC-compliant)
  - [ ] Multiple commands in one TCP packet
  - [ ] Multiple clients connecting/disconnecting rapidly
  - [ ] client changing nicknames
  - [ ] clients quitting
  - [ ] Unicode and special characters
  - [ ] Invalid command spam
  - [ ] Reuse of nicknames
  - [x] Signal Interruptions
  
		Testing sending ctrl+D to send the command in several parts:
			nc -C 127.0.0.1 6667
			com^Dman^Dd

  - [ ] Aggregate received packets in order to rebuild it

### BONUS:
		
- [ ] handle file transfer
- [ ] make a bot
