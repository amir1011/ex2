#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include "queue.h"

#define MAX_LINE_LENGTH 1025

unsigned int numOfNodes;
unsigned int numOfEdges = 0;

typedef enum
{
    true = 0,
    false = 1
} bool;

typedef enum
{
    typeOneError = 0,
    typeTwoError = 1,
    typeThreeError = 2
} Error;

typedef struct Node
{
    unsigned int data;
    int parent;
    unsigned int numOfChildren;
    int *childrenArray;
    int distance;    // for BFS
} Node;

Node *nodeArray = NULL;

void freeNodes()
{
    if (nodeArray != NULL)
    {
        unsigned int i;
        for (i = 0; i < numOfNodes; i++)
        {
            if (nodeArray[i].childrenArray != NULL)
            {
                free(nodeArray[i].childrenArray);
                nodeArray[i].childrenArray = NULL;
            }
        }
        for (i = 0; i < numOfNodes; i++)
        {
            Node *currNode = nodeArray;
            nodeArray++;
            free(currNode);
        }
        free(nodeArray);
    }
}

void errorMessage(Error error)
{
    if (error == typeOneError)
    {
        fprintf(stderr, "Usage: TreeAnalyzer <Graph File Path> <First Vertex> <Second Vertex>\n");
        exit(EXIT_FAILURE);
    }
    else if (error == typeTwoError)
    {
        fprintf(stderr, "Invalid input\n");
        freeNodes();
        exit(EXIT_FAILURE);
    }
    else
    {
        fprintf(stderr, "Memory's allocating failure\n");
        freeNodes();
        exit(EXIT_FAILURE);
    }
}

int isInteger(char arr[])
{
    unsigned int i ,arrLength;
    char *lineEnding;

    lineEnding = strchr(arr, '\r');
    if(lineEnding)
    {
        lineEnding[0] = 0;
    }
    lineEnding = strchr(arr, '\n');
    if (lineEnding)
    {
        lineEnding[0] = 0;
    }
    arrLength = strlen(arr);

    if ((arrLength > 1 && arr[0] == '0') || arrLength == 0)
    {
        return false;
    }
    for (i = 0; i < strlen(arr); i++)
    {
        if (!isdigit(arr[i]))
        {
            return false;
        }
    }
    return true;
}

Node *newNode(unsigned int newData)
{
    Node *newNode = (Node*) malloc(sizeof(Node));
    if (newNode == NULL)
    {
        errorMessage(typeThreeError);
    }
    newNode -> data = newData;
    newNode -> parent = -1;
    newNode -> childrenArray = NULL;
    newNode -> numOfChildren = 0;
    newNode -> distance = -1;
    return newNode;
}


/*void printOne(Node *node)
{
        printf("%d\t%d\t%d\t", node->data, node->parent, node->numOfChildren);
        for (unsigned int j=0; j<node->numOfChildren; j++)
        {
            printf("%d\t", nodeArray-> childrenArray[j]);
        }
    printf("\n");
}*/



unsigned int keyOfRoot()
{
    unsigned int i;
    for (i = 0; i < numOfNodes; i++)
    {
        if (nodeArray[i].parent == -1)
        {
            return nodeArray[i].data;
        }
    }
}

void parseLine(char line[], unsigned int lineNum, FILE *file)
{
    unsigned long i, j;
    if (lineNum == 0)
    {
        if (isInteger(line) == true)
        {
            char *p;
            numOfNodes = (int) strtol(line, &p, 10);
            nodeArray = (Node*) malloc(sizeof(Node)*numOfNodes);
            if (nodeArray == NULL)
            {
                errorMessage(typeThreeError);
            }
            for (i = 0; i < numOfNodes; i++)
            {
                nodeArray[i] = *newNode(i);   //todo not sure about pointers here
            }
        }
        else
        {
            fclose(file);
            errorMessage(typeTwoError);
        }
    }
    else
    {
        if(strcmp(line, "-\n") == 0 || strcmp(line, "-\r\n") == 0 || strcmp(line, "-") == 0)
        {
            return;
        }
        if (!isdigit(line[0]) || line[0] == ' ')
        {
            fclose(file);
            errorMessage(typeTwoError);
        }
        char separatedLine[513][MAX_LINE_LENGTH];      //todo maybe change from 1024 to 20
        unsigned int sonsCounter = 0;
        unsigned int charIndex = 0;
//        unsigned long i;
        for (i = 0; i < strlen(line); i++)
        {
            if (line[i] == '\0' )
            {
                separatedLine[i][0] = '\0';
                break;
            }
            else if (line[i] == ' ' )
            {
                if (line[i+1] == '\n' || line[i+1] == '\r')
                {
                    continue;
                }
                else
                {
                separatedLine[sonsCounter][charIndex] = '\0';
                sonsCounter++;
                charIndex = 0;
                }
            }
            else
            {
                separatedLine[sonsCounter][charIndex] = line[i];
                charIndex++;
            }
        }
        sonsCounter++;
        if (sonsCounter >= numOfNodes)
        {
            fclose(file);
            errorMessage(typeTwoError);
        }
        nodeArray[lineNum - 1].childrenArray = (int*) malloc(sizeof(int)*sonsCounter);
        if (nodeArray[lineNum - 1].childrenArray == NULL)
        {
            errorMessage(typeThreeError);
        }
        nodeArray[lineNum - 1].numOfChildren = sonsCounter;
        for ( i = 0; i < sonsCounter; i++)
        {
            if (isInteger(separatedLine[i]) == false)
            {
                fclose(file);
                errorMessage(typeTwoError);
            }
            else
            {
                char *p;
                unsigned int nodeData = (int) strtol(separatedLine[i], &p, 10);
                if (nodeData >= numOfNodes)
                {
                    fclose(file);
                    errorMessage(typeTwoError);
                }
                else
                {
                    if (nodeArray[nodeData].parent == -1)
                    {
                    nodeArray[nodeData].parent = nodeArray[lineNum - 1].data;      // todo not sure about pointers;
                    nodeArray[lineNum - 1].childrenArray[i] = nodeArray[nodeData].data;
                    }
                    else
                    {
                        errorMessage(typeTwoError);
                    }
                }
            }
        }
        for(i = 0; i < sonsCounter; i++)
        {
            for (j = i + 1; j < sonsCounter; j++)
            {
                if (nodeArray[lineNum - 1].childrenArray[i] == nodeArray[lineNum - 1].childrenArray[j])
                {
                    errorMessage(typeTwoError);
                }
            }
        }
        numOfEdges += sonsCounter;
    }
//        printOne(&nodeArray[lineNum - 1]);
}

void validateAndParseInput(int numOfArg, char **arguments)
{
    if (numOfArg != 4)
    {
        errorMessage(typeOneError);
    }
    if (isInteger(arguments[2]) == false || isInteger(arguments[3]) == false)
    {
        errorMessage(typeTwoError);
    }
    char *p;
    int unsigned vertex1 = (int) strtol(arguments[2], &p, 10);
    int unsigned vertex2 = (int) strtol(arguments[3], &p, 10);

    /*DIR *dir = opendir(argv[1]);
    if (!dir *//*ENOENT == errno*//* || isInteger(argv[2]) != true || isInteger(argv[3]) != true)
    {
        errorMessage(typeTwoError);
    }*/
    FILE *file;
    file = fopen(arguments[1],"r");
    char line[MAX_LINE_LENGTH];
    unsigned int lineNum = 0;
    if (file == NULL)
    {
        fclose(file);
        errorMessage(typeThreeError);
    }
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL)
    {
        parseLine(line, lineNum, file);
        if (lineNum == 0)
        {
            if (vertex1 >= numOfNodes || vertex2 >= numOfNodes)
            {
                fclose(file);
                errorMessage(typeTwoError);
            }
        }
        lineNum++;
    }
    fclose(file);
}

void BFS(unsigned int startNode, bool isRoot)
{
    Queue *queue = allocQueue();
    if (queue == NULL)
    {
        freeQueue(&queue);
        errorMessage(typeThreeError);
    }
    nodeArray[startNode].distance++;
    enqueue(queue, startNode);
    while (!queueIsEmpty(queue))
    {
        unsigned int i, currChild;
        unsigned int currNode = dequeue(queue);
        for (i = 0; i < nodeArray[currNode].numOfChildren; i++)
        {
            currChild = nodeArray[currNode].childrenArray[i];
            if (nodeArray[currChild].distance == -1)
            {
                nodeArray[currChild].distance = nodeArray[currNode].distance + 1;
                enqueue(queue, currChild);
            }
        }
        if (isRoot == false && nodeArray[currNode].parent != -1)
        {
            currChild = nodeArray[currNode].parent;
            if (nodeArray[currChild].distance == -1)
            {
                nodeArray[currChild].distance = nodeArray[currNode].distance + 1;
                enqueue(queue, currChild);
            }
        }
    }
    freeQueue(&queue);
}

void clearDistance()
{
    for (unsigned int i = 0; i < numOfNodes; i++)
    {
        nodeArray[i].distance = -1;
    }
}

unsigned int findMaxOrMinDistance(bool isMax)
{
    unsigned int i, value, nodeData = 0;
    if (isMax == true)
    {
        value = 0;
        for (i = 0; i < numOfNodes; i++)
        {
            if (nodeArray[i].distance > value) {
                value = nodeArray[i].distance;
                nodeData = nodeArray[i].data;
            }
        }
    }
    else
    {
        value = numOfEdges;
        for (i = 0; i < numOfNodes; i++)
        {
            if (nodeArray[i].distance < value) {
                value = nodeArray[i].distance;
                nodeData = nodeArray[i].data;
            }
        }
    }
    return nodeData;
}

void printMinAndMaxBranchLength()
{
    unsigned int root = keyOfRoot(), maxLength, minLength;
    BFS(root, true);
    maxLength = nodeArray[findMaxOrMinDistance(true)].distance;
    minLength = nodeArray[findMaxOrMinDistance(false)].distance;
    clearDistance();
    printf("%d", maxLength);
    printf("%d", minLength);

}

void printShortestPathBetweenTwoNodes(unsigned int node_1, unsigned int node_2)
{
    // todo to build some method that bond the index of node with of it!!!!!!!!!!!!!!!!!

    if (node_1 == keyOfRoot())
    {
        BFS(node_1, true);
    }
    else
    {
    BFS(node_1, false);
    }
    unsigned int pathLength, i, j, prevNode, prevDistance, prevlocation = 0;
    for (i = 0; i < numOfNodes; i++)
    {
        if (nodeArray[i].data == node_2)
            prevlocation = i;
    }
    pathLength = nodeArray[prevlocation].distance;
    prevNode = node_2;
    prevDistance = nodeArray[node_2].distance;
    if (pathLength > 1)
    {
        unsigned int *path = (unsigned int*) malloc(sizeof(unsigned int)*pathLength);
        if (path == NULL)
        {
            errorMessage(typeThreeError);
        }
        for (i = pathLength; i > 1; i--)
        {
            for (j = 0; j <= nodeArray[prevlocation].numOfChildren; j++)
            {
                if (j == nodeArray[prevlocation].numOfChildren)
                {
                    path[i] = nodeArray[prevlocation].parent;
                    //todo update the prevNode with prevLocation;
                    continue;
                }
                unsigned int childNode = nodeArray[prevlocation].childrenArray[j];
                if (nodeArray[childNode].distance == (prevDistance - 1))
                {
                    prevDistance--;
                    //todo uncorrect updates here;
                    prevNode = nodeArray[childNode].data;
                    path[i] = nodeArray[childNode].data;
                    break;
                }
            }
        }
    }
    else    //todo but what if node1=node2????????????
    {
        clearDistance();
        printf("%d %d", node_1, node_2);
    }


}

void printDiameterLength()
{
    unsigned int i, max = 0, farthestNode = keyOfRoot();
    if (numOfNodes > 1)
    {
        BFS(farthestNode, true);
        farthestNode = findMaxOrMinDistance(true);
        clearDistance();
        BFS(farthestNode, false);
        farthestNode = nodeArray[findMaxOrMinDistance(true)].distance ;
        clearDistance();
        printf("%d", farthestNode);
    }
}

void printAll()
{
    for(unsigned int i=0; i<numOfNodes; i++)
    {
        printf("%d\t%d\t%d\t", nodeArray[i].data, nodeArray[i].parent, nodeArray[i].numOfChildren);
        for (unsigned int j=0; j<nodeArray[i].numOfChildren; j++)
        {
            printf("%d ", nodeArray[i].childrenArray[j]);
        }
        printf("\n");
    }
    printf("%d", numOfEdges);
}


int main(int argc, char *argv[]) {

    validateAndParseInput(argc, argv);


    printAll();
    /*unsigned int i;
    for (i = 0; i < numOfNodes; i++)
    {
        free(nodeArray);
        nodeArray++;
    }*/

    free(nodeArray);
    return 0;






    /*if(argc !=1 && argv == NULL){
        return -1;
    }
    unsigned int lineNum =0;
    unsigned int toContinue = 0;
    int count =-1;
    while (toContinue == 0) {
        char inputLine[1024];
        printf("Enter student info. To exit press q, then enter\n");
        fgets(inputLine, sizeof inputLine, stdin);
        if (strcmp(inputLine, "q\n") == 0 || strcmp(inputLine, "q\r\n") == 0) {
            toContinue = 1;
            continue;
        }
        parseLine(inputLine, lineNum);
        *//*if (count > -1)
            printOne(&nodeArray[count]);*//*
        count++;
        lineNum++;
    }*/

}