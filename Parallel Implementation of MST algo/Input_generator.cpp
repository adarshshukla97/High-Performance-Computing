// redirect output to in1.txt (./a.out > in1.txt)

#include <bits/stdc++.h>
using namespace std;
int MAX=10000;
int main()
{
	int V; //V between 1000 and 20000
	V = 20000;

	unsigned int seed = time(NULL);
	cout<<V<<" "<<V*(V-1)/2<<"\n";
	for(int i=1;i<V;i++)
	{
		for(int j=i+1;j<=V;j++)
			cout<<i<<" "<<j<<" "<<rand_r(&seed)%MAX<<"\n";
	}
}
