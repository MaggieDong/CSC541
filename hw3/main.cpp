//
//  main.cpp
//  HomeWork3
//
//  Created by tianqing on 3/17/15.
//  Copyright (c) 2015 tianqing. All rights reserved.
//to do list:
//1, in the case of total num is less than 1000 and less than 750 in replacement

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <sstream>
typedef struct {int headP; int countNum; int siz;} run_Info;
using namespace std;

void basic_mergesort(FILE *inputF, int intNum, run_Info *eachR, string outFileN);
void multi_mergesort(FILE *inputF, int intNum, run_Info *eachR, string outFileN);
void replacementSel_mergesort(FILE *inputF, int intNum, run_Info *eachR, string outFileN);
int compare (const void * a, const void * b);
int create_Nruns(FILE *inputF, int intNum, run_Info *eachR);


int main(int argc, const char * argv[]) {
    FILE *inputF;
    long lSize;
    static run_Info x[1000];
    run_Info *eachR = x;
    int intNum;
    inputF = fopen( argv[2]/*"input.bin"*/, "rb" );
    if (inputF==NULL) {fputs ("File error",stderr); exit (1);}

    fseek (inputF , 0 , SEEK_END);
    lSize = ftell (inputF);
    rewind (inputF);
    intNum = int(lSize/4);
    string outFileN(argv[3]);
    struct timeval startT, endT;
    gettimeofday(&startT, NULL);
    if (strcmp(argv[1], "--basic") == 0) {
        basic_mergesort(inputF, intNum, eachR, outFileN);
    }
    else if(strcmp(argv[1], "--multistep") == 0) {
        multi_mergesort(inputF, intNum, eachR, outFileN);
    }
    else if (strcmp(argv[1], "--replacement") == 0) {
        replacementSel_mergesort(inputF, intNum, eachR, outFileN);
    }
    else
        printf("unvalid command!");
    fclose( inputF );
    gettimeofday(&endT, NULL);
    printf("Time: %lf\n", (endT.tv_sec - startT.tv_sec)+(endT.tv_usec-startT.tv_usec)/1000000.0);//print the processing time
    return 0;
}

int compare (const void * a, const void * b)
{
    return ( *(int*)a - *(int*)b );
}

int create_Nruns(FILE *inputF, int intNum, run_Info *eachR)
{
    bool remain = true;
    int reminder = intNum % 1000;
    int runNum;
    int input_buf[ 1000 ]; /* input buffer */
    if (reminder == 0) {
        runNum = intNum/1000;
        remain = false;
    }
    else
        runNum = intNum/1000 + 1;
    
    for (int fileNum = 0; fileNum < runNum - 1; fileNum++) {
        int input_buf[ 1000 ]; /* input buffer */
        fread( input_buf, sizeof( int ), 1000, inputF );
        qsort (input_buf, 1000, sizeof(int), compare);
        FILE *runF;
        string fileName;
		std::ostringstream ss;
		ss << fileNum;
		fileName = "input.bin." + ss.str();

        runF = fopen(fileName.c_str(), "wb");
        fwrite( input_buf, sizeof( int ), 1000, runF );
        fclose( runF );
        eachR[fileNum].headP = 0;
        eachR[fileNum].countNum = 0;
        eachR[fileNum].siz = 1000;
    }
    if (remain) {
        fread( input_buf, sizeof( int ), reminder, inputF );
        qsort (input_buf, reminder, sizeof(int), compare);
        FILE *runF;
        string fileName;
		std::ostringstream ss;
		ss << runNum - 1;
		fileName = "input.bin." + ss.str();
        runF = fopen(fileName.c_str(), "wb");
        fwrite( input_buf, sizeof( int ), reminder, runF );
        fclose( runF );
        eachR[runNum - 1].headP = 0;
        eachR[runNum - 1].countNum = 0;
        eachR[runNum - 1].siz = reminder;
    }
    else {
        fread( input_buf, sizeof( int ), 1000, inputF );
        qsort (input_buf, 1000, sizeof(int), compare);
        FILE *runF;
        string fileName;
		std::ostringstream ss;
		ss << runNum - 1;
		fileName = "input.bin." + ss.str();

        runF = fopen(fileName.c_str(), "wb");
        fwrite( input_buf, sizeof( int ), 1000, runF );
        fclose( runF );
        eachR[runNum - 1].headP = 0;
        eachR[runNum - 1].countNum = 0;
        eachR[runNum - 1].siz = 1000;
    }
    return runNum;
}


void readToBufArr(int fileNum, int groupC, int subrunNum, run_Info *eachR, int** bufArr, int bufLen, string fileNamePre)//////////////----------------------------
{
    FILE *runF;
    string fileName;
	std::ostringstream ss;
	ss << fileNum + groupC * 15;
	fileName = fileNamePre + ss.str();

    runF = fopen(fileName.c_str(), "rb");
    int runRemain = eachR[fileNum + groupC * 15].siz-eachR[fileNum + groupC * 15].countNum;
    if (runRemain == 0) {
        eachR[fileNum + groupC * 15].headP = bufLen;
    }
    else if (runRemain >= bufLen) {
        fseek(runF, sizeof(int)*eachR[fileNum + groupC * 15].countNum, SEEK_SET);
        fread( bufArr[fileNum], sizeof( int ), bufLen, runF );
        eachR[fileNum + groupC * 15].countNum += bufLen;
        eachR[fileNum + groupC * 15].headP = 0;
    }
    else
    {
        fseek(runF, sizeof(int)*eachR[fileNum + groupC * 15].countNum, SEEK_SET);
        fread( bufArr[fileNum], sizeof( int ), runRemain, runF );
        eachR[fileNum + groupC * 15].countNum += runRemain;
        eachR[fileNum + groupC * 15].headP = 0;

    }
    fclose(runF);
}

void diskMerge(int intNum, run_Info *eachR, int subrunNum, /*int runNum,*/ int groupintNum, /*int bufLen, int** bufArr,*/ int groupC, string fileNamePre, string outputFileN)
{
    
    int bufLen = 1000/subrunNum; // subrunNum equals to 15
    if (groupintNum == intNum) {
    }

    int** bufArr = new int*[subrunNum];
    
    for(int i = 0; i < subrunNum; ++i)
    {
        bufArr[i] = new int[bufLen];
        readToBufArr(i, groupC, subrunNum, eachR, bufArr, bufLen, fileNamePre);
    }
    
    int smallNum = 0;
    int smallest;
    int colCount = 0;
    int markCol = 0;
    int output_buf[ 1000 ]; /* Output buffer */
    FILE *superF;
    string fileName;
    if(groupintNum == intNum)
        fileName = outputFileN;
    else {
		std::ostringstream ss;
		ss << groupC;
		fileName = outputFileN + ss.str();
//        fileName = outputFileN/*"input.bin.super."*/+ to_string(groupC);
    }
//	cout << "mark3" << endl;
    superF = fopen( fileName.c_str(), "wb" );///////////////////////////////////////
    while (smallNum < groupintNum) { //groupintNum equals to subgroup's total int number.
        
        while (eachR[colCount + groupC * 15].headP == bufLen && colCount < subrunNum ) 		{/////////////////////bufmark!
            colCount++;
        }		
        smallest = bufArr[colCount][(eachR[colCount + groupC * 15].headP)];
        //        cout <</* "smallest = " <<*/ smallest << endl;
        markCol = colCount;
        colCount++;
        while (colCount < subrunNum) {
            while (eachR[colCount + groupC * 15].headP == bufLen && colCount < subrunNum) {
                colCount++;
            }
            
            if (colCount < subrunNum && smallest > bufArr[colCount][eachR[colCount + groupC * 15].headP]) {
                smallest = bufArr[colCount][eachR[colCount + groupC * 15].headP];
                markCol = colCount;
            }
            colCount++;
        }
        output_buf[smallNum%1000] = smallest;
        eachR[markCol + groupC * 15].headP ++;
        if (eachR[markCol + groupC * 15].headP == bufLen) {
            //            cout << "eachR[markCol].headP == bufLen-----------" << endl;
            readToBufArr(markCol, groupC, subrunNum, eachR, bufArr, bufLen, fileNamePre);
        }
        else if ((eachR[markCol + groupC * 15].headP == eachR[markCol + groupC * 15].siz % bufLen) && (eachR[markCol + groupC * 15].countNum == eachR[markCol + groupC * 15].siz)) {//////////////bufmark!
            eachR[markCol + groupC * 15].headP = bufLen;/////////////////////bufmark!
        }/////////////////////bufmark!
        //        cout <<"headP!!!!!" << eachR[markCol].headP << endl;
        if (smallNum % 1000 == 999 || smallNum == groupintNum-1) {
            //            cout << "smallNum % 999 == 0!" << endl;
            //            fwrite( output_buf, sizeof( int ), 1000, outputF );
            if (smallNum % 1000 == 999) {
                fwrite( output_buf, sizeof( int ), 1000, superF );
            }
            else
                fwrite( output_buf, sizeof( int ), groupintNum%1000, superF );
        }
        colCount = 0;
        smallNum++;
    }
    fclose( superF );
    for(int i = 0; i < subrunNum; ++i)
    {
        delete[] bufArr[i];
    }
    delete [] bufArr;
    
}

void basic_mergesort(FILE *inputF, int intNum, run_Info *eachR, string outFileN)
{
    int runNum;
    runNum = create_Nruns(inputF, intNum, eachR);
    string fileNamePre = "input.bin.";
    string outputFileN = outFileN/*"sort.bin"*/;

    diskMerge(intNum, eachR, runNum, /*int runNum,*/ intNum,/* bufLen, subGbufArr,*/ 0, fileNamePre, outputFileN);
}


void super_run(/*FILE *inputF,*/ int intNum, run_Info *eachR, int subrunNum, int runNum, int groupNum, int rem_groupNum, run_Info *eachSubR/*, int bufLen, int** bufArr*/)//subrunNum equals 15 for request.
{



    int groupintNum;
    string fileNamePre = "input.bin.";
    string outputFileN = "input.bin.super.";
	// if(groupNum == 0) {
	//     diskMerge(intNum, eachR, runNum, /*int runNum,*/ intNum,/* bufLen, subGbufArr,*/ 0, fileNamePre, outputFileN);
	// }
    for (int groupC = 0; groupC < groupNum; groupC++) {
        groupintNum = 1000*subrunNum;//1000*15 equals to subgroup's total int number.
		
        diskMerge(intNum, eachR, subrunNum, /*int runNum,*/ groupintNum, /*bufLen, bufArr,*/ groupC, fileNamePre, outputFileN);
        eachSubR[groupC].headP = 0;
        eachSubR[groupC].countNum = 0;
        eachSubR[groupC].siz= groupintNum;
    }
    if (rem_groupNum != 0) {
        if (intNum % 1000 == 0) {
            groupintNum = rem_groupNum*1000;
        }
        else
            groupintNum = (rem_groupNum-1)*1000+intNum%1000;
//        cout << "remain!!!!!-------------------"<< endl;
        diskMerge(intNum, eachR, rem_groupNum, /*int runNum,*/ groupintNum, /*bufLen, bufArr,*/ groupNum, fileNamePre, outputFileN);
        eachSubR[groupNum].headP = 0;
        eachSubR[groupNum].countNum = 0;
        eachSubR[groupNum].siz= groupintNum;
        
    }
}

void multi_mergesort(FILE *inputF, int intNum, run_Info *eachR, string outFileN)
{
    string fileNamePre = "input.bin.";
    int runNum;
    int subrunNum = 15;
    runNum = create_Nruns(inputF, intNum, eachR);
    int groupNum = runNum / subrunNum;//equals to 16 and 16*15=240
    int rem_groupNum = runNum % subrunNum;//equals to 10
    int groupTNum = groupNum; //total number of subgroup runs
    if (rem_groupNum !=0) {
        groupTNum = groupNum+1;
    }
//	cout <<rem_groupNum <<  " groupTNum: " << groupTNum << endl;
	if (groupTNum == 1) {
	    string outputFileN = outFileN/*"output.bin"*/;
	    diskMerge(intNum, eachR, runNum, /*int runNum,*/ intNum,/* bufLen, subGbufArr,*/ 0, fileNamePre, outputFileN);
	}
	else {
    run_Info *eachSubR = new run_Info[groupTNum];// subgroup run's info
//	cout << "mark1--" << endl;

    super_run(intNum, eachR, subrunNum, runNum, groupNum, rem_groupNum, eachSubR/*, bufLen, bufArr*/);//generate super runs
//	cout << "mark1" << endl;
    fileNamePre = "input.bin.super.";
    string outputFileN = outFileN/*"output.bin"*/;
//	cout << intNum << "aaaaaaa" << groupTNum << endl;
    diskMerge(intNum, eachSubR, groupTNum, /*int runNum,*/ intNum,/* bufLen, subGbufArr,*/ 0, fileNamePre, outputFileN);
//	cout << "mark2" << endl;
    delete [] eachSubR;
	}
}


////////////////////////////////
void siftDown( int *heap_buf, int k, int N);
void heapsort( int heap_buf[], int N)
{
    /* heapify */
    for (int k = N/2; k >= 0; k--)
    {
        siftDown( heap_buf, k, N);
    }
    while (N-1 > 0)
    { /* swap the root(maximum value) of the heap with the last element of the heap */
        //        swap(a[N-1], a[0]); /* put the heap back in max-heap order */
        int temp = heap_buf[N-1];
        heap_buf[N-1] = heap_buf[0];
        heap_buf[0] = temp;
        siftDown(heap_buf, 0, N-1); /* N-- : decrease the size of the heap by one so that the previous max value will stay in its proper placement */
        N--;
    }
}
void siftDown( int *heap_buf, int k, int N){
    while ( k*2 + 1 < N )
    { /* For zero-based arrays, the children are 2*i+1 and 2*i+2 */
        int child = 2*k + 1; /* get bigger child if there are two children */
        if ((child + 1 < N) && (heap_buf[child] < heap_buf[child+1]))
            child++;
        if (heap_buf[k] < heap_buf[child])
        { /* out of max-heap order */
            //            swap( a[child], a[k] );
            int temp = heap_buf[child];
            heap_buf[child] = heap_buf[k];
            heap_buf[k] = temp;
            /* repeat to continue sifting down the child now */
            k = child;
        }
        else
            return;
    }
}
int readToInputbuf(int& bufNumC, int bufNumTot, bool hasRemain, int *input_buf, int bufRem, FILE *inputF)
{
    int bufSize = 0;
    if (bufNumC == bufNumTot - 1) {
        if(hasRemain) {
            fread(input_buf, sizeof(int), bufRem, inputF);
            bufSize = bufRem;
        }
        else {
            fread(input_buf, sizeof(int), 250, inputF);
//            cout << "input_buf: " << input_buf[249] << endl;
            bufSize = 250;
        }
        bufNumC++;
    }

    else if (bufNumC < bufNumTot-1) {
        fread(input_buf, sizeof(int), 250, inputF);
        bufNumC++;
        bufSize = 250;
    }
    return bufSize;
}

void replacementSel_mergesort(FILE *inputF, int intNum, run_Info *eachR, string outFileN)
{
    int output_buf[ 1000 ]; /* Output buffer */
    //////////////////////////////////////////////////////////////////////haven't test!
    if(intNum <= 750) {
        int heap_buf[ 750 ]; /* Input buffer */
        fread( heap_buf, sizeof( int ), intNum, inputF );
        heapsort(heap_buf, intNum);
        FILE *outputF;
        outputF = fopen(outFileN.c_str()/*"output.bin"*/, "wb");
        fwrite( heap_buf, sizeof( int ), intNum, outputF );
        fclose(outputF);
        ////////.........continue...........
    }
    //////////////////////////////////////////////////////////////////////
    else {
        int bufIntNum = intNum - 750;
        int bufNum = bufIntNum / 250;
        int bufRem = bufIntNum % 250;
        int bufNumTot = bufNum;
        bool hasRemain = false;
        if (bufRem > 0) {
            hasRemain = true;
            bufNumTot = bufNum + 1;
        }
        int input_buf[250];/* Input buffer */
        int heap_buf[ 750 ];/* heap buffer */
        fread( heap_buf, sizeof( int ), 750, inputF );

        int bufNumC = 0;
        int heapSizeC = 750;//heap size count;
        int intNumC = 0;//count the output number;
        int inputBufP = 0;//input buf head point, when it equals to bufSize, load input buf;
        int bufSize = 0;
        int runNumC = 0;
        int runIntNum = 0;
        FILE *runF;
        string runFileName;
        bufSize = readToInputbuf(bufNumC, bufNumTot, hasRemain, input_buf, bufRem, inputF);
		std::ostringstream ss;
		ss << runNumC;
		runFileName = "input.bin." + ss.str();
//		runFileName = "input.bin."+ to_string(runNumC);
        runF = fopen( runFileName.c_str(), "wb" );
        
        heapsort(heap_buf, heapSizeC);

            while (/*bufNumC < 25*/bufSize > 0) {
                while(inputBufP < bufSize) {
                    heapsort(heap_buf, heapSizeC);
                    if (heap_buf[0] <= input_buf[inputBufP]) {
                        output_buf[runIntNum%1000] = heap_buf[0];
                        heap_buf[0] = input_buf[inputBufP];
                    }
                    else {
                        output_buf[runIntNum%1000] = heap_buf[0];
                        heapSizeC --;////////////problem
                        if (heapSizeC > 0) {
                            heap_buf[0] = heap_buf[heapSizeC];
                        }
                        heap_buf[heapSizeC] = input_buf[inputBufP];
                    }
//                    cout << "run1 is: " << output_buf[intNumC%1000] << endl;
                    if (runIntNum % 1000 == 999 || heapSizeC == 0) {
                        ///////////////////////////////////////
                        //                runFileName = "input.bin."+ to_string(runNumC);
                        //                runF = fopen( runFileName.c_str(), "wb" );
                        if (runIntNum % 1000 == 999) {
                            fwrite( output_buf, sizeof( int ), 1000, runF );
//                            cout << "1000 in buf!" << endl;
                            if (heapSizeC == 0) {
    //                            cout << "run over a!" << endl;
                                fclose(runF);
                                eachR[runNumC].headP = 0;
                                eachR[runNumC].countNum = 0;
                                eachR[runNumC].siz = runIntNum + 1;
                                runIntNum = -1;
                                
                                runNumC++;
//                                cout << intNumC << " mark! runNumC = " << runNumC << endl;
								std::ostringstream ss;
								ss << runNumC;
								runFileName = "input.bin." + ss.str();
//								runFileName = "input.bin."+ to_string(runNumC);
                                runF = fopen( runFileName.c_str(), "wb" );
                                heapSizeC = 750;/////////////////////////////////////////////////////////problem
                            }
                        }
                        else {
                            fwrite( output_buf, sizeof( int ), (runIntNum+1)%1000, runF );
    //                        cout << "run over b!" << endl;
                            fclose(runF);
                            eachR[runNumC].headP = 0;
                            eachR[runNumC].countNum = 0;
                            eachR[runNumC].siz = runIntNum + 1;
                            runIntNum = -1;
                            
                            runNumC++;
//                            cout << intNumC << " runNumC = " << runNumC << endl;
							std::ostringstream ss;
							ss << runNumC;
							runFileName = "input.bin." + ss.str();
					//		runFileName = "input.bin."+ to_string(runNumC);
                            runF = fopen( runFileName.c_str(), "wb" );
                            heapSizeC = 750;/////////////////////////////////////////////////////////problem
                        }
                    }
                    
                    inputBufP++;
                    intNumC++;
                    runIntNum++;
//                cout << "inputBufP = " << inputBufP << " bufSize = " << bufSize << endl;
            }
                if (inputBufP == bufSize) {
//                    cout << "inputBufP = " << inputBufP << " bufSize = " << bufSize << " in read!"<< endl;
                    bufSize = readToInputbuf(bufNumC, bufNumTot, hasRemain, input_buf, bufRem, inputF);
//                   cout << "bufNumC = " << bufNumC << endl;
                    inputBufP = 0;
                }
//                heapsort(heap_buf, heapSizeC);
            }
//            else {
//                cout << "heapSize = " << heapSizeC << endl;
                heapsort(heap_buf, heapSizeC);
                for (int lastRem = 0; lastRem < heapSizeC; lastRem++) {
//                    cout << "lastRem = " << heap_buf[lastRem] << " runIntNum = " << runIntNum << endl;
                    output_buf[runIntNum%1000] = heap_buf[lastRem];
                        if (runIntNum % 1000 == 999)
                            fwrite( output_buf, sizeof( int ), 1000, runF );
                        //                fclose(runF);
                    intNumC++;
                    runIntNum++;
                }
                fwrite(output_buf, sizeof(int), runIntNum % 1000, runF);
                fclose(runF);
                eachR[runNumC].headP = 0;
                eachR[runNumC].countNum = 0;
                eachR[runNumC].siz = runIntNum;
                runIntNum = 0;
                
                if (heapSizeC < 750) {
                    int newRemNum = 750-heapSizeC;
                    for (int newRem = heapSizeC; newRem < 750; newRem++) {
//                        cout << "after size: " << heap_buf[newRem] << endl;
                        heap_buf[newRem - heapSizeC] = heap_buf[newRem];
//                        intNumC++;
//                        cout << "intNumC = " << intNumC << endl;
                    }
                    heapsort(heap_buf, newRemNum);
                    runNumC++;
					std::ostringstream ss;
					ss << runNumC;
					runFileName = "input.bin." + ss.str();
                    runF = fopen( runFileName.c_str(), "wb" );////////////////continue
                    fwrite( heap_buf, sizeof( int ), newRemNum, runF );
                    fclose(runF);
                    eachR[runNumC].headP = 0;
                    eachR[runNumC].countNum = 0;
                    eachR[runNumC].siz = newRemNum;
                    runIntNum = 0;
                }
//            }
        
        string fileNamePre = "input.bin.";
        string outputFileN = outFileN/*"output.bin"*/;
        diskMerge(intNum, eachR, runNumC+1, /*int runNum,*/ intNum,/* bufLen, subGbufArr,*/ 0, fileNamePre, outputFileN);

    }
}