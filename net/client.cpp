#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <SFML/Graphics.hpp>
#include <unistd.h>

using namespace std;

void error_handling(string message);

int main(int argc, char const *argv[])
{
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_adr;
    int read_cnt, read_len;

    if(argc != 3)
    {
        cout << "Usage: " << string(argv[0]) << "<ip> <port>" << endl;
        exit(1);
    }

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_adr.sin_port = htons(atoi(argv[2]));

    int ret = connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr));
    if(ret == -1)
        error_handling("connect() error");

    while(read)
    
    return 0;
}


void error_handling(string message)
{
    cerr << message << endl;
    exit(1);
}