/*

* Adarsh D. Shukla - 201501203
* Karan Master  - 201501446
*/


#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int V,E;
int MST_weight;
// A structure to represent a subset for union-find having parent and rank
struct subset
{
    int parent;
    int rank;
};
// A structure to represent an Edge 
struct Edge
{
    int u;
    int v;
    int weight;
};
struct Edge *edges,*MST;
int find(struct subset component[],int x);
void union_operation(struct subset component[],int x,int y);

//  Function to generate minimum spanning tree
void MST_util()
{
	
    struct subset *component = (struct subset *)malloc(V*sizeof(struct subset));

     // "minEdge" array keeps track of minimum edge eminating 
    // from particular connected component.
    int *minEdge = (int *)malloc(V*sizeof(int)); 
    int i;

    // Initialization
		// Creating V subsets with single elements
    for (i = 0; i< V; i++)
    {
        component[i].parent = i;
        component[i].rank = 0;
        minEdge[i] = -1;
    }

		// Initialyy connected components are V. It keeps on reducing till 1.
    int connectedComponents = V;
    int count=0;

     // Join connected components based on minimum weighted edge between the two 
	// until there is only one left ---> MST.
    while(connectedComponents>1)
    {
          // Traverse through all edges and update
        // minEdge of every component
    	for(i=0;i<E;i++)
    	{
    		int u,v,weight;				// Parameters of an edge
    		u = edges[i].u;
    		v = edges[i].v;
    		weight = edges[i].weight;

    		int rootU = find(component,u);				// Root node of the connected component containing 'u'.
    		int rootV = find(component,v);				// Root node of the connected component containing 'v'.

            // if both the endpoints of edge are in same component then
            // don't update the minEdge edge of that component.
    		if(rootU==rootV)
    			continue;

            // else update the minEdge edge for both component if it
            // has less weight than others. 
    		else
    		{
    			if(minEdge[rootU]==-1 || edges[minEdge[rootU]].weight > edges[i].weight)
    				minEdge[rootU] = i;
    			if(minEdge[rootV]==-1 || edges[minEdge[rootV]].weight > edges[i].weight)
    				minEdge[rootV] = i;
    		}
    	}

        // Traverse through all components and add their minEdge edge to MST.
       for(i=0;i<V;i++)
        {
             // Check if minEdge for current component exists
            if(minEdge[i]!=-1)
            {
                int u = edges[minEdge[i]].u;
                int v = edges[minEdge[i]].v;
                int weight = edges[minEdge[i]].weight;

                int rootU = find(component,u);
                int rootV = find(component,v);
                

                if(rootU==rootV)
                    continue;

                // add edge to MST. 
                MST[count].u = u;
                MST[count].v = v;
                MST[count].weight = weight;
                count++;

               // Update the total weight of MST. And reduce the count for connected components by 1.
                MST_weight+=weight;
                connectedComponents--;
				
				// Do a union of rootU and rootV i.e. join the two connected components if they are disjoint.
                union_operation(component,rootU,rootV);
            }
        }
        // Reset the minEdge edge of each component.
        for(i=0;i<V;i++)
           minEdge[i]=-1;

    }

}

// Function to find the root of a connectde component using path compression technique.
int find(struct subset component[],int x)
{
	if(component[x].parent != x)
      component[x].parent = find(component, component[x].parent);
    return component[x].parent;
}

// Function to find the root of a connectde component using path compression technique.
void union_operation(struct subset component[],int x,int y)
{
	int xroot = find(component,x);
	int yroot = find(component,y);

	if (component[xroot].rank < component[yroot].rank)
        component[xroot].parent = yroot;
    else if (component[xroot].rank > component[yroot].rank)
        component[yroot].parent = xroot;

    else
    {
        component[yroot].parent = xroot;
        component[xroot].rank++;
    }
}

int main(int argc,char *argv[])
{
   /* Input: --> First line number of verices 'V' and number of edges 'E'
               --> Next 'E' lines follows "u v w" where 'u' and 'v' are vertices
               --> of the edge and 'w' is weight of the edge*/
	scanf("%d %d",&V,&E);
    int i;
    edges = (struct Edge *)malloc(E*(sizeof(struct Edge)));
    MST = (struct Edge *)malloc(V*(sizeof(struct Edge)));
    for(i=0;i<E;i++)
    {
        int u,v,weight;
        scanf("%d %d %d",&u,&v,&weight);
        edges[i].u = u;
        edges[i].v = v;
        edges[i].weight = weight;
    }
	
	// Creating MST by calling MST_util().
	double start_time=omp_get_wtime();
	MST_util();
	double stop_time=omp_get_wtime();



    // Print MST
	for(i=0;i<V-1;i++)
	{
		printf("%d %d %d\n",MST[i].u,MST[i].v,MST[i].weight);
	}
	printf("Minimum weight is: %d\n",MST_weight);
	printf("Time taken by serial code is %.8f\n",stop_time - start_time);
}
