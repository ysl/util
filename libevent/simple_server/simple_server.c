/*
 * Base on bufferevent.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <event.h>
#include <event2/listener.h>


#define SVR_IP                          "127.0.0.1"
#define SVR_PORT                        8088

/*
 * read_cb()
 */
static void read_cb (struct bufferevent *bev, void *arg)
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
static void event_cb (struct bufferevent *bev, short events, void *arg)
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
 * accept_cb()
 */
static void accept_cb (struct evconnlistener *lev,
                       evutil_socket_t fd, struct sockaddr *sa,
                       int socklen, void *arg)
{
  struct event_base   *evbase = arg;
  struct bufferevent  *bev;

  /*
   * Create bufferevent.
   */
  bev = bufferevent_socket_new(evbase, fd, BEV_OPT_CLOSE_ON_FREE);
  if (bev == NULL) {
    printf("Create bufferevent failed.\n");
    return;
  }

  /*
   * Set callback function.
   */
  bufferevent_setcb(bev, read_cb, NULL, event_cb, NULL);

  /*
   * Enable read event.
   */
  bufferevent_enable(bev, EV_READ);
}

/*
 * evl_init()
 */
static int evl_init (struct event_base *evbase)
{
  struct evconnlistener  *evl;
  struct sockaddr_in     sin;                   // connection address
  uint32_t               flags;                 // socket options

  /*
   * Set up server address.
   */
  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = inet_addr(SVR_IP);
  sin.sin_port = htons(SVR_PORT);

  /*
   * Bind listen socket.
   */
  flags = (LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE);
  evl = evconnlistener_new_bind(evbase, accept_cb, (void *)evbase,
      flags, -1, (struct sockaddr *)&sin, sizeof(sin));

  if (evl == NULL) {
    printf("Bind [%s:%d] failed\n", SVR_IP, SVR_PORT);
    return -1;
  } else {
    printf("Bind [%s:%d] successfully\n", SVR_IP, SVR_PORT);
    return 0;
  }
}

/*
 * main()
 */
int main (int argc, char **argv)
{
  struct event_base *evbase = NULL;
  int               err = 0;
  
  /*
   * Init. event base.
   */
  if ((evbase = event_base_new()) == NULL) {
    printf("Cannot create event base.\n");
    err = -1;
    goto end;
  }
  
  /*
   * Create listener.
   */
  if ((err = evl_init(evbase)) != 0) {
    printf("Create listener failed.\n");
    goto end;
  }

  /*
   * Enter event loop.
   */
  event_base_dispatch(evbase);

end:
  /*
   * Free event base.
   */
  if (evbase)
    event_base_free(evbase);

  return err;
}
