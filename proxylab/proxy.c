/*
 * proxy.c - CS:APP Web proxy
 *
 * TEAM MEMBERS:
 *     
 *
 * IMPORTANT: Give a high level description of your code here. You
 * must also provide a header comment at the beginning of each
 * function that describes what that function does.
 */

#include "csapp.h"

/*
 * Function prototypes
 */
int parse_uri(char *uri, char *target_addr, char *path, int  *port);
void format_log_entry(char *logstring, struct sockaddr_in *sockaddr, char *uri, int size);

FILE *logfile;
void process(int connfd, void *addr_va, int listenfd);
/*
 * main - Main routine for the proxy program
 */
int main(int argc, char **argv)
{
    int connfd, listenfd, port;
    struct sockaddr_in *clientaddr = malloc(sizeof(struct sockaddr_in));
    socklen_t clientlen;

    /* Check arguments */
    if (argc != 2) {
      fprintf(stderr, "Usage: %s <port number>\n", argv[0]);
      exit(0);
    }

    Signal(SIGPIPE, SIG_IGN);

    //Convert string argv[1] into integer port
    port = atoi(argv[1]);

    //Open proxy server that will listen socket
    listenfd = Open_listenfd(port);
    clientlen = sizeof(struct sockaddr_in);
    //Create and open file proxy.log
    logfile = fopen("proxy.log", "a");

    while (1) { //loop
      printf("listenfd: %8x, I'm listening..\n", listenfd);
      //Accept from server
      connfd = Accept(listenfd, (SA *)clientaddr, &clientlen);
      printf("connfd:%8x\n", connfd);

	    //Go to the function process
	    process(connfd, (void *)clientaddr, listenfd);
    }

    free(clientaddr);
    Close(listenfd);
    fclose(logfile);
    return 0;
    //exit(0);
}

/*
 * process - Does the main process of the proxy server.
 * This function reads requests from client, and send them to the server.
 * And it reads responses from the sever, and send them again to the client.
 */

void process(int fd, void *addr_var, int listenfd){
    struct sockaddr_in *clientaddr = (struct sockaddr_in *)addr_var;
    char buf[MAXLINE];
    int totsize = 0;
    rio_t rio_client, rio_server;

    //Read first request from client
    Rio_readinitb(&rio_client, fd);
    Rio_readlineb(&rio_client, buf, MAXLINE);
    printf("%s", buf);

    //Get method, uri, and version from the request of the client
    char method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    if (sscanf(buf, "%s %s %s", method, uri, version) < 3) {
      printf("No method or uri or version\n");
      free(clientaddr);
      fclose(logfile);
      Close(listenfd);
	    exit(0);
    }
    //If the method is not "GET", close the connection and program.
    if (strcmp(method, "GET")) {
      printf("no GET\n");
      if (!strcmp(method, "CONNECT")) return;
      free(clientaddr);
      fclose(logfile);
      Close(listenfd);
      exit(0);
    }

    //Parse the uri, and get the hostname, pathname, and the port.
    char hostname[MAXLINE], pathname[MAXLINE];
    int port;
    if (parse_uri(uri, hostname, pathname, &port)) {
      printf("can't parse uri\n");
      free(clientaddr);
      fclose(logfile);
      Close(listenfd);
      exit(0);
    }

    //After receiving the correct request, Open connection to the server
    printf("OK\n");
    int n, clientfd;
    clientfd = Open_clientfd(hostname, port);
    if (clientfd < 0) {
      return;
    }


    //Send client's requests to the server.
    //Especially send 'method /pathname version'. This makes to view some pages correctly. ex) www.psu.edu
    sprintf(buf, "%s /%s %s\r\n", method, pathname, version);
    write(clientfd, buf, strlen(buf));
    printf("%s", buf);

    while (strcmp(buf, "\r\n")) {
      Rio_readlineb(&rio_client, buf, MAXLINE);

      //ignore the incorrect header, and alert the end of the HTTP header
      //end of the HTTP header
      if (!strcmp(buf, "\r\n")){
	      char *tempbuf = "Connection: close\r\n";
	      write(clientfd, tempbuf, 19);
	      printf("%s", tempbuf);
      }
      //ignore the incorrect header: the headers for the proxy
      if (strncmp(buf, "Keep-Alive:", 11) && strncmp(buf, "Proxy-Connection:", 17)){
        write(clientfd, buf, strlen(buf));
        printf("%s", buf);
      }
      //Rio_writen(clientfd, buf, strlen(buf));
      //printf("%s", buf);
    }

    //link rio_server to clientfd
    Rio_readinitb(&rio_server, clientfd);

    //Read the responses from the server, and write them to the cliet
    while ((n = Rio_readnb(&rio_server, buf, MAXLINE))){
      totsize += n;
      printf("%s", buf);
      write(fd, buf, n);
    }

    //Write Log
    printf("Writing Log\n");
    format_log_entry(buf, clientaddr, uri, totsize);
    fprintf(logfile, "%s\n", buf);
    fflush(logfile);
    Close(clientfd);
    Close(fd);
}


/*
 * parse_uri - URI parser
 *
 * Given a URI from an HTTP proxy GET request (i.e., a URL), extract
 * the host name, path name, and port.  The memory for hostname and
 * pathname must already be allocated and should be at least MAXLINE
 * bytes. Return -1 if there are any problems.
 */
int parse_uri(char *uri, char *hostname, char *pathname, int *port)
{
    char *hostbegin;
    char *hostend;
    char *pathbegin;
    int len;

    if (strncasecmp(uri, "http://", 7) != 0) {
	hostname[0] = '\0';
	return -1;
    }

    /* Extract the host name */
    hostbegin = uri + 7;
    hostend = strpbrk(hostbegin, " :/\r\n\0");
    len = hostend - hostbegin;
    strncpy(hostname, hostbegin, len);
    hostname[len] = '\0';

    /* Extract the port number */
    *port = 80; /* default */
    if (*hostend == ':')
	*port = atoi(hostend + 1);

    /* Extract the path */
    pathbegin = strchr(hostbegin, '/');
    if (pathbegin == NULL) {
	pathname[0] = '\0';
    }
    else {
	pathbegin++;
	strcpy(pathname, pathbegin);
    }

    return 0;
}

/*
 * format_log_entry - Create a formatted log entry in logstring.
 *
 * The inputs are the socket address of the requesting client
 * (sockaddr), the URI from the request (uri), and the size in bytes
 * of the response from the server (size).
 */
void format_log_entry(char *logstring, struct sockaddr_in *sockaddr,
		      char *uri, int size)
{
    time_t now;
    char time_str[MAXLINE];
    unsigned long host;
    unsigned char a, b, c, d;

    /* Get a formatted time string */
    now = time(NULL);
    strftime(time_str, MAXLINE, "%a %d %b %Y %H:%M:%S %Z", localtime(&now));

    /*
     * Convert the IP address in network byte order to dotted decimal
     * form. Note that we could have used inet_ntoa, but chose not to
     * because inet_ntoa is a Class 3 thread unsafe function that
     * returns a pointer to a static variable (Ch 13, CS:APP).
     */
    host = ntohl(sockaddr->sin_addr.s_addr);
    a = host >> 24;
    b = (host >> 16) & 0xff;
    c = (host >> 8) & 0xff;
    d = host & 0xff;


    /* Return the formatted log entry string */
    sprintf(logstring, "%s: %d.%d.%d.%d %s %d", time_str, a, b, c, d, uri, size);
}
