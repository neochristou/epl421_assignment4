git:
	git add .
	git commit -m "$m"
	git push -u origin master

server:
	gcc -Wall -o myserver server.c