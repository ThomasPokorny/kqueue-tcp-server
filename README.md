# Example TCP server that uses `kqueue(2)` #
This is a bare minimum TCP server that can be used to learn how [`kqueue(2)`][kqueue_manpage_link] works. The code contains comments on what each section of code does.

The example in tcpserver_kqueue_callback.c contains a two functions which are executed once data can be read from a client connection. 

### Run ###
`make event-loop`

`./tcpserver_kqueue`

Now connect to the server at port 1815. For instance, using netcat:

`nc -v localhost 1815`

[kqueue_manpage_link]: https://www.freebsd.org/cgi/man.cgi?query=kqueue&apropos=0&sektion=2&manpath=FreeBSD+12.0-RELEASE+and+Ports&arch=default&format=html

### Run callback example ###

`make event-loop-callback`

`./tcpserver_kqueue_callback`

Now connect to the server at port 1816. For instance, using netcat as is presented in the section above
