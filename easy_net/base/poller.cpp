#include "poller.h"

#include "epoll_poller.h"
#include "poll_poller.h"

poller::poller(event_loop *loop) : m_owner_loop(loop) {
    // nothing todo
}

poller *create_poller(poller_type_e type, event_loop *loop) {
    poller *p = nullptr;
    switch (type) {
        case poller_type_e::TYPE_EPOLL: {
            p = new epoll_poller(loop);
            break;
        }
        case poller_type_e::TYPE_POLL: {
            p = new poll_poller(loop);
            break;
        }
    }

    return p;
}