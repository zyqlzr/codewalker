#ifndef __ACHEADFILE__
#define __ACHEADFILE__

#include <iostream>
#include <math.h>
using namespace std;

#define JUMPDIS 1
#define IMG_MAX_LEN 256

#define TENSORLEN 3

#define BLOCKSIZE 		16
#define TENSORMAXEDGELEN 		16	//128*128����ÿ��ͼ���2*2��С
 #define SHOWRESULTNUM 100
//#define testing
class OutputFile
{
public:
	char * output;	//�Ҷ�ͼ�ļ�
	int outputLen;	//�Ҷ�ͼ�ļ���С
	short matrix[IMG_MAX_LEN][IMG_MAX_LEN];	//���ž���
	int edgelCount;							//matrix�ĵ����
	float tensor[TENSORMAXEDGELEN][TENSORMAXEDGELEN][TENSORLEN];	//�ṹ����
	int tensorNum;	//��������
	
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

//matrix �����ɵ������ļ���matrix[x][y]��ʾ�������ģ�x,y)����ĽǶȷ���
//0��ʾ�������㣬��Ч���Ϊ1-N��������������N������

int angleClassify(double angle);
//�������Ϊ0-180�ķ���

#endif