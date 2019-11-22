#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 1025

unsigned int numOfNodes;

typedef enum
{
    true = 0,
    false = 1
} bool;

typedef enum
{
    typeOneError = 0,
    typeTwoError = 1
} Error;

typedef struct Node
{
    unsigned int data;
    struct Node *parent;
    unsigned int numOfChildren;
    struct Node *childrenArray;
} Node;

Node *nodeArray;

void errorMessage(Error error)
{
    if (error == typeOneError)
    {
        fprintf(stderr, "Usage: TreeAnalyzer <Graph File Path> <First Vertex> <Second Vertex>\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        fprintf(stderr, "Invalid input\n");
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


    if ((arrLength > 1 && arr[0] == '0') || arrLength == 0)    //
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
    newNode -> data = newData;
    newNode -> parent = 0;
    newNode -> childrenArray = NULL;
    newNode -> numOfChildren = 0;
    return newNode;
}


/*void printOne(Node *node)
{
        printf("%d\t%d\t", node->data, node->parent->data, node->numOfChildren);
        for (unsigned int j=0; j<node->numOfChildren; j++)
        {
            printf("%d\t", nodeArray->childrenArray[j].data);
        }
}*/


void parseLine(char line[], unsigned int lineNum)
{
    unsigned long i;
    if (lineNum == 0)
    {
        if (isInteger(line) == true)
        {
            char *p;
            numOfNodes = (int) strtol(line, &p, 10);
            nodeArray = (Node*) malloc(sizeof(Node)*numOfNodes);
            for (i = 0; i < numOfNodes; i++)
            {
                nodeArray[i] = *newNode(i);   //todo not sure about pointers here
            }
        }
        else
        {
            errorMessage(typeTwoError);
        }
    }
    else
    {
        if(strcmp(line, "-\n") == 0 || strcmp(line, "-\r\n") == 0)
        {
            /*nodeArray[lineNum].numOfChildren = 0;
            nodeArray[lineNum].childrenArray = NULL;*/
            return;
        }
        if (!isdigit(line[0]) || line[0] == ' ')
        {
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
                separatedLine[sonsCounter][charIndex] = '\0';
                sonsCounter++;
                charIndex = 0;
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
            errorMessage(typeTwoError);
        }
        Node *sonsArray = (Node*) malloc(sizeof(Node)*sonsCounter);
        for ( i = 0; i < sonsCounter; i++)
        {
            if (isInteger(separatedLine[i]) == false)
            {
                errorMessage(typeTwoError);
            }
            else
            {
                char *p;
                unsigned int data = (int) strtol(separatedLine[i], &p, 10);
                if (data >= numOfNodes)
                {
                    errorMessage(typeTwoError);
                }
                else
                {

                    nodeArray[data].parent = &nodeArray[lineNum -1];      // todo not sure about pointers;
                    sonsArray[i] = nodeArray[data];
                }
            }
        }
        nodeArray[lineNum - 1].childrenArray =  sonsArray;
        nodeArray[lineNum - 1].numOfChildren = sonsCounter;
//        printOne(&nodeArray[lineNum - 1]);
        /*for (i = 0; i < sonsCounter; i++)
        {
            free(&sonsArray[i]);
        }*/
        free(sonsArray);
    }

}


void printAll()
{
    for(unsigned int i=0; i<numOfNodes; i++)
    {
        printf("%d\t%d\t", nodeArray[i].data/*, nodeArray[i].parent->data*/, nodeArray[i].numOfChildren);
        for (unsigned int j=0; j<nodeArray[i].numOfChildren; j++)
        {
            printf("%d ", nodeArray[i].childrenArray[j].data);
        }
        printf("\n");
    }
}




























int main(int argc, char *argv[]) {

    /*if (argc != 4)
    {
        errorMessage(typeOneError);
    }
    DIR *dir = opendir(argv[1]);
    if (!dir *//*ENOENT == errno *//*|| isInteger(argv[2]) != true || isInteger(argv[3]) != true)
    {
        errorMessage(typeTwoError);
    }

    FILE *file;
    file = fopen(argv[1],"r");
    char line[MAX_LINE_LENGTH];
    unsigned int lineNum = 0;
    if (file == NULL){
        errorMessage(typeTwoError);
    }
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL)
    {
        parseLine(line, lineNum);
        lineNum++;
    }
    fclose(file);*/


    if(argc !=1 && argv == NULL){
        return -1;
    }
    unsigned int lineNum =0;
    unsigned int toContinue = 0;
    while (toContinue == 0) {
        char inputLine[1024];
        printf("Enter student info. To exit press q, then enter\n");
        fgets(inputLine, sizeof inputLine, stdin);
        if (strcmp(inputLine, "q\n") == 0 || strcmp(inputLine, "q\r\n") == 0) {
            toContinue = 1;
            continue;
        }
        parseLine(inputLine, lineNum);
        lineNum++;
    }


    printAll();
    /*unsigned int i;
    for (i = 0; i < numOfNodes; i++)
    {
        free(nodeArray);
        nodeArray++;
    }*/

    free(nodeArray);
    return 0;
}