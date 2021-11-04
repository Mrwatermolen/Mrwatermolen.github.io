#define ERROR 0;
#define OK 1;
#define Status int;
typedef struct
{
    int vexnum;
    AdjList vex;
    /* data */
} ALGraph;
typedef struct
{
} Stack;
typedef struct ArcNode
{
    int adjvex;
    int info;
    struct ArcNode *nextArc;
} ArcNode;

typedef struct VNode
{
    int data;
    struct ArcNode *firstArc;
} VNode, AdjList[10];

Status TopologicalSort(ALGraph G)
{
    int *indegree = (int *)malloc(sizeof(int) * G.vexnum);
    Stack S;
    int counter = 0;
    FindInDegree(G, indegree);
    InitStack(S);

    for (int i = 0; i < G.vexnum; ++i)
        if (!indegree[i])
            Push(S, i);

    while (!StackEmpty(S))
    {
        int k;
        Pop(S, k);
        printf("%d\t", k);
        ++counter;
        for (ArcNode *p = G.vex[k].firstArc; p; p = p->nextArc)
        {
            int i = p->adjvex;
            if (--indegree[i] == 0)
                Push(S, i);
        }
    }

    if (counter < G.vexnum)
    {
        return ERROR;
    }
    else
    {
        return OK;
    }
}

int visited[];
void DFSTraverse(ALGraph G)
{
    for (int i = 0; i < G.vexnum; ++i)
        visited[i] = 0;
    for (int i = 0; i < G.vexnum; ++i)
    {
        if (!visited[i])
        {
            DFS(G, i);
        }
    }
}

void DFS(ALGraph G, int i)
{
    visited[i] = 1;
    for (int w = FirstNeighbor(G, i); w >= 0; w = NextNeighbor(G, w))
    {
        if (!visited[w])
        {
            DFS(G, w);
        }
    }
    printf(i);
}

int ve[], vl[];

Status CriticalPath()
{
    return ERROR;
}