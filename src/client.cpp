#include <iostream>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/un.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <stdlib.h>
#include <arpa/inet.h>
#include <vector>
#include <string>
#include <vector>
#include "Graph_Sort.h"

using namespace std;

enum alg_name{BELLMAN,TOPSORT};

const int PARSE_BUF_SIZE = 200;
const int MAX_NODES = 50;
const int BUF_SIZE = 1024;
const int QUEUE_SIZE = 5;

/*
struct sockaddr_in {
    short  sin_family;
    unsigned short   sin_port;
    struct in_addr   sin_addr;
    char   sin_zero[8];
};
*/


//send data
int send_data(int sock,char* mes,int data)
{
    memset(mes,'\0',BUF_SIZE);
    sprintf(mes,"%d",data);
    int num = send(sock, mes, strlen(mes),0);

    sleep(1);
    return num;
}

//Forming string which contain edges
void formEdges(vector<Edge> edges,char* buf)
{
    char temp[PARSE_BUF_SIZE];
    memset(buf,'\0',PARSE_BUF_SIZE);
    memset(temp,'\0',PARSE_BUF_SIZE);
    
    for (vector<Edge>::iterator p = edges.begin();p != edges.end();++p)
    {
        sprintf(temp,"%d,%d,%d,",p->src,p->dest,p->weight);
        strcat(buf,temp);
        memset(temp,'\0',PARSE_BUF_SIZE);
    } 
    sprintf(temp,"%s",".");//end of message
    strcat(buf,temp);
}


//MAIN
int main(int argc,char** argv)
{   

//client's part - create vector consisting of edges  and send it and number of nods to server

// Number of nodes in the graph
    char buf[PARSE_BUF_SIZE];
    int cnt = 1;
    memset(buf,'\0',strlen(buf));
    int N,src,dest;
    bool inp = false;//false - no errors, true - some errors have occured

    alg_name alg_flag = BELLMAN;
 

//choosing the algorithm
    cout << "Input name of algorithm you want server to count path: 0 for algorithm with topological sorting or 1 for Bellman-Ford algorithm: ";
    inp = false;
    while(!inp)
    {
        inp = true;
        cin >> buf;
        if ((buf[0] != '0') && ((buf[0] != '1')))
        {
            cout << "Incorrect number:can't can't be neither zero nor one" << endl;
            inp = false;
            continue;
        }
       if (buf[1] != '\0')
       {
            cout << "Incorrect number,input again please" << endl;
            inp = false;
            break;

       }
    if (inp == true)
        {
            alg_flag = static_cast<alg_name>(atoi(buf));
            break;
        }  
    }

    if (alg_flag == BELLMAN) cout << "You have chosen Bellman-Ford algorithm: remember, it does not support graphs with negative circles and zero edge weights" << endl;
        else cout << "You have chosen Topological sorting algorithm: remember, it does not support graphs with circles(will output appropriate message)" << endl;

    inp = false;
//N input
    while(!inp)
    {
        inp = true;
        cout <<  "Enter number of nodes in the graph: ";
        cin >> buf;
        if (buf[0] == '0')
        {
            cout << "Incorrect number:can't be zero,input again please" << endl;
            inp = false;
            continue;
        }
        for (size_t p = 0;p<strlen(buf);++p)
        {
            if ((buf[p] > '9') || (buf[p] < '0'))
            {
                cout << "Incorrect number,input again please" << endl;
                inp = false;
                break;
            }

        }
    if (inp == true)
        {
            N = atoi(buf);
            break;
        }  
    }

//src input
    inp = false;
    while(!inp)
    {
        inp = true;
        cout <<  "Enter source node of the graph: ";
        cin >> buf;
        if (buf[0] == '0')
        for (size_t p = 0;p<strlen(buf);++p)
        {
            if ((buf[p] > '9') || (buf[p] < '0'))
            {
                cout << "Incorrect number,input again please" << endl;
                inp = false;
                break;
            }

        }
        if (inp == true)
        {
           src = atoi(buf);
           if (src >= N)
           {
                cout << "Source number can't be greater than number of nodes in graph!" << endl;
                inp = false;
           } else break; 
        } 
    }

//dest input
    inp = false;
    while(!inp)
    {
        inp = true;
        cout <<  "Enter destination node of the graph: ";
        cin >> buf;
        if (buf[0] == '0')
        for (size_t p = 0;p<strlen(buf);++p)
        {
            if ((buf[p] > '9') || (buf[p] < '0'))
            {
                cout << "Incorrect number,input again please" << endl;
                inp = false;
                break;
            }

        }
        if (inp == true)
        {
           dest = atoi(buf);
           if (dest >= N)
           {
                cout << "Destination number can't be greater than number of nodes in graph!" << endl;
                inp = false;
           } else break; 
        } 
    }

    inp = false;
    memset(buf,'\0',strlen(buf));
    vector <Edge> edges;
    int from,to,weight;
    cout << "Input graph edge in format <from to> " << endl;
    cout << "To configure you input write \"end\" or \"End\" " << endl;
    for(;;){
        inp = false;
        cout << cnt << " edge:" << endl;
        cout << "From ";
        cin >> buf;
        if ((strcmp(buf,"End") == 0) || (strcmp(buf,"end") == 0))
        {
            cout << "End of the input" << endl;
            break;
        }

        //parse first number - "from"
        for (size_t p = 0;p<strlen(buf);++p)
        {
            if ((buf[p] > '9') || (buf[p] < '0'))
            {
                cout << "Incorrect number,input again please" << endl;
                inp = true;
                break;
            }

        }

        if (inp) continue;
        from = atoi(buf);
        if ((from < 0) || (from >= N))
        {
            cout << "Incorrect graph edges,input again please" << endl;
            continue;
        }
        memset(buf,'\0',strlen(buf));

        cout << "To ";
        cin >> buf;

        //parse second number - "to"
        for (size_t p = 0;p<strlen(buf);++p)
        {
            if ((buf[p] > '9') || (buf[p] < '0'))
            {
                cout << "Incorrect number,input again please" << endl;
                inp = true;
                break;
            }
        }
        if (inp) continue;
        to = atoi(buf);
        if ((to < 0) || (to >= N))
        {
            cout << "Incorrect graph edges,input again please" << endl;
            continue;
        }
    
        memset(buf,'\0',strlen(buf));
        cout << "Weight " << from <<" -> " << to << " ";
        cin >> buf;

        //parse weight between from and to
        for (size_t p = 0;p<strlen(buf);++p)
        {
            if (!(((buf[p] <= '9') && (buf[p] => '0')) || (buf[p] == '-')))
            {
                cout << "Incorrect number,input again please" << endl;
                inp = true;
                break;
            }
        }
        if (inp) continue;
        weight = atoi(buf);

        cnt++;
        Edge temp;
        temp.weight = weight;
        temp.src = from;
        temp.dest = to;
        edges.push_back(temp);
    }

 //<end of graph creating>  


//sending information about graph to server
    int sock;
    struct sockaddr_in serv_addr;
    char mes[BUF_SIZE];

//cleaning messages
    memset(&serv_addr, '\0', sizeof(serv_addr));
    memset(mes,'\0',BUF_SIZE);
    sock = socket(AF_INET, SOCK_STREAM, 0);//opening socket
    if(sock < 0)
    {
        perror("socket");
        exit(1);
    }

//server's structure
    serv_addr.sin_family = AF_INET;
    if (argv[1] == NULL)
    {
        cout << "You must input appropriate port" << endl;
        exit(1);
    }

    serv_addr.sin_port = htons(atoi(argv[1]));//port which process will hold
    
    if (argv[2] == NULL)
    {
        cout << "You must input server's adress" << endl;
        exit(2);
    }

    serv_addr.sin_addr.s_addr = inet_addr(argv[2]);


    if(connect(sock,(struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("connect");
        exit(2);
    }
    recv(sock, mes, BUF_SIZE, 0);
    cout << mes << endl;

//send prefered algorithm id
    send_data(sock,mes,alg_flag);

//send sumber of nods in graph
    send_data(sock,mes,N);

//send source node of  graph
    send_data(sock,mes,src);

//send destination node of  graph
    send_data(sock,mes,dest);

//forming message
    formEdges(edges,mes);
    send(sock, mes, strlen(mes),0);

    shutdown(sock,SHUT_WR);
    memset(mes,'\0',BUF_SIZE);
    recv(sock,mes,BUF_SIZE, 0);//receiving result
    cout << mes  << endl;

    memset(mes,'\0',BUF_SIZE);
    recv(sock, mes, BUF_SIZE, 0);
    cout << mes << endl;
    close(sock);


    return 0;
}
