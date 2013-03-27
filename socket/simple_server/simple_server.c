#include <stdio.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define SVR_IP                          "127.0.0.1"
#define SVR_PORT                        8088
#define BUF_SIZE                        1024


fd_set      active_fd_set;
int         sockfd;
int         max_fd;

/*
 * read_cb()
 *
 * read data from current fd.
 */
static int read_cb (int fd)
{
  char                buff[BUF_SIZE];
  int recv_len;

  /*
   * Receive data.
   */
  memset(buff, 0, sizeof(buff));
  recv_len = recv(fd, buff, sizeof(buff), 0);
  if (recv_len == -1) {
    perror("recv()");
    return -1;

  } else if (recv_len == 0) {
    printf("Client disconnect\n");

  } else {
    printf("Receive: len=[%d] msg=[%s]\n", recv_len, buff);

    /*
     * Send (In fact we should determine when it can be written)
     */
    send(fd, buff, recv_len, 0);
  }

  /*
   * Close socket.
   */
  close(fd);
  FD_CLR(fd, &active_fd_set);
}

/*
 * accept_cb()
 */
static int accept_cb()
{
  /*
   * Connection request on listen socket.
   */
  struct sockaddr_in  client_addr;
  socklen_t           len;
  int                 clnt_fd;

  /*
   * Accept.
   */
  clnt_fd = accept(sockfd, (struct sockaddr *)&client_addr, &len);
  if (clnt_fd == -1) {
    perror("accept()");
    return -1;

  } else {
    printf("Accept client come from [%s:%u] by fd [%d]\n",
           inet_ntoa(client_addr.sin_addr),
           ntohs(client_addr.sin_port), clnt_fd);

    /*
     * Add to fd set, check it in next run.
     */
    FD_SET(clnt_fd, &active_fd_set);

    /*
     * Check the max fd number.
     */
    if (clnt_fd > max_fd)
      max_fd = clnt_fd;

    return 0;
  }
}

int init_socket ()
{
  struct sockaddr_in  server_addr;
  socklen_t           len;
  int                 flag = 1;

  /*
   * Set server address.
   */
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(SVR_IP);
  server_addr.sin_port = htons(SVR_PORT);
  len = sizeof(struct sockaddr_in);

  /*
   * Create socket.
   */
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      perror("socket()");
      return -1;
  } else {
      printf("sockfd=[%d]\n", sockfd);
  }

  /*
   * Set socket option.
   */
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int)) < 0) {
      perror("setsockopt()");
      return -1;
  }

  /*
   * Bind address.
   */
  if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
      perror("bind()");
      return -1;
  } else {
      printf("bind [%s:%u] success\n",
          inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
  }

  /*
   * Listen.
   */
  if (listen(sockfd, 128) == -1) {
      perror("listen()");
      return -1;
  }

  return 0;
}

/*
 * main()
 */
int main (int argc, char **argv)
{
  if (init_socket(&sockfd) < 0) {
    printf("Init the socket failed.\n");
    return -1;
  }

  /*
   * Clean fd set.
   */
  FD_ZERO(&active_fd_set);
  FD_SET(sockfd, &active_fd_set);
  max_fd = sockfd;

  while (1) {
    int             err;
    struct timeval  tv;
    fd_set          read_fds;  // Only use in current run.

    /*
     * Set select timeout.
     */
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    /*
     * Copy fd set.
     */
    read_fds = active_fd_set;
    err = select(max_fd + 1, &read_fds, NULL, NULL, &tv);
    if (err == -1) {
      perror("select()");
      return -1;

    } else if (err == 0) {
      printf("Select timeout\n");
      continue;

    } else {
      /*
       * Service all sockets.
       */
      int i;
      for (i = 0; i < FD_SETSIZE; i++) {
        if (FD_ISSET(i, &read_fds)) {
          if (i == sockfd) {
            accept_cb();
          } else {
            read_cb(i);
          }
        }
      } // end of for

    } // end of if

  } // end of while

  return 0;
}
