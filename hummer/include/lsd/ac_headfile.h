#ifndef __ACHEADFILE__
#define __ACHEADFILE__

#include <iostream>
#include <math.h>
using namespace std;

#define JUMPDIS 1
#define IMG_MAX_LEN 256

#define TENSORLEN 3

#define BLOCKSIZE 		16
#define TENSORMAXEDGELEN 		16	//128*128矩阵，每个图像块2*2大小
 #define SHOWRESULTNUM 100
//#define testing
class OutputFile
{
public:
	char * output;	//灰度图文件
	int outputLen;	//灰度图文件大小
	short matrix[IMG_MAX_LEN][IMG_MAX_LEN];	//正排矩阵
	int edgelCount;							//matrix的点个数
	float tensor[TENSORMAXEDGELEN][TENSORMAXEDGELEN][TENSORLEN];	//结构张量
	int tensorNum;	//张量块数
	
public:
	OutputFile()
	{
		output=NULL;
		outputLen=0;
		tensorNum=0;
		edgelCount=0;
		memset(matrix,0,sizeof(short)*IMG_MAX_LEN*IMG_MAX_LEN);
		memset(tensor,0,TENSORMAXEDGELEN*TENSORMAXEDGELEN*TENSORLEN*sizeof(float));
		
	}

	~OutputFile()
	{
		if(output)
			delete output;
		outputLen=0;	
	}

	void clean()
	{
		if(output)
		{
			delete output;
			output = NULL;
		}
		outputLen=0;
		tensorNum=0;
		edgelCount=0;
		memset(matrix,0,sizeof(short)*IMG_MAX_LEN*IMG_MAX_LEN);
		memset(tensor,0,TENSORMAXEDGELEN*TENSORMAXEDGELEN*TENSORLEN*sizeof(float));
	}
};

bool calDescriptor(char * inputImg, int inputLen, OutputFile &outputfile,char *outputFilePath=NULL);

//matrix 是生成的正排文件，matrix[x][y]表示输出结果的（x,y)坐标的角度分类
//0表示无特征点，有效结果为1-N（正数），代表N个分类

int angleClassify(double angle);
//输入参数为0-180的方向

#endif