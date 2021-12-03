#include "netlib/platform.hpp"
#include <set>
#include <stdexcept>
#include "netlib/io_multiplexer.hpp"


namespace netlib {


    //check the operation enum
    static void check_operation(io_multiplexer::operation_type operation) {
        switch (operation) {
        case io_multiplexer::operation_type::read:
        case io_multiplexer::operation_type::write:
            return;
        }
        throw std::invalid_argument("Invalid operation value specified in operations list.");
    }


    //constructor.
    io_multiplexer::io_multiplexer(size_t max_resource_count)
        : m_max_resource_count(max_resource_count)
        , m_resources_changed{false}
        , m_resource_count{0}
        , m_stop{false}
    {
    }


    //destructor.
    io_multiplexer::~io_multiplexer() {
        stop();
    }


    /**
     * Adds a resource.
     */
    bool io_multiplexer::add(io_resource& resource, const std::initializer_list<std::pair<callback_type, operation_type>>& operations) {
        //check if the initializer list contains operations of the same type
        std::set<operation_type> operation_set;
        for (const auto& entry : operations) {
            check_operation(entry.second);
            const auto [it, ok] = operation_set.insert(entry.second);
            if (!ok) {
                throw std::invalid_argument("The operations list contains the same operation more than once.");
            }
        }

        //if the operations set is empty, then there is an error
        if (operation_set.empty()) {
            throw std::invalid_argument("The operations list is empty.");
        }

        //build the resources info
        std::vector<resource_info> resources;
        for (const auto& entry : operations) {
            if (!entry.first) {
                throw std::invalid_argument("The operations list contains an empty callback.");
            }
            resources.push_back(resource_info{ &resource, entry.first, entry.second });
        }

        std::lock_guard lock(m_mutex);

        //check if resources exceed the number of pollfd entries
        if (m_resource_count + resources.size() > std::numeric_limits<nfds_t>::max()) {
            throw std::invalid_argument("Resource count overflow.");
        }

        //check if resources can be added
        if (m_resource_count + resources.size() >= m_max_resource_count) {
            return false;
        }

        //try to insert the resource
        const auto [it, ok] = m_resources.emplace(&resource, resources);

        //if the resource is already added, throw error
        if (!ok) {
            throw std::invalid_argument("The given resource has already been added to this multiplexer.");
        }

        //add resource count
        m_resource_count += resources.size();

        //put change event in pipe, if not put yet
        set_resources_changed();

        //success
        return true;
    }


    //Adds an array of resources.
    bool io_multiplexer::add(const std::vector<io_resource*>& resources, const callback_type& cb, operation_type operation) {
        size_t add_index = 0;
        bool added_all_resources = true;

        //add the resources
        for (; add_index < resources.size(); ++add_index) {
            if (!add(*resources[add_index], cb, operation)) {
                added_all_resources = false;
                break;
            }
        }

        //if added all resources, return true
        if (added_all_resources) {
            return true;
        }

        //remove the added resources
        for (size_t i = 0; i < add_index; ++i) {
            remove(*resources[i]);
        }

        //could not add all resources
        return false;
    }



    //removes a resource.
    void io_multiplexer::remove(io_resource& resource) {
        std::lock_guard lock(m_mutex);

        //find resources
        auto it = m_resources.find(&resource);

        //if not found, throw error
        if (it == m_resources.end()) {
            throw std::invalid_argument("The given resource has not been added to this multiplexer.");
        }

        //count resources
        m_resource_count -= it->second.size();

        //erase the resource
        m_resources.erase(it);

        //put change event in pipe, if not put yet
        set_resources_changed();
    }


    //poll
    io_multiplexer::poll_result_type io_multiplexer::poll(int milliseconds) {
        //check state
        {
            std::lock_guard lock(m_mutex);

            //if stopped
            if (m_stop) {
                return stopped;
            }

            //if empty
            if (m_resources.empty()) {
                return empty;
            }
 
            //if resources are changed, rebuild the arrays needed for polling and processing
            if (m_resources_changed) {
                m_resources_changed = false;
                build_pollfd();
            }
        }

        //wait for data; add one to the resource count to account for the internal pipe
        int result = ::poll(reinterpret_cast<pollfd*>(m_pollfd.data()), static_cast<nfds_t>(m_resource_table.size()), timeout);

        //check if stopped after waking up
        {
            std::lock_guard lock(m_mutex);
            if (m_stop) {
                return stopped;
            }
        }

        //if result was greater than 0, then process resources
        if (result > 0) {
            process_resources(result);
            return ok;
        }

        //if result is 0, then there was a timeout
        if (result == 0) {
            return timeout;
        }

        //there was an error
        throw std::runtime_error(get_last_error_message());
    }


    //Sets the multiplexer in stopped state, if it is not stopped yet.
    void io_multiplexer::stop() {
        {
            std::lock_guard lock(m_mutex);

            //already stopped; do nothing else
            if (m_stop) {
                return;
            }

            //set the stop flag
            m_stop = true;
        }

        //notify the polling thread
        char buffer[1];
        m_resources_changed_pipe.write(buffer, 0);
    }


    //sets the resources changed, if not yet changed
    void io_multiplexer::set_resources_changed() {
        if (!m_resources_changed) {
            m_resources_changed = true;
            char buffer;
            m_resources_changed_pipe.write(&buffer, 0);
        }
    }


    //builds the pollfd data
    void io_multiplexer::build_pollfd() {
        //clear arrays
        m_resource_table.clear();
        m_pollfd.clear();

        //set the appropriate size for arrays; add 1 for the internal pipe
        m_resource_table.resize(m_resource_count + 1);
        m_pollfd.resize(sizeof(pollfd) * (m_resource_count + 1));

        //get pointers to arrays
        resource_info* res = m_resource_table.data();
        pollfd* pfd = reinterpret_cast<pollfd*>(m_pollfd.data());

        //put the internal pipe as the first object; there is no resource for it
        ++res;
        pfd->events = POLLRDNORM;
        pfd->fd = m_resources_changed_pipe.get_read_descriptor();
        ++pfd;

        //put resources
        for (const auto& resource_entry : m_resources) {
            for (const resource_info& resource : resource_entry.second) {
                //copy the resource
                *res = resource;

                //set up the pollfd structure
                put_resource(pfd, resource);

                //next resource/pollfd structure
                ++res;
                ++pfd;
            }
        }
    }


    //put resource
    void io_multiplexer::put_resource(struct pollfd* pfd, const resource_info& resource) {
        switch (resource.operation) {
        case read:
            pfd->events = POLLRDNORM;
            pfd->fd = resource.resource->read_handle();
            break;

        case write:
            pfd->events = POLLWRNORM;
            pfd->fd = resource.resource->write_handle();
            break;
        }
    }


    //process resources
    void io_multiplexer::process_resources(size_t resource_count) {
        //if the internal pipe has data, reduce the resource count by one
        pollfd* pfd_first = reinterpret_cast<pollfd*>(m_pollfd.data());
        if (pfd_first->revents) {
            --resource_count;
        }

        //skip entry at 0 because it is the internal pipe
        for (size_t resource_index = 1; resource_index < m_resource_table.size() && resource_count > 0; ++resource_index) {

            //get pointer to the pfd structure
            pollfd* pfd = &reinterpret_cast<pollfd*>(m_pollfd.data())[resource_index];

            //if there was a read or write event on this structure, invoke the callback
            if (pfd->revents) {
                //one less resource to process
                --resource_count;

                //get the resource info
                resource_info& resource = m_resource_table[resource_index];

                //invoke the callback
                resource.callback(*this, *resource.resource);
            }

        }
    }


} //namespace netlib
