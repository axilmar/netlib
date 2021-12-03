#ifndef NETLIB_LOCKABLE_RESOURCE_HPP
#define NETLIB_LOCKABLE_RESOURCE_HPP


#include <mutex>
#include "io_resource.hpp"
#include "pipe.hpp"


namespace netlib {


    /**
     * A lockable resource.
     * @param T type of lockable object.
     */
    template <class T> class lockable_resource : public io_resource {
    public:
        /**
         * The default constructor.
         */
        lockable_resource() : m_pipe(NETLIB_MINIMUM_PIPE_SIZE) {}

        /**
         * Returns the internal pipe's write handle.
         */
        handle_type write_handle() const override { return m_pipe.write_handle(); }

        /**
         * Returns the internal pipe's read handle.
         */
        handle_type read_handle() const override { return m_pipe.read_handle(); }

        /**
         * Returns true if the pipe is stream oriented.
         */
        virtual bool is_stream_oriented() const { return pipes_are_stream_oriented; }

        /**
         * Reads the data from a resource.
         * It waits for the pipe to send some data, meaning the lockable object is unlocked,
         * and then tries to lock the lockable object.
         * @param buffer destination buffer.
         * @param size number of bytes to read; 1 if locked, 0 if not locked.
         * @return number of bytes read or closed.
         */
        io_result_type read(void* buffer, size_t size) override {
            //wait for unlock signal
            char buf[1];
            io_result_type result = m_pipe.read(buf, 0);

            //if the pipe is still open, try to lock the lockable object
            if (result.second) {
                return { try_lock() ? 1 : 0, true };
            }

            //not open
            return { 0, false };
        }

        /**
         * Interface for writing data to a resource.
         * Throws std::logic_error; a lockable resource can only be read.
         * @param buffer source buffer.
         * @param size number of bytes to write.
         * @return number of bytes written or closed.
         */
        io_result_type write(const void* buffer, size_t size) override {
            throw std::logic_error("This lockable resource is a read-only resource.");
        }

        /**
         * Tries to lock the resource.
         */
        bool try_lock() {
            return m_lockable_object.try_lock();
        }

        /**
         * Locks the resource.
         * @exception std::runtime_error thrown if the lockable object cannot be locked.
         */
        void lock() {
            //if the resource is locked successfully, do nothing else
            if (try_lock()) {
                return;
            }

            //the mutex is locked; wait for unlocked signal from the pipe
            for (;;) {
                //get data from pipe
                char buf[1];
                io_result_type result = m_pipe.read(buf, 0);

                //if the pipe is not closed
                if (result.second) {

                    //if it is locked successfully, return
                    if (try_lock()) {
                        return;
                    }

                    //some other thread locked the lockable object; wait for unlocked signal again
                    continue;
                }

                throw std::runtime_error("Cannot lock lockable resource because it is closed.");
            }
        }

        /**
         * Unlocks the resource.
         */
        void unlock() {
            //unlocks the object
            m_lockable_object.unlock();

            //send unlock signal for those that wait for the lockable resource
            char buf[1];
            m_pipe.write(buf, 0);
        }

    private:
        pipe m_pipe;
        T m_lockable_object;
    };


    /**
     * A mutex which is also an i/o resource and can be multiplexed with sockets and pipes.
     */
    using mutex = lockable_resource<std::mutex>;


} //namespace netlib


#endif //NETLIB_LOCKABLE_RESOURCE_HPP
