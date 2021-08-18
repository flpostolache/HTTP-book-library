#include <stdlib.h> /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.hpp"
#include "requests.hpp"
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

char *compute_get_request(char *host, char *url, char *token, char *query_params,
                          char **cookies, int cookies_count)
{
    char *message = new char[BUFLEN];
    char *line = new char[LINELEN];

    // Scriem numele metodei, URL-ul si tipul protocolului
    if (query_params != NULL)
    {
        sprintf(line, "GET %s%s HTTP/1.1", url, query_params);
    }
    else
    {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Adaugam host-ul
    sprintf(line, "Host: %s", host);
    compute_message(message, line);


    // Adaugam cookie-uri
    if (cookies != NULL)
    {
        for (int i = 0; i < cookies_count; i++)
        {
            sprintf(line, "Cookie: %s", cookies[i]);
            compute_message(message, line);
        }
    }

    // Adaugam token-ul JWT de acces
    if (token != NULL)
    {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }

    // Adaug un new line la sfarsit de mesaj
    compute_message(message, "");
    return message;
}

char *compute_post_request(char *host, char *url, char *content_type, json js, char *token,
                           char **cookies, int cookies_count)
{
    char *message = new char[BUFLEN];
    char *line = new char[LINELEN];

    // Scriem numele metodei, URL-ul si tipul protocolului
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);

    // Adaugam host-ul
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    
    // Adaugam tipul a ceea ce trimitem
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    //Calculam si adaugam dimensiunea mesajului pe care vreau sa il transmit
    int len = strlen(js.dump().c_str());

    sprintf(line, "Content-Length: %d", len);
    compute_message(message, line);

    // Adaugam cookie-uri
    if (cookies != NULL)
    {
        for (int i = 0; i < cookies_count; i++)
        {
            sprintf(line, "Cookie: %s", cookies[i]);
            compute_message(message, line);
        }
    }

    // Adaugam token-ul JWT de acces
    if (token != NULL)
    {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }

    // Adaug newline la sfarsitul header-ului
    compute_message(message, "");

    // Adaug datele pe care vreau sa le trimit
    compute_message(message, js.dump().c_str());

    free(line);
    return message;
}

char *compute_delete_request(char *host, char *url, char *token, char *token_de_acces)
{
    char *message = new char[BUFLEN];
    char *line = new char[LINELEN];

    // Scriem numele metodei, URL-ul si tipul protocolului
    sprintf(line, "DELETE %s%s HTTP/1.1", url, token);

    compute_message(message, line);

    // Adaug host-ul
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // Adaugam token-ul JWT de acces
    if (token_de_acces != NULL)
    {
        sprintf(line, "Authorization: Bearer %s", token_de_acces);
        compute_message(message, line);
    }

    // Adaug new line la final de header
    compute_message(message, "");
    return message;
}