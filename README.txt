EPL 421 Assignment 4 - 2019

Makefile:
	make (Compile and create the executable)
	make git m="Commit message" (All in one add/commit/push)

Github: https://github.com/neochristou/epl421_assignment4.git

Execute: (Start server)
	./myserver

Testing: (Send requests)
	nc -C localhost 2005 < Test_Requests/Get_All_request.txt
	nc -C localhost 2005 < Test_Requests/Wrong_Method_request.txt

***PUT requests needs to specify the Content-Length of the body sent to the server***