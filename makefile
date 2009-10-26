all: quote_client quote_server

clean:
	-rm quote_client quote_server

client:
	gcc -g -Wall -o quote_client quote_client.c -lcrypto -lnsl

server:
	gcc -g -Wall -o quote_server quote_server.c -lpthread -lcrypto -lnsl
