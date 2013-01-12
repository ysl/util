#include <stdio.h>
#include <event.h>
#include <evhttp.h>
 
#define SVR_IP                         "127.0.0.1"
#define SVR_PORT                       8088


/*
 * dump_req()
 */
void dump_req (struct evhttp_request *req)
{
  char                      *peer_ip = NULL;
  uint16_t                  peer_port = 0;
  struct evhttp_connection  *conn;
  const char                *uri;

  /* 
   * Get client's IP and port
   */
  if ((conn = evhttp_request_get_connection(req)) != NULL) {
    evhttp_connection_get_peer(conn, &peer_ip, &peer_port);
  }

  /* 
   * Retrieve URI info
   */
  uri = evhttp_request_get_uri(req);

  printf("Receive request from [%s:%d]  URI: %s\n",
         (peer_ip ? peer_ip : "null"), peer_port, uri);
}

/*
 * specific_cb()
 */
void specific_cb (struct evhttp_request *req, void *arg)
{
    struct evbuffer *evbuf;
 
	/*
     * Dump client information.
	 */
    dump_req(req);
    
	/*
	 * Create buffer for send back reply.
	 */
    if ((evbuf = evbuffer_new()) == NULL) {
        printf("evbuffer_new() failed");
        evhttp_send_reply(req, HTTP_INTERNAL, "Internal error", NULL);
        return;
    }
 
    /* 
     * Body
     */
    evbuffer_add_printf(evbuf, "It's work!");
     
    /* 
     * Response
     */
    evhttp_send_reply(req, HTTP_OK, "OK", evbuf);
     
    /* 
     * Free resource
     */
    evbuffer_free(evbuf);
}

/*
 * generic_cb()
 */
void generic_cb (struct evhttp_request *req, void *arg)
{ 
    /*
     * Dump request
     */
    dump_req(req);

    /* 
     * Response
     */
    evhttp_send_reply(req, HTTP_NOTFOUND, "Not found", NULL);
}
 
/*
 * main()
 */
int main (int argc, char **argv)
{
    struct event_base *evbase;
    struct evhttp     *evhttp;
 
    /* 
     * Init event base
     */
    if ((evbase = event_base_new()) == NULL) {
        printf("event_base_new() failed\n");
        return -1;
    }
 
    /* 
     * Init evhttp
     */
    if ((evhttp = evhttp_new(evbase)) == NULL) {
        printf("evhttp_new() failed\n");
        return -1;
    }
 
    /* 
     * Set server IP, port
     */
    if (evhttp_bind_socket(evhttp, SVR_IP, SVR_PORT) == -1) {
        printf("evhttp_bind_socket() failed\n");
        return -1;
    } else {
        printf("Listening on [%s:%d]\n", SVR_IP, SVR_PORT);
    }
     
    /*
     *  Set a callback for specific path
     */
    if (evhttp_set_cb(evhttp, "/test", specific_cb, NULL) < 0) {
        printf("evhttp_set_cb() failed\n");
        return -1;
    }
     
    /* 
     * Set a callback for default path
     */
    evhttp_set_gencb(evhttp, generic_cb, NULL);
 
    /* 
     * Enter event loop
     */
    event_base_dispatch(evbase);
 
    /* 
     * Free resource
     */
    evhttp_free(evhttp);
    event_base_free(evbase);
     
    return 0;
}

