git:
	git add .
	git commit -m "$m"
	git push -u origin master
all:
	gcc -Wall -o myserver server.c myjson.c -ljansson -lpthread
	gcc -Wall -o myclient client.c

client:
	gcc -Wall -o myserver server.c myjson.c -ljansson 

server:
	gcc -Wall -o myclient client.c -lpthread