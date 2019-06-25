#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/un.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <netdb.h>
#include <csignal>
#include <vector>
#include <iterator>
#include <ctime> 
#include "Graph_Sort.h"
#include <list>
#include <string>
#include <climits>

using namespace std;

enum alg_name{BELLMAN,TOPSORT};

const int BUF_SIZE = 1024;
const int QUEUE_SIZE = 5;
const int MAX_CON = 4;


static int cl_counter = 0;

/*
struct sockaddr_in {
    short  sin_family;  
    unsigned short   sin_port;   
    struct in_addr   sin_addr;   
    char   sin_zero[8];  
};
*/


//Bellman-Ford algorithm(no negative circles and NULL-weighted nodes)
void bellman_ford(int N, int src,int dest,const vector<Edge>& edge,char* buf)
{

    vector<int> res(N);

    memset(buf,'\0',strlen(buf));

    for (int i = 0; i<N;++i) res[i]=INT_MAX/2;
    res[src] = 0;
     
    for (int i= 0; i<N;++i)
    {
        for (int j = 0; j<N;++j)
        {
            for (vector<Edge>::const_iterator p = edge.begin(); p != edge.end(); ++p)//searching needed edges
                if (((p->src == edge[j].src) && (p->dest == edge[j].dest)) && (res[edge[j].src] + edge[j].weight < res[edge[j].dest]))  res[edge[j].dest] = res[edge[j].src] + edge[j].weight;
        }
            
    }

    char temp1[N];
    memset(temp1,'\0',strlen(temp1));//temporary buf(for sprintf and strcat)
    strcat(buf,"Minimal distance from ");
    sprintf(temp1,"%d ",src);
    strcat(temp1,"to ");
    strcat(buf,temp1);
    memset(temp1,'\0',strlen(temp1));//temporary buf(for sprintf and strcat)
    sprintf(temp1,"%d ",dest);
    strcat(buf,temp1);
    strcat(buf,"is ");
    memset(temp1,'\0',strlen(temp1));//temporary buf(for sprintf and strcat)

    for (int i=0; i<N;++i)
        {
            if (i == dest)
            {
                if (res[i] == INT_MAX/2) sprintf(temp1,"%s ","unreachable");
                    else  sprintf(temp1,"%d ",res[i]);
                strcat(buf,temp1);
                break;
            }
        } 
}


//Does topological sort above graph without circles and writes either result in buf either message \" Sorting is not possible \"  
bool sort_Top(const Graph &graph, int N, vector<int>& res)
{  
    char temp[3];
    memset(temp,'\0',strlen(temp));
    vector<int> L;//empty list that will contain sorted results

    if (doTopologicalSort(graph, L, N)){
        res = move(L);
        return true;
    } else return false;    
        
}


//Find min path from source node to others in graph and writes result in buf
void minPaths(int src,int dest,int N,const vector<Edge>& edges,vector<int> top_sorted,char* buf)//top_sorted is vector-result of topological sort,buf is resulting string
{
    int i,j;
    memset(buf,'\0',strlen(buf));
    i = j = 0;
    while(src != top_sorted[i])  i++;
    while(dest != top_sorted[j]) j++;
    //now we know that src is top_sorted[i] and dest is top_sorted[j]
    //INT_MIN/2 and INT_MAX/2 are picked not to go beyond digit capacity of int
    vector<int> result(N);
    for (int p = 0;p<i;++p)   result[top_sorted[p]] = INT_MIN/2;//non-reachable nodes
    result[top_sorted[i]] = 0;//node itself
    for (int p = i+1;p<N;++p)   result[top_sorted[p]] = INT_MAX/2;//reachable nodes
    for (int j = i+1;j<N;++j)//way to j node
    {
        for (int k = i;k<j;k++)//from k previous nodes
        {
            for (vector<Edge>::const_iterator p = edges.begin(); p != edges.end();++p)//searching needed edges
            {
                if (((p->src == top_sorted[k]) && (p->dest == top_sorted[j])) && ((p->weight + result[top_sorted[k]]) < (result[top_sorted[j]])))  result[top_sorted[j]] = p->weight + result[top_sorted[k]];
                    
            }
        }
    }

    char temp1[N];
    memset(temp1,'\0',strlen(temp1));//temporary buf(for sprintf and strcat)
    strcat(buf,"Minimal distance from ");
    sprintf(temp1,"%d ",src);
    strcat(temp1,"to ");
    strcat(buf,temp1);
    memset(temp1,'\0',strlen(temp1));//temporary buf(for sprintf and strcat)
    sprintf(temp1,"%d ",dest);
    strcat(buf,temp1);
    strcat(buf,"is ");

    memset(temp1,'\0',strlen(temp1));//temporary buf(for sprintf and strcat)
    for (int p = 0;p<N;++p)
    {
        if (p == dest)
        {
            if ((result[p] == INT_MIN/2) || (result[p] == INT_MAX/2)) sprintf(temp1,"%s ","unreachable");
                else sprintf(temp1,"%d ",result[p]);
            strcat(buf,temp1);
            break;    
        }
    }       
}



//Parses string into  numbers. Responsibility for possible errors lies with client.
void parseStr(const char* mes, vector<Edge>& edges)
{
    Edge temp;
    int i,j;
    i = 0;
    int len = strlen(mes);
    char src[len];
    char dest[len];
    char weight[len];
    for(;;)
    {
        if (mes[i] == '.')  break; 
        memset(src,'\0',len);
        memset(dest,'\0',len);
        memset(weight,'\0',len);
       
        j = 0;
                    
        while (mes[i] != ',')
        {
            src[j] = mes[i];
            i++;
            j++;
        }
        i++;
        src[j] = '\0';

        j = 0;

        while (mes[i] != ',')
        {
            dest[j] = mes[i];
            i++;
            j++;
        } 
        i++;
        dest[j] = '\0';

        j = 0;

        while (mes[i] != ',')
        {
            weight[j] = mes[i];
            i++;
            j++;
        } 
        i++;
        weight[j] = '\0';

        temp.src = atoi(src);
        temp.dest = atoi(dest);
        temp.weight = atoi(weight);
        edges.push_back(temp);
    }          
}


//Function-handler for SIGUSR1
void dec_cnt(int)
{
    cl_counter--;
}



//MAIN
int main(int argc,char** argv)
{
    //log file
    ofstream log;
    log.open("log.txt",ios_base::out | ios_base::app); 


    //activity information
    time_t st_time;
    st_time = time(NULL);
    log << "Server started on " << asctime(localtime(&st_time)) << endl;
   
    signal(SIGUSR1,dec_cnt);//for descresing processes connected to our server

    int pid;
    int N,src,dest;//number of nods,source and destination node in graph

    vector <Edge> edges;
    vector<int> srt_nodes;

    cout << "Local Network server. Version 1.5 beta" << endl << "by Pavel Sivashchenko" << endl << "All rights reserved" << endl;
    cout << "Press \"exit\", \"fin\" or \"term\" to exit" << endl << endl;
    char mes[BUF_SIZE];
    struct sockaddr_in serv_addr,cl_addr;
    int sock = socket(AF_INET,SOCK_STREAM,0);

    memset(&serv_addr, '\0', sizeof(serv_addr));
    memset(&cl_addr, '\0', sizeof(cl_addr));
    memset(mes, '\0',BUF_SIZE);

    if(sock < 0)
    {
        perror("socket");
        exit(1);
    }

    if (argv[1] == NULL)
    {
        cout << "You must input appropriate port" << endl;
        exit(1);
    }
//server's structure
    serv_addr.sin_family = AF_INET;



    serv_addr.sin_port = htons(atoi(argv[1]));//port number
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("bind");
        exit(2);
    }

    if (fork())//main server's terminal process
    {
        for(;;)
        {
            waitpid(-1,NULL,WNOHANG);
            cin >> mes;
            if ((strcmp(mes,"exit") == 0) || (strcmp(mes,"term") == 0) || (strcmp(mes,"fin") == 0))
            {   
                shutdown(sock,2);
                close(sock);/*closing additional socket*/
                cout << "Server is terminating...";
                log << "Server is terminating...";

                st_time = time(NULL);
                log << "Server terminated on " << asctime(localtime(&st_time)) << endl;
                log.close();
                kill(0,SIGKILL);//killing all sons and server itself
                exit(0);
            }else cout << "Wrong command!" << endl;
        }
        
    }

    //main server's process
    for(;;){
            listen(sock,QUEUE_SIZE);
            unsigned int addr_len = sizeof(cl_addr);
            int add_sock = accept(sock,(struct sockaddr*)&cl_addr,&addr_len);
            if(add_sock < 0)
            {
                perror("accept");
                exit(3);
            }
            cl_counter++;
     //creating threads
            if (pid = fork())
            {
                close(add_sock);
                while (waitpid(-1,NULL,WNOHANG) == -1);
            }
            else //clients' trigger
            {
                
                if (cl_counter > MAX_CON)//limit of connectors
                {
                    send(add_sock,"Server is overcrowded!Please,wait for some time.",strlen("Server is overcrowded!Please,wait for some time."),0);
                    kill(pid,SIGUSR1);
                    exit(0);
                }
                cout << "Currently there are(-is) " << cl_counter<< " user(-s) connected" <<  endl;
                log << "Currently there are(-is) " << cl_counter<< " user(-s) connected" <<  endl;
                send(add_sock,"Connection established",strlen("Connection established"),0);
    
                alg_name alg_flag;

    //receiving algorithm id
                recv(add_sock,mes,BUF_SIZE,0);
                alg_flag = static_cast<alg_name>(atoi(mes));
                
                memset(mes, '\0',BUF_SIZE);
                cout << "Connected, ip:" << inet_ntoa(cl_addr.sin_addr) << endl;
                log << "Connected, ip:" << inet_ntoa(cl_addr.sin_addr) << endl; 

    //receiving number of nods
                recv(add_sock,mes,BUF_SIZE,0);
                N = atoi(mes);
                
                memset(mes, '\0',BUF_SIZE);
    //receiving source node
                recv(add_sock,mes,BUF_SIZE,0);
                src = atoi(mes);

                memset(mes, '\0',BUF_SIZE);
    //receiving destination node
                recv(add_sock,mes,BUF_SIZE,0);
                dest = atoi(mes);

                int start = clock(); 
                memset(mes, '\0',BUF_SIZE);
                recv(add_sock, mes, BUF_SIZE, 0);
                log << "From" << ' ' << inet_ntoa(cl_addr.sin_addr) << N << ": " <<  mes << endl;
                parseStr(mes,edges);

                //edges is vector of edges

                cout << "Doing sort for "<< inet_ntoa(cl_addr.sin_addr) << "..." << endl;
                log << "Doing sort for "<< inet_ntoa(cl_addr.sin_addr) << "..." << endl;
                
                Graph graph(edges,N);

    //path-finding
                if (alg_flag == TOPSORT)
                {
                    if (sort_Top(graph,N,srt_nodes) == false)
                    {
                        sprintf(mes,"%s","Graph has circles! Sorting is not possible!");

                    } else minPaths(src,dest,N,edges,srt_nodes,mes);
                } else bellman_ford(N,src,dest,edges,mes);
                
                
                cout << "Done sort for "<< inet_ntoa(cl_addr.sin_addr) <<  endl;

                shutdown(add_sock,SHUT_RD);
                send(add_sock,mes,strlen(mes),0);//sending result
                
                sleep(1);

                send(add_sock,"Disconnected",strlen("Disconnected"),0);
                
                close(add_sock);/*closing additional socket*/
                cout << "Disconnected, ip :" << inet_ntoa(cl_addr.sin_addr) << endl;
                log << "Disconnected, ip :" << inet_ntoa(cl_addr.sin_addr) << endl;

                int finish = clock();

                log << "Time spend for process with ip : " << inet_ntoa(cl_addr.sin_addr) << (finish - start)/ CLOCKS_PER_SEC  << endl;
                kill(pid,SIGUSR1);
                exit(0);
            }  
    }
}