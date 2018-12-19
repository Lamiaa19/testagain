/**
 * server6.c: TCP_MD5SIG
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>

#include <arpa/inet.h>

#define BUFFERSZ 256

struct tcp_md5sig md5;

static inline void error(char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[BUFFERSZ], *key, *s6ip, *sport, *c6ip, *cport;
    struct sockaddr_in6 serv_addr, saddr, cli_addr;
    int n, r;
    char client_addr_ipv6[100];

    if (argc != 6) {
        fprintf(stderr, "Usage: %s <server IPv6> <server port> "
                "<client IPv6> <client port> <MD5 key>\n",
                argv[0]);
        return -1;
    }

    s6ip = argv[1];
    sport = argv[2];
    c6ip = argv[3];
    cport = argv[4];
    key = argv[5];

    printf("\nIPv6 TCP Server (TCP_MD5SIG) ...\n");

    //Sockets Layer Call: socket()
    sockfd = socket(AF_INET6, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(sport);
    serv_addr.sin6_family = AF_INET6;
    if(inet_pton(AF_INET6, s6ip, &serv_addr.sin6_addr) <= 0)
        error("ERROR on inet_pton");
    serv_addr.sin6_port = htons(portno);

    //Sockets Layer Call: bind()
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
                sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    // Client sockaddr_in6 for TCPMD5_SIG
    memset(&saddr, '0', sizeof(saddr));
    saddr.sin6_family = AF_INET6;
    if(inet_pton(AF_INET6, c6ip, &saddr.sin6_addr)<=0)
        error("ERROR on inet_pton");
    saddr.sin6_port = htons(atoi(cport));
    md5.tcpm_addr = *(struct sockaddr_storage *) &saddr;
    strcpy(md5.tcpm_key, key);
    md5.tcpm_keylen = strlen(key);
    if ((r = setsockopt(sockfd, IPPROTO_TCP, TCP_MD5SIG,
                    &md5, sizeof(md5))) < 0)
        error("listen setsockopt TCP_MD5SIG");

    //Sockets Layer Call: listen()
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    //Sockets Layer Call: accept()
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0)
        error("ERROR on accept");

    //Sockets Layer Call: inet_ntop()
    inet_ntop(AF_INET6, &(cli_addr.sin6_addr),client_addr_ipv6, 100);
    printf("Incoming connection from client having IPv6 address: %s\n",
            client_addr_ipv6);

    memset(buffer,0, BUFFERSZ);

    //Sockets Layer Call: recv()
    n = recv(newsockfd, buffer, BUFFERSZ-1, 0);
    if (n < 0)
        error("ERROR reading from socket");

    printf("Message from client: %s\n", buffer);

    //Sockets Layer Call: send()
    n = send(newsockfd, "Server got your message",
            strlen("Server got your message"), 0);
    if (n < 0)
        error("ERROR writing to socket");

    //Sockets Layer Call: close()
    close(sockfd);
    close(newsockfd);

    return 0;

	char *str2md5(const char *str, int length) {
    int n;
    MD5_CTX c;
    unsigned char digest[16];
    char *out = (char*)malloc(33);

    MD5_Init(&c);

    while (length > 0) {
        if (length > 512) {
            MD5_Update(&c, str, 512);
        } else {
            MD5_Update(&c, str, length);
        }
        length -= 512;
        str += 512;
    }

    MD5_Final(digest, &c);

    for (n = 0; n < 16; ++n) {
        snprintf(&(out[n*2]), 16*2, "%02x", (unsigned int)digest[n]);
    }

    return out;
}
int odbierz_i_policz_md5(int gniazdo,char *str1, int length)
{
    char *out = (char*)malloc(33);
    char *wiad;

    if(recv(gniazdo, out, 33,0) != -1)
    {
        wiad = str2md5(str1,length) ;
        if (strncmp (wiad,out,33) == 0)
        {
            free(out);
            free(wiad);
            return 1;
        }
        else
        {
            free(out);
            free(wiad);
            return 0;
        }
    }
    else{
        fprintf(stdout,"Recv error! \n");
        exit(EXIT_FAILURE);
    }

}

}
