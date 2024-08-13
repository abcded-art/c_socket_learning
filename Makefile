all:
	ssh ubuntu@3.39.100.211 "gcc --output chat_server ./chat_socket_server.c --pthread"