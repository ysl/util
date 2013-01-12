/*
 * Base on evhttp.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <event.h>
#include <evhttp.h>
#include <sys/queue.h>


#define SERVER_HOSTNAME   "127.0.0.1"
#define SERVER_PORT       18088

/*
 * http_cb()
 */
static void http_cb (struct evhttp_request *req, void *arg)
{
  struct evkeyvalq          *headers;
  struct evkeyval           *header;
  char                      *buf;
  struct evbuffer           *input;
  int                       input_len;
  struct evhttp_connection  *evcon = arg;

  /*
   * Show the http status code
   */
  printf("[Response code] %d\n", req->response_code);

  /*
   * Print out the http header
   */
  if ((headers = evhttp_request_get_input_headers(req)) != NULL) {
    TAILQ_FOREACH(header, headers, next) {
      printf("[Header] %s: %s\n", header->key, header->value);
    }
  }

  /*
   * Get returned data
   */
  input = evhttp_request_get_input_buffer(req);
  input_len = evbuffer_get_length(input);
  if (input_len > 0) {
    buf = calloc(1, input_len + 1);
    if (buf) {
      evbuffer_copyout(input, buf, input_len);
      printf("[Content]\n%s", buf);
      free(buf);
    }
  }

  /*
   * Free connection
   * the evhttp_request_get_connection(req) doesn't work
   */
  evhttp_connection_free(evcon);
}

/*
 * launch_request()
 */
void launch_request (struct event_base *evbase, char *hostname, unsigned short port)
{
  struct evhttp_connection  *evhttp_connection = NULL;
  struct evhttp_request     *evhttp_request = NULL;


  /*
   * Create connection to server
   */
  evhttp_connection = evhttp_connection_base_new(evbase, NULL, hostname, port);

  /*
   * Set connection time out
   */
  evhttp_connection_set_timeout(evhttp_connection, 60);

  /*
   * Make request
   */
  evhttp_request = evhttp_request_new(http_cb, evhttp_connection);

  /*
   * Set http header
   */
  evhttp_add_header(evhttp_request->output_headers, "Host", hostname);
  evhttp_add_header(evhttp_request->output_headers, "Content-Type", "text/plain");

  /*
   * Make an http request over the specified connection
   */
  evhttp_make_request(evhttp_connection, evhttp_request, EVHTTP_REQ_GET, "/");
}

/*
 * main()
 */
int main (int argc, char **argv)
{
  char              *hostname;
  unsigned short    port;
  struct event_base *evbase;

  if (argc != 3) {
    printf("Usage: %s [hostname] [port]\n", argv[0]);
    return -1;
  }

  hostname = argv[1];
  port = atoi(argv[2]);

  /*
   * Initialize event base
   */
  if ((evbase = event_base_new()) == NULL) {
    printf("Cannot create event base.\n");
    return -1;
  }

  /*
   * Launch http request
   */
  launch_request(evbase, hostname, port);

  /*
   * Enter event loop.
   */
  event_base_dispatch(evbase);

  /*
   * Free
   */
  event_base_free(evbase);

  return 0;
}
