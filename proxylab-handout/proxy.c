#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3";

/* func */
void *thread(void *vargp);
void doit(int fd);
void parse_request_headers(rio_t *rp, char (*header)[1000], int *header_len);
int parse_url(char *url, char *host, char *path, char *port);
void clienterror(int fd, char *cause, char *errnum,
                 char *shortmsg, char *longmsg);

// 第一部分，实现单进程连接和转发
// 解析 http 请求 和 发送请求，接收终端服务器响应
// 暂时只接收 GET 请求
// 要求：
// 1. host 取 url 中的域名
// 2. User-Agent 取上面的静态变量
// 3. Connection 默认 close
// 4. Proxy-Connection 默认 close
// 5. 其他 header 直接加上
// 6. response 直接给客户端

// 第二部分，实现并发
int main(int argc, char **argv)
{
    int listenfd, *connfd;
    char hostname[MAXLINE], port[10];
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid;

    /* Check command line args */
    if (argc != 2)
    {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }

    listenfd = Open_listenfd(argv[1]);
    printf("Proxy start listen on port:%s\n", argv[1]);
    while (1)
    {
        clientlen = sizeof(clientaddr);
        connfd = Malloc(sizeof(int));
        *connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen); //line:netp:tiny:accept
        Getnameinfo((SA *)&clientaddr, clientlen, hostname, MAXLINE,
                    port, MAXLINE, 0);
        printf("Accepted connection from (%s, %s)\n", hostname, port);

        /* start a thread */
        Pthread_create(&tid, NULL, thread, connfd);
    }
}

void *thread(void *vargp)
{
    int connfd = *((int *)vargp);
    Pthread_detach(Pthread_self());
    Free(vargp);
    doit(connfd);  //line:netp:tiny:doit
    Close(connfd); //line:netp:tiny:close
    return NULL;
}

void doit(int fd)
{
    int header_len = 0;
    char buf[MAXLINE], method[10], url[MAXLINE], version[15];
    char host[MAXLINE], path[MAXLINE], port[10] = "80"; // 端口默认是 80
    char headers[20][1000];
    rio_t rio;

    /* Read request line and headers */
    Rio_readinitb(&rio, fd);
    if (!Rio_readlineb(&rio, buf, MAXLINE)) //line:netp:doit:readrequest
        return;
    printf("====request====\n");
    printf("%s", buf);
    sscanf(buf, "%s %s %s", method, url, version); //line:netp:doit:parserequest

    // 判断不是 GET 返回错误
    if (strcasecmp(method, "GET"))
    { //line:netp:doit:beginrequesterr
        clienterror(fd, method, "501", "Not Implemented",
                    "Tiny does not implement this method");
        return;
    } //line:netp:doit:endrequesterr

    // 解析 url 取出 host 和 path
    if (!parse_url(url, host, path, port))
    {
        clienterror(fd, method, "401", "Bad request",
                    "The request url is incorrect");
        return;
    }

    // 读取其他头
    parse_request_headers(&rio, headers, &header_len);

    // 发送请求给终端服务器
    int clientfd;
    rio_t client_rio;
    char request[MAXLINE], client_buf[MAXLINE];

    clientfd = Open_clientfd(host, port);
    if (clientfd < 0)
    {
        clienterror(fd, method, "500", "end server connect failed",
                    "Maybe try again ?");
        return;
    }

    Rio_readinitb(&client_rio, clientfd);

    sprintf(request, "GET %s HTTP/1.0\r\nHost: %s:%s\r\nUser-Agent: %s\r\nConnection: close\r\nPoxy-Connection: close\r\n", path, host, port, user_agent_hdr);

    // 将多的请求头加入
    for (int i = 0; i < header_len; i++)
    {
        sprintf(request, "%s%s", request, headers[i]);
    }
    int len = strlen(request);
    request[len] = '\r'; // 最后加上 "\r\n" 标志结束
    request[len + 1] = '\n';
    request[len + 2] = '\0';

    printf("request struct: %s\n", request);

    // 发送请求 & 接收响应给客户端
    Rio_writen(clientfd, request, strlen(request));
    // ???? 不懂
    // if (!Rio_readlineb(&client_rio, client_buf, MAXLINE))
    // {
    //     clienterror(fd, method, "500", "end server no response",
    //                "Please make sure your url is correct.");
    //    return;
    // }
    printf("=====response=====\n");
    size_t n = 0;
    while ((n = Rio_readlineb(&client_rio, client_buf, MAXLINE)) != 0)
    {
        Rio_writen(fd, client_buf, n);
        printf("%s", client_buf);
    }
}

// 读取每一行的头文件，除了指定的文件，其他都加到请求去
void parse_request_headers(rio_t *rp, char (*header)[1000], int *header_len)
{
    *header_len = 0;
    char buf[MAXLINE];
    char header_name[20];
    char tmp[MAXLINE];

    while (1)
    { //line:netp:readhdrs:checkterm
        Rio_readlineb(rp, buf, MAXLINE);
        if (!strcmp(buf, "\r\n"))
            break;
        // printf("%s", buf);
        sscanf(buf, "%s %s", header_name, tmp);
        // 判断不是那几个固定的 header 就拿出来
        if (strcmp(header_name, "Host:") != 0 && strcmp(header_name, "User-Agent:") != 0 && strcmp(header_name, "Connection:") != 0 && strcmp(header_name, "Proxy-Connection:") != 0)
        {
            strcpy(header[*header_len], buf);
            (*header_len)++;
        }
    }
    return;
}

// 只需要分离 host 和后面的 path 即可
// 需要判断请求是否正确,示例：
// http://www.baidu.com/123/dasd/ 也可能没有最后的斜杆
int parse_url(char *url, char *host, char *path, char *port)
{
    if (strlen(url) <= 7)
        return 0;
    // 从头开始解析
    if (!strnstr(url, "http://", 7))
        return 0; // 解析是否是 http 1.1

    char *tmp = &url[7];
    int index = 7;
    for (; index < strlen(url);)
    {
        if (url[index] == '/')
            break;
        index++;
    }

    strncpy(host, tmp, index - 7);
    host[index - 7] = '\0';

    if (index != strlen(url))
    {
        char *tmp = &url[index];
        strcpy(path, tmp);
    }
    else
    {
        strcpy(path, "/");
    }

    // 新增解析端口
    if (strstr(host, ":"))
    {
        char *tmp;
        for (int i = 0; i < strlen(host); i++)
        {
            if (host[i] == ':')
            {
                i++;
                if (i == strlen(host))
                    return 0;
                tmp = &host[i];
                if (strspn(tmp, "0123456789") != strlen(tmp))
                    return 0;
                strcpy(port, tmp);
                host[i - 1] = '\0';
                break;
            }
        }
    }

    return 1;
}

void clienterror(int fd, char *cause, char *errnum,
                 char *shortmsg, char *longmsg)
{
    char buf[MAXLINE];

    /* Print the HTTP response headers */
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n\r\n");
    Rio_writen(fd, buf, strlen(buf));

    /* Print the HTTP response body */
    sprintf(buf, "<html><title>Tiny Error</title>");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<body bgcolor="
                 "ffffff"
                 ">\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "%s: %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<p>%s: %s\r\n", longmsg, cause);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<hr><em>The Tiny Web server</em>\r\n");
    Rio_writen(fd, buf, strlen(buf));
}
