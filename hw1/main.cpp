//
//  main.cpp
//  HomeWork1
//
//  Created by mdong3 on 1/16/15.
//  Copyright (c) 2015 mdong3. All rights reserved.
//

#include <stdio.h>
#include <fstream>
#include <string.h>
#include <sys/time.h>
using namespace std;

int* readToMemory(FILE *readfile);
int fileLen(FILE *fileN);
void SequntialSearch(int seekLen, int keyLen, FILE *seek, FILE *key, int *seekArray, int *keyArray, bool mem);
void BinarySearch(int seekLen, int keyLen, FILE *seek, FILE *key, int *seekArray, int *keyArray, bool mem);

int main(int argc, char *argv[]) {
    struct timeval startT, endT;
    gettimeofday(&startT, NULL);
    
    FILE *key;
    key = fopen(argv[2], "rb");
    FILE *seek;
    seek =fopen(argv[3], "rb");//fopen("/Users/admin/Documents/xcode/CSC541/HomeWork1/seek.db", "rb");
    int keyLen = fileLen(key);
    int seekLen = fileLen(seek);
    int *keyArray=NULL;
    int *seekArray=NULL;
    string memOrdis;
    bool mem = false;
    int modeLen = string(argv[1]).length();
    string MemOrDis = "";
    string LinOrBin = "";

    int len = 2;
    while(argv[1][len] != '-'){
        MemOrDis += argv[1][len];
        len ++;
    }
    for (int len2 = len+1; len2 < modeLen;len2++) {
        LinOrBin += argv[1][len2];
    }
    if(MemOrDis == "mem")//if it is memory read, then new two arrays

    {
        mem = true;
        keyArray = new int[keyLen];//(int *)malloc(keyLen*sizeof(int));
        fread(keyArray, 4, keyLen, key);
        seekArray = new int[seekLen];//(int *)malloc(seekLen*sizeof(int));
        fread(seekArray, 4, seekLen, seek);
    }
    else if (MemOrDis == "disk"){}
    else{printf("commend format error!\n"); return 0;}
    if(LinOrBin == "lin")
       SequntialSearch(seekLen, keyLen, seek, key, seekArray, keyArray, mem);//do a quential search
    else if(LinOrBin == "bin")
        BinarySearch(seekLen, keyLen, seek, key, seekArray, keyArray, mem);//do a binary search
    else return 0;
    delete [] keyArray;
    delete [] seekArray;
//    free(keyArray);
//    free(seekArray);
    fclose(seek);
    fclose(key);
    gettimeofday(&endT, NULL);
    printf("Time: %lf\n", (endT.tv_sec - startT.tv_sec)+(endT.tv_usec-startT.tv_usec)/1000000.0);//print the processing time
    return 0;
}
int fileLen(FILE *fileN){//get the number of integers in the file
    long lSize;
    if(fileN == NULL){fputs("File error", stderr);
        return -1;}
    fseek(fileN, 0, SEEK_END);
    lSize = ftell(fileN);
    rewind(fileN);
    int arrLen = int(lSize/4);
    return arrLen;
}

int getith(int i, FILE *fileN, int *array, bool mem){//get the ith integer of the file in memory or disk
    if (mem == true) {
        return array[i];
    }
    else{
        fseek(fileN,4*i,SEEK_SET);
        int temp;
        fread(&temp,4,1,fileN);
        return temp;
    }
}

void SequntialSearch(int seekLen, int keyLen, FILE *seek, FILE *key, int *seekArray, int *keyArray, bool mem){
    int *hit = new int[seekLen];
    for(int s = 0; s < seekLen; s++){
       int seekV = getith(s, seek, seekArray, mem);
        bool flag = false;
        for (int k = 0; k < keyLen; k++) {
            int keyV = getith(k, key, keyArray, mem);
            if(seekV == keyV){
                hit[s] = 1;
                flag = true;
                break;
            }
        }
        if (!flag) {
            hit[s] = 0;
        }
    }
    for (int m = 0; m < seekLen; m++) {
        if(hit[m] == 0)
            printf("%12d: No\n", getith(m, seek, seekArray, mem));
        else
            printf("%12d: Yes\n", getith(m, seek, seekArray, mem));
    }
    delete [] hit;
}

void BinarySearch(int seekLen, int keyLen, FILE *seek, FILE *key, int *seekArray, int *keyArray, bool mem){
    int *hit = new int[seekLen];
    for(int s = 0; s < seekLen; s++){
        int seekV = getith(s, seek, seekArray, mem);
        int upbound = keyLen-1;
        int lowbound = 0;
        bool flag = false;
		bool inLoop = true;
        int k;
		if(seekV == getith(0, key, keyArray, mem)){
			hit[s] = 1;
		}
		else {
	        while(upbound-lowbound > 1){
	            k = (upbound-lowbound)/2+lowbound;
	            int keyV = getith(k, key, keyArray, mem);
	            if (seekV == keyV) {
	                hit[s] = 1;
	                flag = true;
	                break;
	            }
	            else if(seekV < keyV){
	                upbound = k;
	            }
	            else{
	                lowbound = k;}
	       	 }
	        if (flag == false) {
	            hit[s] = 0;
	        }	
		}
    }
    for (int m = 0; m < seekLen; m++) {
        if(hit[m] == 0)
            printf("%12d: No\n", getith(m, seek, seekArray, mem));
        else
            printf("%12d: Yes\n", getith(m, seek, seekArray, mem));
    }
    delete [] hit;
}


