#ifndef NETLIB_IO_MULTIPLEXER_HPP
#define NETLIB_IO_MULTIPLEXER_HPP


#include <vector>
#include <functional>
#include <mutex>
#include <map>
#include "pipe.hpp"


/**
 * Preprocessor definition for the multiplexer max resource count.
 */
#ifndef NETLIB_DEFAULT_MULTIPLEXER_RESOURCE_COUNT
#define NETLIB_DEFAULT_MULTIPLEXER_RESOURCE_COUNT 1024
#endif 


namespace netlib {


    /**
     * Input/output multiplexer.
     * Thread-safe class.
     */
    class io_multiplexer {
    public:
        /**
         * Callback function when a resource is available for an operation.
         * @param m current multiplexer.
         * @param r resource.
         */
        using callback_type = std::function<void(io_multiplexer& m, io_resource& r)>;

        /**
         * operation type.
         */
        enum operation_type {
            /**
             * read operation.
             */
            read,

            /**
             * write operation.
             */
            write
        };

        /**
         * poll result type.
         */
        enum poll_result_type {
            /**
             * poll is ok.
             */
            ok,

            /**
             * there was a timeout.
             */
            timeout,

            /**
             * poll is stopped. 
             */
            stopped
        };

        /**
         * The constructor.
         * @param max_resource_count maximum resource count.
         */
        io_multiplexer(size_t max_resource_count = NETLIB_DEFAULT_MULTIPLEXER_RESOURCE_COUNT);

        /**
         * The copy constructor.
         * The object is not copyable due to its internal complexity.
         */
        io_multiplexer(const io_multiplexer&) = delete;

        /**
         * The move constructor.
         * The object is not movable due to its internal complexity.
         */
        io_multiplexer(io_multiplexer&&) = delete;

        /**
         * The destructor.
         * If a thread is waiting for a resource to be available,
         * then this thread is notifed to stop waiting.
         */
        ~io_multiplexer();

        /**
         * The copy assignment operator.
         * The object is not copyable due to its internal complexity.
         */
        io_multiplexer& operator = (const io_multiplexer&) = delete;

        /**
         * The move assignment operator.
         * The object is not movable due to its internal complexity.
         */
        io_multiplexer& operator = (io_multiplexer&&) = delete;

        /**
         * adds a resource.
         * @param resource the resource to add.
         * @param cb callback to invoke when the resource is ready; must be non-null.
         * @param operations read and/or write operations list.
         * @return false if no more resources can be added to this multiplexer.
         * @exception std::invalid_argument thrown if the resource is already added or if the callback is empty or the operations list contains the same operations.
         */
        bool add(io_resource& resource, const std::initializer_list<std::pair<callback_type, operation_type>>& operations);

        /**
         * Removes a resource.
         * @param resource resource to remove.
         * @exception std::invalid_argument thrown if the resource has not been added to this multiplexer.
         */
        void remove(io_resource& resource);

        /**
         * Polls all the internal resources.
         * Invokes the appropriate callback for the resources that are ready.
         * Only one thread can safely poll for resources.
         * @param milliseconds milliseconds to wait; if less then zero, then it blocks.
         * @return true if polling can continue, false if the multiplexer object is destroyed.
         */
        poll_result_type poll(int milliseconds = -1);

    private:
        //resource info
        struct resource_info {
            io_resource* resource;
            callback_type callback;
            operation_type operation;
        };

        //max resource count
        const size_t m_max_resource_count;

        //internal pipe used for communicating resource changes
        pipe m_resources_changed_pipe;

        //used for object synchronization
        std::mutex m_mutex;

        //map of resources
        std::map<io_resource*, std::vector<resource_info>> m_resources;

        //if resources are changed
        bool m_resources_changed;

        //resources count
        size_t m_resource_count;

        //mutex used for polling
        std::mutex m_poll_mutex;

        //resource table; 1:1 correspondence with the pollfd array.
        std::vector<resource_info> m_resource_table;

        //pollfd array
        std::vector<char> m_pollfd;

        //if stopped
        bool m_stop;

        //sets the resources changed, if not yet changed
        void set_resources_changed();

        //builds the pollfd data
        void build_pollfd();

        //put resource
        static void put_resource(struct pollfd* pfd, const resource_info& resource);

        //process resources
        void process_resources(size_t resource_count);
    };


} //namespace netlib


#endif //NETLIB_IO_MULTIPLEXER_HPP
