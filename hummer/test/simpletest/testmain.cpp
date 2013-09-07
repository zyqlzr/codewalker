#include "test.hpp"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*
extern test_instance_t pubsub_test;
extern test_instance_t pubsub_testdo;
extern test_instance_t xml_test;
extern test_instance_t filenotify_test;
*/

//extern test_instance_t cluster_test;
extern test_instance_t hbase_client_test;
extern test_instance_t hbase_wrapper_test;
extern test_instance_t time_cached_test;
extern test_instance_t hbase_sketch_test;

int main()
{
	printf("********************simple test start******************\n");
	//RegisterInstance(&pubsub_test);
	//RegisterInstance(&pubsub_testdo);
	//RegisterInstance(&counter_test);
	//RegisterInstance(&xml_test);
	//RegisterInstance(&filenotify_test);
	//RegisterInstance(&compute_test);
	//RegisterInstance(&test_dirsub);
	//RegisterInstance(&cluster_test);
	//RegisterInstance(&hbase_client_test);
	//RegisterInstance(&hbase_wrapper_test);
	//RegisterInstance(&time_cached_test);
	RegisterInstance(&hbase_sketch_test);
	Exec();
	printf("********************simple test end********************\n");
    return 0;
}


