//
//  main.cpp
//  hw4
//
//  Created by tianqing on 4/5/15.
//  Copyright (c) 2015 tianqing. All rights reserved.
/////////////////////
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
//#include <queue>
typedef struct { /* B-tree node */
    int n; /* Number of keys in node */
    int *key; /* Node's keys */
    long *child; /* Node's child subtree offsets */
} btree_node;

struct queue_node{
	long data;
	queue_node *next;
};

using namespace std;

class myQueue
{
	struct queue_node * front;
	struct queue_node * behind;
public:
	myQueue() {
		front = behind = NULL;
	}
	void push(long offset);
	long pop();
	bool empty();
};
void myQueue::push(long offset) {
	struct queue_node *newNode;
	newNode = new queue_node;
	newNode->data = offset;
	newNode->next = NULL;
	if(front == NULL)
		front = newNode;
	else
		behind->next = newNode;
	behind = newNode;
}
long myQueue::pop() {
	if(front == NULL)
		return -1;
	long top = front->data;
	front = front->next;
	return top;
}
bool myQueue::empty() {
	return front == NULL;
}
void add(int value, FILE *indexF, long &root, int order, long &allOffC, bool& firstLine );
bool CreateNewNode(int value, FILE *indexF, int order, long nodeOff, int &parentValue, long &childOff, long &allOffC, bool& firstLine);
bool searchNode(int value, long nodeOff, int &position, FILE* indexF, int &childP);
int readKeyVal(int pos, FILE* indexF, long nodeOff);
long readChildOff(int pos, FILE* indexF, long nodeOff, int order);
int readCountNum(FILE *indexF, long nodeOff);
void writeKeyVal(int pos, FILE* indexF, long nodeOff, int KeyVal);
void writeChildOff(int pos, FILE* indexF, long nodeOff, int order, long ChildOff);
void writeCountNum(FILE *indexF, long nodeOff, int countN);
void writeNewNode(FILE* indexF, long newNodeOff, int order, long& allOffC);
//void readOneNode(FILE* indexF, int order, long nodeOff);
void printOut(long nodeOff, int order, FILE* indexF, bool& firstLine  /*const char* fileName*/);
void search(int value, long root, int& position, FILE* indexF, int order, int& childP);

int main(int argc, const char * argv[]) {
//    cout << "mark " << endl;
    FILE *indexF;
    long root; /* Offset of B-tree root node */
    long allOffC = 0;
    int order;
    order = atoi(argv[2]); /* B-tree order */
    indexF = fopen( argv[1]/*"index.bin"*/, "r+b" ); /* If file doesn't exist, set root offset to unknown and create * file, otherwise read the root offset at the front of the file */
    if ( indexF == NULL ) {
//        cout << "no index!" << endl;
        root = -1;
        indexF = fopen( argv[1]/*"index.bin"*/, "w+b" );
        fwrite( &root, sizeof( long ), 1, indexF );
        allOffC = sizeof(long);
		fclose(indexF);/*closeee*/
		indexF = fopen( argv[1]/*"index.bin"*/, "r+b" );
    }
    else {
        fseek(indexF, 0, SEEK_END);
        allOffC = ftell(indexF);
//        cout << "allOffC: " << allOffC<< endl;
        rewind(indexF);
//        fseek( indexF, 0, SEEK_SET );
        fread(&root, sizeof(long), 1, indexF);
//	fclose(indexF);/*closeee*/
//        cout << "root is: " << root << endl;
    }
	string inputLine;
    char mystring[1024];
    fgets(mystring, 1024, stdin);
    string inputLin(mystring);
    inputLine = inputLin.substr(0,inputLin.size()-1);
//    getline(inputF, inputLine);
	bool firstLine = true;
    while(inputLine != "end"){
//        cout << inputLine << endl;
        istringstream eachW(inputLine);
        string sub;
        eachW >> sub;
        if(sub == "add"){
//			            cout << "add" << endl;
            eachW >> sub;
            int value = atoi(const_cast<char*>(sub.c_str()));
//            cout << "V is: " << value << endl;
            add(value, indexF, root, order, allOffC, firstLine);
        }
        else if(sub == "find"){
			if(!firstLine)
				printf("\n");
            eachW >> sub;
            int value = atoi(const_cast<char*>(sub.c_str()));
            int pos = 0;
            int childP = 0;
            search(value, root, pos, indexF, order, childP);
        }
        else if(sub == "print"){
            printOut(root, order, indexF, firstLine /*argv[1]*/);
        }
        else{
            break;}
        char mystring[1024];
        fgets(mystring, 1024, stdin);
        string inputLin(mystring);
        inputLine = inputLin.substr(0,inputLin.size()-1);
    }
    fseek( indexF, 0, SEEK_SET );
    fwrite(&root, sizeof(long), 1, indexF);
	fclose(indexF);
    return 0;
}
int readKeyVal(int pos, FILE* indexF, long nodeOff)
{
    int KeyVal;
    fseek( indexF, nodeOff+(pos+1)*sizeof(int), SEEK_SET );
    fread(&KeyVal, sizeof(int), 1, indexF);
    return KeyVal;
}
long readChildOff(int pos, FILE* indexF, long nodeOff, int order)
{
//	indexF = fopen( "index.bin", "r+b" );
    long ChildOff;
    fseek( indexF, nodeOff+(order)*sizeof(int)+pos*sizeof(long), SEEK_SET );
    fread(&ChildOff, sizeof(long), 1, indexF);
//	fclose(indexF);
    return ChildOff;
}
int readCountNum(FILE *indexF, long nodeOff)
{
//	indexF = fopen( "index.bin", "r+b" );
    int countN;
    fseek( indexF, nodeOff, SEEK_SET );
    fread(&countN, sizeof(int), 1, indexF);
//	fclose(indexF);
    return countN;
}
void writeKeyVal(int pos, FILE* indexF, long nodeOff, int KeyVal)
{
//	indexF = fopen( "index.bin", "r+b" );
    fseek( indexF, nodeOff+(pos+1)*sizeof(int), SEEK_SET );
    fwrite(&KeyVal, sizeof(int), 1, indexF);
//	fclose(indexF);
}
void writeChildOff(int pos, FILE* indexF, long nodeOff, int order, long ChildOff)
{
//	indexF = fopen( "index.bin", "r+b" );
    fseek( indexF, nodeOff+(order)*sizeof(int)+pos*sizeof(long), SEEK_SET );
    fwrite(&ChildOff, sizeof(long), 1, indexF);
//	fclose(indexF);
}
void writeCountNum(FILE *indexF, long nodeOff, int countN)
{
//	indexF = fopen( "index.bin", "r+b" );
    fseek( indexF, nodeOff, SEEK_SET );
    fwrite(&countN, sizeof(int), 1, indexF);
//	fclose(indexF);
}
void writeNewNode(FILE* indexF, long newNodeOff, int order, long &allOffC)
{
//	indexF = fopen( "index.bin", "r+b" );
    btree_node EachNode; /* Single B-tree node */
    EachNode.n = 0;
    EachNode.key = (int *) calloc( order - 1, sizeof( int ) );//allocate each node's keyvalue
    EachNode.child = (long *) calloc( order, sizeof( long ) );//allocate each node's child offset
    fseek( indexF, newNodeOff, SEEK_SET );
    fwrite( &EachNode.n, sizeof( int ), 1, indexF );
    fwrite( EachNode.key, sizeof( int ), order - 1, indexF );
    for(int i = 0; i < order; i++) {
        long EachNodeChild = EachNode.child[i]-1;
        fwrite( &EachNodeChild, sizeof( long ), 1, indexF );
    }
    allOffC = newNodeOff + (sizeof(int)+sizeof(long))*order;
//	fclose(indexF);
}

void add(int value, FILE *indexF, long &root, int order, long& allOffC, bool& firstLine)
{
    int val;
    long childN;
    bool createNewNode;
    createNewNode = CreateNewNode(value, indexF, order, root, val, childN, allOffC, firstLine);
    long newNodeOff = allOffC;
    if (createNewNode) {
        writeNewNode(indexF, newNodeOff, order, allOffC);//create one new node in disk
        writeCountNum(indexF, newNodeOff, 1);
        writeKeyVal(0, indexF, newNodeOff, val);
        writeChildOff(0, indexF, newNodeOff, order, root);
        writeChildOff(1, indexF, newNodeOff, order, childN);
        root = newNodeOff;
    }
}

bool searchNode(int value, long nodeOff, int &position, FILE* indexF, int &childP)
{
//    cout << readKeyVal(0, indexF, nodeOff) << endl;
    if (value < readKeyVal(0, indexF, nodeOff)) {//value[1] is [0] in my case
        position = -1;
        childP = 0;
        return false;
    }
    else {
        position = readCountNum(indexF, nodeOff);
        position--;
        while (position >= 0) {
            if (value > readKeyVal(position, indexF, nodeOff)) {
                childP = position+1;
                return false;
            }
            else if (value < readKeyVal(position, indexF, nodeOff)) {
                position--;
            }
            else {
                childP = position;
                return true;
            }

        }
        return false;
    }
}
void search(int value, long root, int& position, FILE* indexF, int order, int& childP)
{
    if (root == -1) {
        printf("Entry with key=%d does not exist", value);
//        cout << "Entry with key=" << value << " does not exists" << endl;
//        return -1;
    }
    else {
        if (searchNode(value, root, position, indexF, childP))
            printf("Entry with key=%d exists", value);
//            cout << "Entry with key=" << value << " exists" << endl;
//            return root;
        else {
            search(value, readChildOff(childP, indexF, root, order), position, indexF, order, childP);
        }
    }
}

void AddToNode(int value, long childOff, long nodeOff, int position, FILE* indexF, int order)
{
    int countNum = readCountNum(indexF, nodeOff);
    for(int i = countNum-1; i > position; i--) {
        int temVal = readKeyVal(i, indexF, nodeOff);
        writeKeyVal(i+1, indexF, nodeOff, temVal);
        long childOff = readChildOff(i+1, indexF, nodeOff, order);
        writeChildOff(i+2, indexF, nodeOff, order, childOff);
    }
    writeKeyVal(position+1, indexF, nodeOff, value);
    writeChildOff(position+2, indexF, nodeOff, order, childOff);
    writeCountNum(indexF, nodeOff, countNum+1);
}

void split(int value, long& childOff, long nodeOff, int position, int& parentValue, int order, long &allOffC, FILE* indexF)
{
    int mid, min;
    min = order/2;
    if(position < min)
        mid = min;
    else
        mid = min+1;
    long newChildN = allOffC;
    writeNewNode(indexF, newChildN, order, allOffC);
    for (int i = mid; i < order-1; i++) {
        int iValue = readKeyVal(i, indexF, nodeOff);
        writeKeyVal(i-mid, indexF, newChildN, iValue);
        long iChildOff = readChildOff(i+1, indexF, nodeOff, order);
        writeChildOff(i-mid+1, indexF, newChildN, order, iChildOff);
        writeChildOff(i+1, indexF, nodeOff, order, -1);//the original node splitted right part childoffset is -1;
    }
    writeCountNum(indexF, newChildN, order - 1 - mid);
    writeCountNum(indexF, nodeOff, mid);
    if (position < min)
        AddToNode(value, childOff, nodeOff, position, indexF, order);
    else {
        AddToNode(value, childOff, newChildN, position-mid, indexF, order);
    }
    childOff = newChildN;
    
    int countN = readCountNum(indexF, nodeOff);
    parentValue = readKeyVal(countN - 1, indexF, nodeOff);/////////////countN - 1
    long newChildOff = readChildOff(countN, indexF, nodeOff, order);/////////////countN - 1
    writeChildOff(0, indexF, newChildN, order, newChildOff);
    writeCountNum(indexF, nodeOff, countN - 1);
}
bool CreateNewNode(int value, FILE *indexF, int order, long nodeOff, int &parentValue, long &childOff, long &allOffC, bool& firstLine)//setval
{
    int position;
    int childP;
    if (nodeOff == -1) {
        parentValue = value;
        childOff = -1;
        return true;
    }
    else {
        if(searchNode(value, nodeOff, position, indexF, childP)) {
			if(!firstLine)
				printf("\n");
			else
				firstLine = false;
            printf("Entry with key=%d already exists", value);
		}
        else {///////////////////////////////////
            long childKoffVal = readChildOff(childP, indexF, nodeOff, order);
            if( CreateNewNode(value, indexF, order, childKoffVal, parentValue, childOff, allOffC, firstLine)) {

                int NodeKeyNum = readCountNum(indexF, nodeOff);
                if (NodeKeyNum < order-1) {
                    AddToNode(parentValue, childOff, nodeOff, position, indexF, order);//////////////////////////////////
                    return false;
                }
                else {
                    split(parentValue, childOff, nodeOff, position, parentValue, order, allOffC, indexF);
                    return true;
                }
            }
        }
        return false;
    }
}

void readNode(FILE* indexF, int order, long nodeOff, btree_node &EachNode)
{

    fseek( indexF, nodeOff, SEEK_SET );
    fread( &EachNode.n, sizeof( int ), 1, indexF );
    EachNode.key = (int *) calloc( EachNode.n, sizeof( int ) );//allocate each node's keyvalue
    EachNode.child = (long *) calloc( EachNode.n+1, sizeof( long ) );//allocate each node's child offset

    fread( EachNode.key, sizeof( int ), EachNode.n, indexF );
    fseek( indexF, nodeOff+sizeof(int)*order, SEEK_SET );
    fread( EachNode.child, sizeof( long ), EachNode.n+1, indexF );
}
void printOut(long root, int order, FILE* indexF, bool& firstLine /*const char* fileName*/)
{
	if(!firstLine)
		printf("\n");
	firstLine = false;
    long nodeOff;
//    queue<long> offQue;
	myQueue offQue;
    offQue.push(root);
    int rowCount = 1;
    while(!offQue.empty()) {
		myQueue offQue2;
        printf(" %d: ", rowCount);
        while(!offQue.empty()) {
//            nodeOff = offQue.front();
            nodeOff = offQue.pop();
            btree_node EachNode; /* Single B-tree node */
            readNode(indexF, order, nodeOff, EachNode);
            
            for(int i = 0; i < EachNode.n-1; i++) {
                printf("%d,", EachNode.key[i]);
                if(EachNode.child[i] > -1)
                    offQue2.push(EachNode.child[i]);
            }
            printf("%d ", EachNode.key[EachNode.n-1]);
//            cout << EachNode.key[EachNode.n-1] << " ";
            if(EachNode.child[EachNode.n-1] > -1)
                offQue2.push(EachNode.child[EachNode.n-1]);
            if(EachNode.child[EachNode.n] > -1)
                offQue2.push(EachNode.child[EachNode.n]);
        }
//        offQue = offQue2;
		while(!offQue2.empty()) {
			// offQue.push(offQue2.front());
			// offQue2.pop();
			offQue.push(offQue2.pop());
		}
        rowCount++;
		if(!offQue.empty())
        	printf("\n");
	}	
}
