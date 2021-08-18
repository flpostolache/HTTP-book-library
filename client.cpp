#include "json.hpp"
#include <iostream>
#include <bits/stdc++.h>
#include "helpers.hpp"
#include "requests.hpp"
#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>

using json = nlohmann::json;
using namespace std;

int main()
{
    char mesaj_de_la_tastatura[200];
    char *message;
    char *response;
    int sockfd;
    char *cookie_de_sesiune = NULL;
    char *token_acces_bibli = NULL;

    while (1)
    {
        //citesc ce comanda primesc de la client
        fgets(mesaj_de_la_tastatura, 200, stdin);
        mesaj_de_la_tastatura[strlen(mesaj_de_la_tastatura) - 1] = '\0';
        //dupa fiecare comanda citita
        sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);

        //daca comanda de la client este "register"
        if (strcmp(mesaj_de_la_tastatura, "register") == 0)
        {
            json js;

            //citesc numele de utilizator si il adaug in json
            cout << "username=";
            fgets(mesaj_de_la_tastatura, 200, stdin);
            mesaj_de_la_tastatura[strlen(mesaj_de_la_tastatura) - 1] = '\0';
            js["username"] = mesaj_de_la_tastatura;

            //citesc parola si il adaug in json
            cout << "password=";
            fgets(mesaj_de_la_tastatura, 200, stdin);
            mesaj_de_la_tastatura[strlen(mesaj_de_la_tastatura) - 1] = '\0';
            js["password"] = mesaj_de_la_tastatura;

            //trimit un mesaj de tip post, il afisez si il trimit server-ului
            message = compute_post_request("34.118.48.238", "/api/v1/tema/auth/register",
                                           "application/json", js, NULL, NULL, 0);
            
            puts(message);
            send_to_server(sockfd, message);

            //astept raspunsul de la server si il afisez
            response = receive_from_server(sockfd);
            puts(response);
        }

        //daca comanda de la client este "login"
        if (strcmp(mesaj_de_la_tastatura, "login") == 0)
        {
            json js;

            //citesc numele de utilizator si il adaug in json
            cout << "username=";
            fgets(mesaj_de_la_tastatura, 200, stdin);
            mesaj_de_la_tastatura[strlen(mesaj_de_la_tastatura) - 1] = '\0';
            js["username"] = mesaj_de_la_tastatura;

            //citesc parola si il adaug
            cout << "password=";
            fgets(mesaj_de_la_tastatura, 200, stdin);
            mesaj_de_la_tastatura[strlen(mesaj_de_la_tastatura) - 1] = '\0';
            js["password"] = mesaj_de_la_tastatura;

            //trimit un mesaj de tip post, il afisez si il trimit server-ului
            message = compute_post_request("34.118.48.238", " /api/v1/tema/auth/login",
                                           "application/json", js, NULL, NULL, 0);
            puts(message);
            send_to_server(sockfd, message);

            //astept raspunsul de la server si il afisez
            response = receive_from_server(sockfd);
            puts(response);

            //in cazul in care primesc cookie-ul de sesiune, il salvez
            char *aux = strstr(response, "connect.sid");
            if (aux != NULL)
            {
                cookie_de_sesiune = new char[500];
                char *aux2 = strtok(aux, ";");
                strcpy(cookie_de_sesiune, aux);
            }
        }

        //daca comanda de la client este "enter_library"
        if (strcmp(mesaj_de_la_tastatura, "enter_library") == 0)
        {
            //daca avem cookie-ul de sesiune
            if (cookie_de_sesiune != NULL)
            {
                //trimit mesaj cu cookie-ul de sesiune
                char **cookies = new char *[10];
                cookies[0] = new char[500];
                strcpy(cookies[0], cookie_de_sesiune);
                message = compute_get_request("34.118.48.238", "/api/v1/tema/library/access",
                                          NULL, NULL, cookies, 1);
            }
            else
                //trimit mesaj fara cookie-ul de sesiune
                message = compute_get_request("34.118.48.238", "/api/v1/tema/library/access",
                                          NULL, NULL, NULL, 0);

            //afisez mesajul si il trimit
            puts(message);
            send_to_server(sockfd, message);

            //astept sa primesc raspunsul de la server
            response = receive_from_server(sockfd);
            puts(response);

            //in cazul in care primesc token-ul de acces la biblioteca, il salvez
            char *aux = strstr(response, "token");
            if (aux != NULL)
            {
                token_acces_bibli = new char[500];
                char *aux2 = strtok(aux + 8, "\"");
                strcpy(token_acces_bibli, aux + 8);
            }
        }

        //daca comanda de la client este "get_books"
        if (strcmp(mesaj_de_la_tastatura, "get_books") == 0)
        {
            //trimit mesaj pentru a cere lista de carti
            message = compute_get_request("34.118.48.238", "/api/v1/tema/library/books",
                                          token_acces_bibli, NULL, NULL, 0);
            puts(message);
            send_to_server(sockfd, message);

            //astept sa primesc raspuns de la server
            response = receive_from_server(sockfd);
            puts(response);
        }

        //daca comanda de la client este "get_book"
        if (strcmp(mesaj_de_la_tastatura, "get_book") == 0)
        {

            //citesc id-ul cartii despre care vreau sa aflu informatii
            cout << "id=";
            fgets(mesaj_de_la_tastatura, 200, stdin);
            mesaj_de_la_tastatura[strlen(mesaj_de_la_tastatura) - 1] = '\0';

            //trimit mesaj pentru a afla informatii despre o carte cu un anumit id
            message = compute_get_request("34.118.48.238", "/api/v1/tema/library/books/",
                                          token_acces_bibli, mesaj_de_la_tastatura, NULL, 0);
            puts(message);
            send_to_server(sockfd, message);

            //astept sa primesc raspuns de la server
            response = receive_from_server(sockfd);
            puts(response);
        }

        //daca comanda de la client este "add_book"
        if (strcmp(mesaj_de_la_tastatura, "add_book") == 0)
        {
            json js;

            //citesc ce titlu are cartea pe care vreau sa o adaug
            cout << "title=";
            fgets(mesaj_de_la_tastatura, 200, stdin);
            mesaj_de_la_tastatura[strlen(mesaj_de_la_tastatura) - 1] = '\0';
            js["title"] = mesaj_de_la_tastatura;

            //citesc ce autor are cartea pe care vreau sa o adaug
            cout << "author=";
            fgets(mesaj_de_la_tastatura, 200, stdin);
            mesaj_de_la_tastatura[strlen(mesaj_de_la_tastatura) - 1] = '\0';
            js["author"] = mesaj_de_la_tastatura;

            //citesc ce gen are cartea pe care vreau sa o adaug
            cout << "genre=";
            fgets(mesaj_de_la_tastatura, 200, stdin);
            mesaj_de_la_tastatura[strlen(mesaj_de_la_tastatura) - 1] = '\0';
            js["genre"] = mesaj_de_la_tastatura;

            //citesc cine a publicat cartea pe care vreau sa o adaug
            cout << "publisher=";
            fgets(mesaj_de_la_tastatura, 200, stdin);
            mesaj_de_la_tastatura[strlen(mesaj_de_la_tastatura) - 1] = '\0';
            js["publisher"] = mesaj_de_la_tastatura;

            //citesc numarul de pagini al cartii pe care vreau sa o adaug
            cout << "page_count=";
            fgets(mesaj_de_la_tastatura, 200, stdin);
            mesaj_de_la_tastatura[strlen(mesaj_de_la_tastatura) - 1] = '\0';
            js["page_count"] = mesaj_de_la_tastatura;

            //formez si trimit server-ului un mesaj cu datele despre noua carte pe
            //care vreau sa o adaug
            message = compute_post_request("34.118.48.238", "/api/v1/tema/library/books",
                                           "application/json", js, token_acces_bibli, NULL, 0);
            puts(message);
            send_to_server(sockfd, message);

            //astept raspuns de la server si il afisez
            response = receive_from_server(sockfd);
            puts(response);
        }

        //daca comanda de la client este "delete_book"
        if (strcmp(mesaj_de_la_tastatura, "delete_book") == 0)
        {
            //citesc id-ul cartii pe care vreau sa o sterg
            cout << "id=";
            fgets(mesaj_de_la_tastatura, 200, stdin);
            mesaj_de_la_tastatura[strlen(mesaj_de_la_tastatura) - 1] = '\0';

            //formez si trimit server-ului un mesaj cu datele necesare pentru a 
            //sterge cartea cu id-ul primit
            message = compute_delete_request("34.118.48.238", "/api/v1/tema/library/books/",
                                             mesaj_de_la_tastatura, token_acces_bibli);
            puts(message);
            send_to_server(sockfd, message);

            //astept raspuns de la server si il afisez
            response = receive_from_server(sockfd);
            puts(response);
        }

        //daca comanda de la client este "exit"
        if (strcmp(mesaj_de_la_tastatura, "exit") == 0)
        {
            break;
        }

        //daca comanda de la client este "logout"
        if (strcmp(mesaj_de_la_tastatura, "logout") == 0)
        {

            //in cazul in care am cookie-ul de sesiune, il includ in mesaj
            if (cookie_de_sesiune != NULL)
            {
                char **cookies = new char *[10];
                cookies[0] = new char[500];
                strcpy(cookies[0], cookie_de_sesiune);
                message = compute_get_request("34.118.48.238", "/api/v1/tema/auth/logout",
                                          NULL, NULL, cookies, 1);
            }
            else
                message = compute_get_request("34.118.48.238", "/api/v1/tema/auth/logout",
                                          NULL, NULL, NULL, 0);
            puts(message);
            send_to_server(sockfd, message);

            //astept raspuns de la server, daca m-am delogat cu succes
            response = receive_from_server(sockfd);
            puts(response);

            //in cazul in care am primit un token JWT, dar m-am delogat, sterg acest token
            //din memorie
            if (token_acces_bibli != NULL)
            {
                free(token_acces_bibli);
                token_acces_bibli = NULL;
            }

            //in cazul in care am primit un cookie de sesiune, dar m-am delogat, 
            //sterg acest token din memorie
            if (cookie_de_sesiune != NULL)
            {
                free(cookie_de_sesiune);
                cookie_de_sesiune = NULL;
            }
        }

        //inchid conexiunea dupa fiecare comanda, valida sau invalida citita de la
        //tastatura
        close_connection(sockfd);
    }
    //inchid inca o data conexiunea de pe socket, dupa ce am dat exit
    close_connection(sockfd);
    return 0;
}