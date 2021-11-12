# Netlib

C++17 networking/serialization/deserialization library.

## Features

- sockets wrapper implementation that provides a portable, platform-neutral sockets API.
- automatic serialization/deserialization of C++ basic types, STL types/containers and trivially-copyable structs.
- optimized serialization/deserialization for STL containers backed up by contiguous arrays.
- automatic handling of endianess.
- support for tcp/udp, for ip4/ip6 networking, out of the box; open API for support of other network types.
- customizable memory allocation for received messages.
- optional automatic message registration/message id creation.
- separation of messages into namespaces, based on c++ namespaces, in case of automatic message registration.
- crc32 check for udp messages.

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
    std::string content;
    
    //constructor that creates a message id for this message automatically.
    my_message() : message(auto_message_id<my_message>::register_message()) {}
    
    //serialize message
    void serialize(byte_buffer& buffer) const final {
        message::serialize(buffer);
        netlib::serialize(buffer, content);
    }
    
    //deserialize message
    void deserialize(const byte_buffer& buffer, byte_buffer& pos) final {
        message::deserialize(buffer);
        netlib::deserialize(buffer, pos, content);
    }
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

### message_id

Numeric identifier for a message.

### message_size

Type of message size; by default, it is the type `uint16_t`, meaning the maximum message size = 2^16 = 65536 bytes. It can be changed by the preprocessor definition `NETLIB_MESSAGE_SIZE`.

### message

Base class for messages.

### message_registry

Allows registration of message creation functions, in order to automatically create message instances from the received message id.

### auto_message_id< T, Ref = T>

Allows automatic registration/creation of message id for message `T`, using `Ref` as the reference type.

### byte_buffer

Alias for `std::vector<std::byte>`; used through out the library for buffers.

### socket_error

Exception thrown when there is an error in a socket operation; the error message contains a platform specific error id and error message text.

### message_error

Exception thrown when a message constraint is not satisfied (for example, the received message id is different from the id of the message that was created for the received id).

## Details

### Message ids

In order for each message to be recognized when received, each message shall have is own unique id.

There are two ways to define message ids:

1. manual creation of message ids.
2. automatic creation of message ids.

#### Manual creation of message ids

Ids can be declared manually, using integers or enumerations. Example:

```c++
enum class MsgId : netlib::message_id {
    Msg1,
    Msg2
};

class Msg1 : public netlib::message {
public:
    Msg1() : netlib::message(MsgId::Msg1) {}
};

class Msg2 : public netlib::message {
public:
    Msg2() : netlib::message(MsgId::Msg2) {}
};
```

#### Automatic creation of message ids

Manual creation and maintenance of message ids is cumbersome, especially if the number of messages grows too big.

The library provides a way to automatically enumerate messages and create message ids. Example:

```c++
class Msg1 : public netlib::message {
public:
    Msg1() : netlib::message(netlib::auto_message_id<Msg1>::register_message()) {}
};

class Msg2 : public netlib::message {
public:
    Msg2() : netlib::message(netlib::auto_message_id<Msg2>::register_message()) {}
};
```

#### Anatomy of an automatically created message id

##### Message id default type

The library, by default, uses the type `uint16_t' for message ids; this means that, by default, 2^16 = 65536 different messages can co-exist. 

This can be changed at compile time by defining the type used for the message ids through the preprocessor definition `NETLIB_MESSAGE_ID_STORAGE_TYPE`.

##### Message id structure

An automatically created message id is broken down to the following parts:

1. bits 10-15: contains the namespace id.
2. bits 0-9: contains the message id.

The preprocessor definition `NETLIB_MESSAGE_ID_NAMESPACE_BITS` can be used to alter the size, in bits, of the namespace id field. By default, it has the value 6, which means that each message id can accommodate **2^6=64 namespaces and 2^10=1024 messages per namespace**.

##### Message namespaces

The library uses the c++ namespaces to differentiate messages: when a message is registered to the library, its typename is split into namespace name and class name. Messages with the same name but in different namespaces will get a different namespace id and possibly the same message id.

This allows an application to separate messages into logical domains using namespaces.

### Message registration

Messages need to be registered to the library in order to be automatically deserialized from network received data.

- Messages with manually specified ids must be registered with the function `message_registry::register_message<T>(id)`.
- Messages with automatically specified ids need not be registered manually; the function `auto_message_id<T>::register_message()` returns the message id and registers the message.
- The function `auto_message_id<T>::get_message_id()` registers type T and returns its id without registering the message callback.

