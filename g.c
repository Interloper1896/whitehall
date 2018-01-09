#include <stdio.h>

typedef struct _edge
{
	int origine;
	int extremite;
} edge;

int node[1000];

edge graph[]=
{
	{1, 200},
	{1, 201},
	{2, 203},
	{2, 205},
	{2, 204},
	{3, 206},
	{3, 207},
	{3, 208},
	{8, 200},
	{8, 209},
	{9, 200},
	{9, 201},
	{10, 210},
	{10, 211},
	{11, 201},
	{11, 211},
	{12, 202},
	{12, 212},
	{13, 212},
	{13, 219},
	{13, 218},
	{13, 213},
	{14, 203},
	{14, 212},
	{14, 213},
	{15, 213},
	{15, 220},
	{16, 204},
	{16, 206},
	{16, 213},
	{17, 206},
	{17, 214},
	{18, 207},
	{18, 214},
	{19, 215},
	{19, 216},
	{28, 209},
	{28, 223},
	{29, 210},
	{29, 217},
	{30, 217},
	{30, 223},
	{31, 217},
	{31, 224},
	{32, 218},
	{32, 225},
	{33, 219},
	{33, 220},
	{33, 225},
	{33, 226},
	{34, 220},
	{34, 221},
	{34, 226},
	{35, 221},
	{35, 227},
	{36, 228},
	{36, 229},
	{37, 215},
	{37, 221},
	{37, 229},
	{37, 222},
	{38, 218},
	{38, 222},
	{39, 222},
	{39, 230},
	{48, 223},
	{48, 231},
	{49, 231},
	{49, 232},
	{50, 236},
	{50, 232},
	{50, 237},
	{51, 224},
	{51, 232},
	{51, 233},
	{51, 238},
	{52, 233},
	{52, 239},
	{52, 240},
	{53, 225},
	{53, 233},
	{53, 234},
	{54, 226},
	{54, 234},
	{55, 234},
	{55, 240},
	{55, 241},
	{56, 229},
	{56, 230},
	{56, 235},
	{57, 230},
	{57, 243},
	{200, 210},
	{200, 1},
	{200, 8},
	{200, 9},
	{201, 1},
	{201, 9},
	{201, 11},
	{201, 202},
	{202, 201},
	{202, 12},
	{202, 203},
	{203, 202},
	{203, 14},
	{203, 2},
	{204, 2},
	{204, 16},
	{205, 2},
	{205, 206},
	{209, 8},
	{209, 210},
	{209, 28},
	{210, 200},
	{210, 10},
	{210, 29},
	{210, 209},
	{211, 11},
	{211, 10},
	{211, 218},
};

int police[300];

void resetNodes()
{
	int i;

	for (i=0;i<1000;i++)
		node[i]=-1;
	for (i=0;i<300;i++)
		police[i]=0;

//	police[203]=1;
}

void algojack(int n, int dist)
{
	int size=(int)sizeof(graph)/(int)sizeof(edge);
	int ext;
	int i;

	node[n]=dist;
	for (i=0;i<size;i++)
		if (graph[i].origine==n)
		{
			ext=graph[i].extremite;
			if ((node[ext]==-1) || (dist<node[ext]))
			{
				if (ext<200)
				{
					if (dist<1)
						algojack(ext,dist+1);
				}
				else
				{
					if (police[ext]==0)
						algojack(ext,dist);
				}
			}
		}
}

void algocop(int n, int dist)
{
	int size=(int)sizeof(graph)/(int)sizeof(edge);
	int ext;
	int i;

	node[n]=dist;
	for (i=0;i<size;i++)
	{
		if (graph[i].origine==n)
		{
			ext=graph[i].extremite;
			if ((node[ext]==-1) || ((dist)<node[ext]))
			{
				if (ext>=200)
				{
					if ((dist)<2)
						algocop(ext,dist+1);
				}
				else
				{
					algocop(ext,dist);
				}
			}
		}
	}
}

void printNode()
{
	int i;

	puts("====================");
	for (i=0;i<1000;i++)
		if (node[i]!=-1)
			printf("%d (%d)\n",i,node[i]);	
}

int main(int argc, char **argv)
{
	int start;

	resetNodes();
	algojack(1,0);
	printNode();

	resetNodes();
	algocop(201,0);
	printNode();

	return 0;
}

