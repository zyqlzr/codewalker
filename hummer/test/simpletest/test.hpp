#ifndef __TEST_H__
#define __TEST_H__

#define MAX_TEST_NUM 100

enum TestResult
{
	TEST_OK = 0,
	TEST_ERR = -1,
	TEST_CRASH = -2
};

typedef int (* TEST_CB)(void* input);

enum TaskExecFlag
{
	SKIP_TASK = 0,
	EXEC_TASK,
	END_TASK
};

enum InstanceExecFlag
{
	INSTANCE_EXEC_PER = 0,
	INSTANCE_EXEC_ALL
};

typedef struct test_task_s
{
    int uExecFlag;
    TEST_CB fCb;
    void* pArg;
    const char* sTaskName;
}test_task_t;

typedef struct test_instance_s
{
	char* name; 
    int iTestNum;
    int iAllExecFlag;
    test_task_t aTestTask[MAX_TEST_NUM];
}test_instance_t;

void RegisterInstance(test_instance_t* instance);
void UnRegisterInstance(test_instance_t* instance);
void Exec();

#endif
