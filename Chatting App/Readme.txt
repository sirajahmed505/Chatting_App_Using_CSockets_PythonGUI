----TO RUN---------

---linux omly----

1. You can compile the client and server files separately using '-lpthread' flag
OR
You can run the 'comp.sh' file to compile both server and client.

2. To run the server you need a parameter. An integer sepecifying number of clients.
For instance, ./server.out 2     //for two clients.

3. U will need to change the client server to match your machine if you not usind VM.

4. You can run it on multiple devices on single LAN.
For VM, to run on multiple devices you will have to configure some WiFi settings in VM settings
since VM assigns you have fake IP of '10.0.2.15'
So try to run the server on on a configured VM or a PC with booted Ubantu/

Note: You do not need to run the 'gui.py' file, the client runs it by itself.
You will just need Python3 installed on your Ubantu.