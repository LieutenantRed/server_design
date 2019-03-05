TARGET=server client

.PHONY: clean

all: $(TARGET)

client: 
	gcc -Wall -o client src/client.c -g

server: orcish.o tcp_routine.o multiplex.o udp_routine.o simple_socket.o socket_pool.o prod_cons.o
	gcc -Wall -o simple_socket orcish.o tcp_routine.o simple_socket.o -pthread -g
	gcc -Wall -o socket_pool orcish.o tcp_routine.o socket_pool.o -pthread -g
	gcc -Wall -o prod_cons orcish.o udp_routine.o prod_cons.o -pthread -g
	gcc -Wall -o multiplex orcish.o udp_routine.o tcp_routine.o multiplex.o -pthread -g
	rm *.o

orcish.o:
	gcc -Wall -c src/orcish.c -g

tcp_routine.o:
	gcc -Wall -c src/tcp_routine.c -g

simple_socket.o:
	gcc -Wall -c src/simple_socket.c -g

socket_pool.o:
	gcc -Wall -c src/socket_pool.c -g

udp_routine.o:
	gcc -Wall -c src/udp_routine.c -g

prod_cons.o:
	gcc -Wall -c src/prod_cons.c -g

multiplex.o:
	gcc -Wall -c src/multiplex.c -g



clean:
	rm simple_socket socket_pool prod_cons client multiplex