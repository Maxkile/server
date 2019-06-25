// no negative circles and null weights
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

const int BUF_SIZE = 1024;
const int QUEUE_SIZE = 5;
const int MAX_CON = 4;


const int Vmax=1000;
const int Emax=Vmax*(Vmax-1)/2;


//алгоритм Беллмана-Форда
void bellman_ford(int N, int src,const vector<Edge>& edge,int e)
{

vector<int> res(N);
int i, j;

for (i=0; i<N; i++) res[i]=INT_MAX/2;
	res[src]=0;
 
for (i=0; i<N; i++)
{
	for (j=0; j<e; j++)
	{
		if (res[edge[j].src]+edge[j].weight<res[edge[j].dest])	res[edge[j].dest]=res[edge[j].src]+edge[j].weight;
	}
		
}

 
for (i=0; i<N; i++)
	{
		if (res[i]==INT_MAX/2)	cout<<endl<<src<<"->"<<i<<"="<<"Not";
			else cout<<endl<<src<<"->"<<i<<"="<<res[i];
	} 
}




int main()
{
int w;
int i, j, n, e, start;
vector<Edge> edge(Emax);

cout<<"Количество вершин > "; cin>>n;
e=0;
for (i=0; i<n; i++)
for (j=0; j<n; j++)
{
cout<<"Вес "<<i<<"->"<<j<<" > "; cin>>w;
if (w!=0)
{
edge[e].src=i;
edge[e].dest=j;
edge[e].weight=w;
e++;
}
}
 
cout<<"Стартовая вершина > "; cin>>start;
cout<<"Список кратчайших путей:";
bellman_ford(n, start,edge,e);
}