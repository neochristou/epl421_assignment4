git:
	git add .
	git commit -m "$m"
	git push -u origin master

server:
	gcc -Wall -o myserver server.c myjson.c -ljansson

client:
	gcc -Wall -o myclient client.c