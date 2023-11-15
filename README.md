# SEND_RECEIVE_UDP
It is a C++ application that can be used to do automatic tests. First run your code X to be tested and read values 1 => ON, 0 => OFF for some buttons depending on your machine (by default there are 4 buttons implemented), then modify their values locally both automatically or manually through send.exe! In automatic mode it is also possible to control the timing between each send. You can find an example in file command.txt. For example "1 0 0 0;1000" mean that you want BUTTON 1 => ON, BUTTON 2 => OFF, BUTTON 3 => OFF, BUTTON 4 => OFF and you will wait 1000 ms before doing the next send.
Obviously, the ports for sending packages must to be configured based on your needs; by default are made two send for ports 3001 and 4001. The receive.exe is useful to test first the UDP communication; it must be implemented directly inside the code you need to test
