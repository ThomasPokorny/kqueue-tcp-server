event-loop:
	clang tcpserver_kqueue.c -o tcpserver_kqueue

event-loop-callback:
		clang tcpserver_kqueue_callback.c -o tcpserver_kqueue_callback
