/*
 * Base on bufferevent.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <event.h>


/*
 * write_cb()
 */
void write_cb (struct bufferevent *bev, void *arg)
{
  printf("Write data successfully.\n");
}

/*
 * read_cb()
 */
void read_cb (struct bufferevent *bev, void *arg)
{
  struct evbuffer *input; 
  int             len;

  /*
   * Get input buffer
   */
  input = bufferevent_get_input(bev);
  len = evbuffer_get_length(input);
  printf("Get data [%d] bytes\n", len);
  
  /*
   * Drain all input
   */
  evbuffer_drain(input, len);
}

/*
 * event_cb()
 */
void event_cb (struct bufferevent *bev, short events, void *arg)
{
  char *event_type;
  
  if (events & BEV_EVENT_CONNECTED) {
    /* 
     * Normally
     */
    printf("Try to connect to server\n");
  } else {
    if (events & BEV_EVENT_EOF) {
      event_type = "BEV_EVENT_EOF";
    } else if (events & BEV_EVENT_ERROR) {
      event_type = "BEV_EVENT_ERROR";
    } else if (events & BEV_EVENT_TIMEOUT) {
      event_type = "BEV_EVENT_TIMEOUT";
    } else {
      event_type = "BEV_EVENT_UNKNOWN";
    }
    
    /*
     * Other case: BEV_EVENT_EOF, BEV_EVENT_ERROR, BEV_EVENT_TIMEOUT...
     */
     printf("Get event[%s]\n", event_type);
    
    /*
     * Disable read
     */
    bufferevent_disable(bev, EV_READ);

    /*
     * Free
     */
    bufferevent_free(bev);
  }
}

/*
 * connect_server()
 */
int connect_server (struct event_base *evbase, char *hostname, int port)
{
  struct bufferevent *bev;
  
  /*
   * Create bufferevent
   */
  if ((bev = bufferevent_socket_new(evbase, -1, BEV_OPT_CLOSE_ON_FREE))
      == NULL) {
    printf("Cannot create bufferevent\n");
    return -1;
  }

  if (bufferevent_socket_connect_hostname(bev, NULL, AF_INET,
      hostname, port) != 0) {
    printf("Cannot connect to [%s:%d]\n", hostname, port);
    return -1;
  }

  /*
   * Set callback functions
   */
  bufferevent_setcb(bev, read_cb, write_cb, event_cb, NULL);

  /*
   * Enable read/write event
   */
  bufferevent_enable(bev, EV_READ | EV_WRITE | EV_PERSIST);
  
  /*
   * Wirte data
   */
  bufferevent_write(bev, "Hello world", 11);
  
  return 0;
}

/*
 * main()
 */
int main (int argc, char **argv)
{
  struct event_base *evbase = NULL;
  char              *ip;
  int               port;
  int               ret = 0;
  
  /* 
   * Target server ip, port
   */
  ip = argv[1];
  port = atoi(argv[2]);
  
  /*
   * Init. event base
   */
  if ((evbase = event_base_new()) == NULL) {
    printf("Cannot create event base.\n");
    ret = -1;
    goto end;
  }
  
  /*
   * Connect to server
   */
  if ((ret = connect_server(evbase, ip, port)) != 0) {
    printf("Connect to server failed.\n");
    goto end;
  }

  /*
   * Enter event loop
   */
  event_base_dispatch(evbase);

end:
  /*
   * Free event base
   */
  if (evbase)
    event_base_free(evbase);

  return ret;
}
