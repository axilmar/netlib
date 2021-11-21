#ifndef NETLIB_UDP_MESSAGING_INTERFACE_HPP
#define NETLIB_UDP_MESSAGING_INTERFACE_HPP


#include "socket_messaging_interface.hpp"


namespace netlib {


    /**
     * UDP messaging interface.
     * It adds a crc32 on the end of each message.
     */
    class udp_messaging_interface : public socket_messaging_interface {
    public:
        /**
         * the default constructor.
         */
        udp_messaging_interface();

        /**
         * Constructor from address family.
         * @param af address family.
         */
        udp_messaging_interface(int af = constants::ADDRESS_FAMILY_IP4);

        /**
         * Opens the socket.
         * @param af address family.
         */
        void open_socket(int af = constants::ADDRESS_FAMILY_IP4);

        /**
         * Sends a message.
         * @param msg message to send.
         * @return true if the message was sent, false if it could not be sent.
         */
        bool send_message(message&& msg) override;

        /**
         * Receives a message.
         * @param mesres memory resource to use for allocating memory for the message.
         * @param max_message_size maximum number of bytes that can be possibly received.
         * @return a pointer to the received message or null if reception was impossible.
         */
        message_pointer<> receive_message(std::pmr::memory_resource& memres, size_t max_message_size = NETLIB_MAX_PACKET_SIZE) override;

        using socket_messaging_interface::receive_message;

        /**
         * Sends a message to a specific address.
         * @param msg message to send.
         * @param addr address to send the message to.
         * @return true if the message was sent, false if it could not be sent.
         */
        bool send_message(message&& msg, const socket_address& addr);

        /**
         * Receives a message.
         * @param addr address of sender.
         * @param mesres memory resource to use for allocating memory for the message.
         * @param max_message_size maximum number of bytes that can be possibly received.
         * @return a pointer to the received message or null if reception was impossible.
         */
        message_pointer<> receive_message(socket_address& addr, std::pmr::memory_resource& memres, size_t max_message_size = NETLIB_MAX_PACKET_SIZE);

        /**
         * Receives a message.
         * @param addr address of sender.
         * @param max_message_size maximum number of bytes that can be possibly received.
         * @return a pointer to the received message or null if reception was impossible.
         */
        message_pointer<> receive_message(socket_address& addr, size_t max_message_size = NETLIB_MAX_PACKET_SIZE);

    protected:
        /**
         * Internal function that can be customized on the socket function used to send data.
         * @param buffer buffer with data to send.
         * @param socket_send function to use for sending the data.
         */
        template <class SocketSendFunction>
        static bool send_packet(byte_buffer& buffer, SocketSendFunction&& socket_send) {
            serialize_crc32(buffer);
            const size_t sent_size = socket_send(buffer);
            return sent_size == buffer.size();
        }

        /**
         * Internal function that can be customized on the socket function used to receive data.
         * @param buffer buffer with data to receive.
         * @param socket_receive function to use for receiving the data.
         */
        template <class SocketReceiveFunction>
        static bool receive_packet(byte_buffer& buffer, SocketReceiveFunction&& socket_receive) {
            const size_t size = socket_receive(buffer);
            return size ? deserialize_crc32(buffer) : false;
        }

        /**
         * Sends the data.
         * @param buffer buffer of data to send.
         * @return true if sent, false otherwise.
         */
        bool send_data(const byte_buffer& buffer);

        /**
         * Receives the data.
         * @param buffer buffer to store the data.
         * @return true if the data were received, false otherwise.
         */
        bool receive_data(byte_buffer& buffer);

        /**
         * Sends the data.
         * @param buffer buffer of data to send.
         * @param addr address of receiver.
         * @return true if sent, false otherwise.
         */
        bool send_data(const byte_buffer& buffer, const socket_address& addr);

        /**
         * Receives the data.
         * @param buffer buffer to store the data.
         * @param addr address of sender.
         * @return true if the data were received, false otherwise.
         */
        bool receive_data(byte_buffer& buffer, socket_address& addr);

    private:
        //serialize crc32
        static void serialize_crc32(byte_buffer& buffer);

        //deserialize crc32 and compare it to the one stored in the data
        static bool deserialize_crc32(byte_buffer& buffer);
    };


} //namespace netlib


#endif //NETLIB_UDP_MESSAGING_INTERFACE_HPP
