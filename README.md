*This project has been created as part of the 42 curriculum by esouhail, maissat, braugust.*


### Description

**Internet Relay Chat or IRC** is a text-based communication protocol on the Internet.<br>
It offers real-time messaging that can be either public or private. Users can exchange<br>
direct messages and join group channels.<br>
IRC clients connect to IRC servers in order to join channels. IRC servers are connected<br>
together to form a network.

```text
IRC itself is a teleconferencing system, which (through the use of
the client-server model) is well-suited to running on many machines
in a distributed fashion.  A typical setup involves a single process
(the server) forming a central point for clients (or other servers)
to connect to, performing the required message delivery/multiplexing
and other functions.
```

### Instructions

1. Compile the program
```bash
make
```

2. Run the server (program)

```bash
./ircserver <port_number> <server_password>
```

3. Connect to the server

We have chosen irssi as our reference client. This simply means that we made sure<br>
that our program works properly with the IRC client *irssi*, which provides a TUI<br>
and also helps us manage file transfer.

```bash
irssi -c <ipv4_address> -p <port_number> -w <server_password> -n <user_nickname>
```

or inside irssi

```bash
/connect <ipv4_address> <port_number> <server_password>
```


### Resources
- [RFC 1459 - Internet Relay Chat Protocol](https://datatracker.ietf.org/doc/html/rfc1459)
- [Modern IRC Client Protocol](https://modern.ircdocs.horse/)
- [IRC Definition Files](https://defs.ircdocs.horse/)

AI was also used as a tutor helping understand the protocol better and explaining lower level concepts involved in the project like sockets, ports and functions like bind(), accept() etc.