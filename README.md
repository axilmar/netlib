# Netlib

C++17 networking/serialization/deserialization library.

## Features

- sockets wrapper implementation that provides a portable, platform-neutral sockets API.
- automatic serialization/deserialization of C++ basic types, STL types/containers and trivially-copyable structs.
- optimized serialization/deserialization for STL containers backed up by contiguous arrays.
- easiest way to declare messages, with automatic serialization/deserialization.
- automatic handling of endianess.
- provided support for tcp/udp, for ip4/ip6 networking; open API for support of other network types.
- allows customization of memory allocation for received messages.
- provides optional automatic message registration/message id creation based on the alphabetical order of message class names.

## Example

### Code

```c++
#include <iostream>
#include <thread>
#include "netlib.hpp"

using namespace netlib;

//custom message declaration
class my_message : public message {
public:
    //content of the message
    field<std::string> content;
    
    //constructor that sets the message id from message id auto-enumeration.
    my_message() : message(auto_message_id<my_message>::get_message_id()) {}
};

//server thread
static void server_thread_proc() {
    //socket to listen for connections; tcp on ip4 protocol
	socket listen_socket{ socket::TCP_IP4 };
    
    //set the address to  listen to
    listen_socket.bind({"localhost", 1000});
    
    //set the socket to listen mode
    listen_socket.listen();
    
    //wait for connections
    socket_messaging_interface test_socket = listen_socket.accept().first;
    
    //prepare message
    my_message msg1;
    msg1.content = "hello world!";
    
    //send message
    for(size_t i = 0; i < 10; ++i) {
        test_socket.send_message(msg1);
    }
}

//client thread
static void client_thread_proc() {
    //create the socket
    socket_messaging_interface test_socket{ socket::TCP_IP4 };
    
    //connect to the server
    test_socket.connect({"localhost", 1000});
 
    //accept messages and print them
    for(int i = 0; i < 10; ++i) {
        message_pointer msg = test_socket.receive_message();
        my_message* my_msg = static_cast<my_message*>(msg.get());
        std::cout << my_msg.content << std::endl;
    }
}

int main() {
    //start the threads
	std::thread server_thread{server_thread_proc}; 
    std::thread client_thread{client_thread_proc};
    
    //wait for threads to terminate
    client.thread.join();
    server_thread.join();
    
    return 0;
}
```

### Output

```
hello world!
hello world!
hello world!
hello world!
hello world!
hello world!
hello world!
hello world!
hello world!
hello world!
```

## Types

### socket

Provides the socket wrapper implementation.

### socket_address

Combines a network address with a port.

### network_address

Wrapper over network addresses.

### messaging_interface

Base class for classes that can send/receive messages.

### socket_messaging_interface

Messaging interface with a sockets implementation.

### message

Base class for messages.

### field<T>

Class that allows auto-registering message fields for type `T`.

### message_registry

Allows registration of message creation functions, in order to automatically create message instances from the received message id.

### auto_message_id<T>

Allows automatic registration/creation of message id for message `T`.

### byte_buffer

Alias for `std::vector<std::byte>`; used through out the library for buffers.

### socket_error

Exception thrown when there is an error in a socket operation; the error message contains a platform specific error id and error message text.

### message_error

Exception thrown when a message constraint is not satisfied (for example, the received message id is different from the id of the message that was created for the received id).

### stringstream

Derived from std::stringstream, it provides `operator std::string` which allows creating strings with multiple parameters; for example: `stringstream() << "my error: " << error_number `.
