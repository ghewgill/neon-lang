#include <stdlib.h>
#include <string.h>
#include <string>

#include <zmq.h>

#include "neonext.h"

const Ne_MethodTable *Ne;

static const char *Exception_ZeromqException = "ZeromqException";

extern "C" {

#define CHECK(x) if (x != 0) Ne_RAISE_EXCEPTION(Exception_ZeromqException, __FUNCTION__, errno)

static void free_data(void *data, void * /*hint*/)
{
    free(data);
}

Ne_EXPORT int Ne_INIT(const Ne_MethodTable *methodtable)
{
    Ne = methodtable;
    return Ne_SUCCESS;
}

Ne_CONST_INT(Ne_ZMQ_REQ   , ZMQ_REQ)
Ne_CONST_INT(Ne_ZMQ_REP   , ZMQ_REP)
Ne_CONST_INT(Ne_ZMQ_DEALER, ZMQ_DEALER)
Ne_CONST_INT(Ne_ZMQ_ROUTER, ZMQ_ROUTER)
Ne_CONST_INT(Ne_ZMQ_PUB   , ZMQ_PUB)
Ne_CONST_INT(Ne_ZMQ_SUB   , ZMQ_SUB)
Ne_CONST_INT(Ne_ZMQ_PUSH  , ZMQ_PUSH)
Ne_CONST_INT(Ne_ZMQ_PULL  , ZMQ_PULL)
Ne_CONST_INT(Ne_ZMQ_PAIR  , ZMQ_PAIR)

Ne_CONST_INT(Ne_ZMQ_POLLIN , ZMQ_POLLIN)
Ne_CONST_INT(Ne_ZMQ_POLLOUT, ZMQ_POLLOUT)
Ne_CONST_INT(Ne_ZMQ_POLLERR, ZMQ_POLLERR)

Ne_FUNC(Ne_zmq_bind)
{
    void *socket = Ne_PARAM_POINTER(void, 0);
    std::string endpoint = Ne_PARAM_STRING(1);

    CHECK(::zmq_bind(socket, endpoint.c_str()));
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_zmq_close)
{
    void *socket = Ne_PARAM_POINTER(void, 0);

    CHECK(::zmq_close(socket));
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_zmq_connect)
{
    void *socket = Ne_PARAM_POINTER(void, 0);
    std::string endpoint = Ne_PARAM_STRING(1);

    CHECK(::zmq_connect(socket, endpoint.c_str()));
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_zmq_ctx_get)
{
    void *context = Ne_PARAM_POINTER(void, 0);
    int option_name = Ne_PARAM_INT(1);

    int r = ::zmq_ctx_get(context, option_name);
    if (r < 0) {
        CHECK(r);
    }
    Ne_RETURN_INT(r);
}

Ne_FUNC(Ne_zmq_ctx_new)
{
    Ne_RETURN_POINTER(::zmq_ctx_new());
}

Ne_FUNC(Ne_zmq_ctx_set)
{
    void *context = Ne_PARAM_POINTER(void, 0);
    int option_name = Ne_PARAM_INT(1);
    int option_value = Ne_PARAM_INT(2);

    CHECK(::zmq_ctx_set(context, option_name, option_value));
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_zmq_ctx_shutdown)
{
    void *context = Ne_PARAM_POINTER(void, 0);

    CHECK(::zmq_ctx_shutdown(context));
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_zmq_ctx_term)
{
    void *context = Ne_PARAM_POINTER(void, 0);

    CHECK(::zmq_ctx_term(context));
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_zmq_curve_keypair)
{
    Ne_Cell *z85_public_key = Ne_OUT_PARAM(0);
    Ne_Cell *z85_secret_key = Ne_OUT_PARAM(1);

    char public_key[41];
    char secret_key[41];
    CHECK(::zmq_curve_keypair(public_key, secret_key));
    Ne->cell_set_string(z85_public_key, public_key);
    Ne->cell_set_string(z85_secret_key, secret_key);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_zmq_disconnect)
{
    void *socket = Ne_PARAM_POINTER(void, 0);
    std::string endpoint = Ne_PARAM_STRING(1);

    CHECK(::zmq_disconnect(socket, endpoint.c_str()));
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_zmq_msg_close)
{
    zmq_msg_t *msg = Ne_PARAM_POINTER(zmq_msg_t, 0);

    CHECK(::zmq_msg_close(msg));
    delete msg;
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_zmq_msg_copy)
{
    zmq_msg_t *dest = Ne_PARAM_POINTER(zmq_msg_t, 0);
    zmq_msg_t *src = Ne_PARAM_POINTER(zmq_msg_t, 1);

    CHECK(::zmq_msg_copy(dest, src));
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_zmq_msg_data)
{
    zmq_msg_t *msg = Ne_PARAM_POINTER(zmq_msg_t, 0);

    size_t size = ::zmq_msg_size(msg);
    void *data = ::zmq_msg_data(msg);
    Ne_RETURN_STRING(std::string(static_cast<char *>(data), size).c_str());
}

Ne_FUNC(Ne_zmq_msg_get)
{
    zmq_msg_t *msg = Ne_PARAM_POINTER(zmq_msg_t, 0);
    int property = Ne_PARAM_INT(1);

    int r = ::zmq_msg_get(msg, property);
    if (r < 0) {
        CHECK(r);
    }
    Ne_RETURN_INT(r);
}

Ne_FUNC(Ne_zmq_msg_init_data)
{
    std::string data = Ne_PARAM_STRING(0);

    zmq_msg_t *msg = new zmq_msg_t;
    void *d = malloc(data.size());
    memcpy(d, data.data(), data.size());
    CHECK(::zmq_msg_init_data(msg, d, data.size(), free_data, NULL));
    Ne_RETURN_POINTER(msg);
}

Ne_FUNC(Ne_zmq_msg_init_size)
{
    int size = Ne_PARAM_INT(0);

    zmq_msg_t *msg = new zmq_msg_t;
    CHECK(::zmq_msg_init_size(msg, size));
    Ne_RETURN_POINTER(msg);
}

Ne_FUNC(Ne_zmq_msg_init)
{
    zmq_msg_t *msg = new zmq_msg_t;
    CHECK(::zmq_msg_init(msg));
    Ne_RETURN_POINTER(msg);
}

Ne_FUNC(Ne_zmq_msg_more)
{
    zmq_msg_t *msg = Ne_PARAM_POINTER(zmq_msg_t, 0);

    Ne_RETURN_INT(::zmq_msg_init(msg) != 0);
}

Ne_FUNC(Ne_zmq_msg_move)
{
    zmq_msg_t *dest = Ne_PARAM_POINTER(zmq_msg_t, 0);
    zmq_msg_t *src = Ne_PARAM_POINTER(zmq_msg_t, 1);

    CHECK(::zmq_msg_move(dest, src));
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_zmq_msg_recv)
{
    zmq_msg_t *msg = Ne_PARAM_POINTER(zmq_msg_t, 0);
    void *socket = Ne_PARAM_POINTER(void, 1);
    int flags = Ne_PARAM_INT(2);

    int r = ::zmq_msg_recv(msg, socket, flags);
    if (r < 0) {
        CHECK(r);
    }
    Ne_RETURN_INT(r);
}

Ne_FUNC(Ne_zmq_msg_send)
{
    zmq_msg_t *msg = Ne_PARAM_POINTER(zmq_msg_t, 0);
    void *socket = Ne_PARAM_POINTER(void, 1);
    int flags = Ne_PARAM_INT(2);

    CHECK(::zmq_msg_send(msg, socket, flags));
    delete msg;
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_zmq_msg_set)
{
    zmq_msg_t *msg = Ne_PARAM_POINTER(zmq_msg_t, 0);
    int property = Ne_PARAM_INT(1);
    int value = Ne_PARAM_INT(2);

    CHECK(::zmq_msg_set(msg, property, value));
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_zmq_msg_size)
{
    zmq_msg_t *msg = Ne_PARAM_POINTER(zmq_msg_t, 0);

    Ne_RETURN_UINT(static_cast<unsigned int>(::zmq_msg_size(msg)));
}

Ne_FUNC(Ne_zmq_poll)
{
    const Ne_Cell *items = Ne_IN_PARAM(0);
    int timeout_ms = Ne_PARAM_INT(1);
    Ne_Cell *out_items = Ne_OUT_PARAM(0);

    size_t item_count = Ne->cell_get_array_size(items);
    zmq_pollitem_t *litems = static_cast<zmq_pollitem_t *>(alloca(sizeof(zmq_pollitem_t) * item_count));
    for (size_t i = 0; i < item_count; i++) {
        const Ne_Cell *item = Ne->cell_get_array_cell(items, static_cast<int>(i));
        // These offsets match the PollItem structure in zeromq.neon.
        litems[i].socket = Ne->cell_get_pointer(Ne->cell_get_array_cell(item, 0));
        litems[i].fd = Ne->cell_get_number_int(Ne->cell_get_array_cell(item, 1));
        litems[i].events = static_cast<short>(Ne->cell_get_number_int(Ne->cell_get_array_cell(item, 2)));
        litems[i].revents = 0;
    }
    int r = ::zmq_poll(litems, static_cast<int>(item_count), timeout_ms);
    if (r < 0) {
        CHECK(r);
    }
    Ne->cell_copy(out_items, items);
    for (size_t i = 0; i < item_count; i++) {
        Ne->cell_set_number_int(Ne->cell_set_array_cell(Ne->cell_set_array_cell(out_items, static_cast<int>(i)), 3), litems[i].revents);
    }
    Ne_RETURN_INT(r);
}

Ne_FUNC(Ne_zmq_proxy_steerable)
{
    void *frontend = Ne_PARAM_POINTER(void, 0);
    void *backend = Ne_PARAM_POINTER(void, 1);
    void *capture = Ne_PARAM_POINTER(void, 2);
    void *control = Ne_PARAM_POINTER(void, 3);

    CHECK(::zmq_proxy_steerable(frontend, backend, capture, control));
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_zmq_proxy)
{
    void *frontend = Ne_PARAM_POINTER(void, 0);
    void *backend = Ne_PARAM_POINTER(void, 1);
    void *capture = Ne_PARAM_POINTER(void, 2);

    CHECK(::zmq_proxy(frontend, backend, capture));
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_zmq_recv)
{
    void *socket = Ne_PARAM_POINTER(void, 0);
    int len = Ne_PARAM_UINT(1);
    int flags = Ne_PARAM_INT(2);

    char *buf = static_cast<char *>(alloca(len));
    int r = ::zmq_recv(socket, buf, len, flags);
    if (r < 0) {
        CHECK(r);
    }
    Ne_RETURN_STRING(std::string(buf, len).c_str());
}

Ne_FUNC(Ne_zmq_send)
{
    void *socket = Ne_PARAM_POINTER(void, 0);
    std::string buf = Ne_PARAM_STRING(1);
    int flags = Ne_PARAM_INT(2);

    CHECK(::zmq_send(socket, buf.data(), buf.size(), flags));
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_zmq_socket)
{
    void *context = Ne_PARAM_POINTER(void, 0);
    int type = Ne_PARAM_INT(1);

    void *r = ::zmq_socket(context, type);
    if (r == NULL) {
        Ne_RAISE_EXCEPTION(Exception_ZeromqException, __FUNCTION__, errno);
    }
    Ne_RETURN_POINTER(r);
}

Ne_FUNC(Ne_zmq_strerror)
{
    int errnum = Ne_PARAM_INT(0);

    Ne_RETURN_STRING(::zmq_strerror(errnum));
}

Ne_FUNC(Ne_zmq_unbind)
{
    void *socket = Ne_PARAM_POINTER(void, 0);
    std::string endpoint = Ne_PARAM_STRING(1);

    CHECK(::zmq_unbind(socket, endpoint.c_str()));
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_zmq_version)
{
    Ne_Cell *major = Ne_OUT_PARAM(0);
    Ne_Cell *minor = Ne_OUT_PARAM(1);
    Ne_Cell *patch = Ne_OUT_PARAM(2);

    int maj, min, pat;
    ::zmq_version(&maj, &min, &pat);
    Ne->cell_set_number_int(major, maj);
    Ne->cell_set_number_int(minor, min);
    Ne->cell_set_number_int(patch, pat);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_zmq_z85_decode)
{
    std::string string = Ne_PARAM_STRING(0);

    uint8_t *buf = static_cast<uint8_t *>(alloca((string.length() + 3) * 4 / 5));
    uint8_t *r = ::zmq_z85_decode(buf, string.c_str());
    if (r == NULL) {
        Ne_RAISE_EXCEPTION(Exception_ZeromqException, __FUNCTION__, errno);
    }
    Ne_RETURN_STRING(reinterpret_cast<char *>(buf));
}

Ne_FUNC(Ne_zmq_z85_encode)
{
    std::string data = Ne_PARAM_STRING(0);

    char *buf = static_cast<char *>(alloca(data.length() * 5 / 4 + 1));
    char *r = ::zmq_z85_encode(buf, reinterpret_cast<const uint8_t *>(data.data()), data.length());
    if (r == NULL) {
        Ne_RAISE_EXCEPTION(Exception_ZeromqException, __FUNCTION__, errno);
    }
    Ne_RETURN_STRING(buf);
}

} // extern "C"
