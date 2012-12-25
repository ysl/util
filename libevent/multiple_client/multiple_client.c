/*
 * Base on bufferevent.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <event.h>
#include <time.h>

#define INTERVAL 100 * 1000


/*
 * Global object
 */
struct global_st {
  char              *ip;
  int               port;
  struct event_base *evbase;
  struct event      *timer_ev;
  int               conn_num;
  int               count;
} g;

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
 * timer_cb()
 */
void timer_cb (int fd, short event, void *arg)
{
  struct timeval    tm;

  if (g.count >= g.conn_num)
    return;
    
  printf("Client[%d]\n", g.count++);

  /* 
   * Add the original event
   */
  evutil_timerclear(&tm);
  tm.tv_usec = INTERVAL;
  if (evtimer_add(g.timer_ev, &tm) != 0) {
    printf("Cannot add timer event.\n");
  }

  /*
   * Connect to server
   */
  if (connect_server(g.evbase, g.ip, g.port) != 0) {
    printf("Connect to server failed.\n");
  }
}

/*
 * main()
 */
int main (int argc, char **argv)
{
  int               ret = 0;
  struct timeval    tm;
  
  /*
   * Show usage
   */
  if (argc != 4) {
    printf("Usage:\n\t%s [ip] [port] [conn_num]\n", argv[0]);
    return 0;
  }
  
  /* 
   * Set target server ip, port.
   */
  g.ip = argv[1];
  g.port = atoi(argv[2]);
  g.evbase = NULL;
  g.timer_ev = NULL;
  g.count = 0;
  g.conn_num = atoi(argv[3]);
  
  /*
   * Init. event base
   */
  if ((g.evbase = event_base_new()) == NULL) {
    printf("Cannot create event base.\n");
    ret = -1;
    goto end;
  }
  
  /*
   * Use timer callback to launch multiple client.
   */
  evutil_timerclear(&tm);
  tm.tv_usec = INTERVAL;
  g.timer_ev = evtimer_new(g.evbase, timer_cb, NULL);
  if (g.timer_ev == NULL) {
    printf("Cannot create event.\n");
    ret = -1;
    goto end;
  }
  
  if (evtimer_add(g.timer_ev, &tm) != 0) {
    printf("Cannot add timer event.\n");
    ret = -1;
    goto end;
  }

  /*
   * Enter event loop
   */
  event_base_dispatch(g.evbase);

end:
  /*
   * Free object
   */
  if (g.timer_ev)
    event_free(g.timer_ev);
  if (g.evbase)
    event_base_free(g.evbase);

  return ret;
}
