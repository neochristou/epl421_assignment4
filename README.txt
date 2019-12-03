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

Supported requests:

    GET /items: Returns 200 OK and a json list containing all available items on the server.
    Returns 404 Not Found if there are no item on the server.

    PUT /items: Adds the provided json list in the server. Returns 200 OK.
    Returns 404 Not Found and discard changes if there are any items in the list that does not exist on the server.

    GET /items/{itemname}: Returns 200 OK and the item in json form.
    Returns 404 Not Found if the item does not exist on the server.

    PUT /items/{itemname}: Adds the provided item on the server. Returns 200 OK.
    Returns 404 Not Found and discard changes if the item does not exist.

    DELETE /items: Deletes all the items from the server and returns 200 OK.

    DELETE /items/{itemname}: Deletes the item from the server and returns 200 OK.
    Returns 404 Not Found if the item does not exist.

    HEAD /items, HEAD /items/{itemname}: Works the same way as GET, but only returns head of reply without the body.
