//gcc *.c  -o main -lpthread
//for((i=0;i<10000000;i++));{ ./main;}

#include "log.h"

#ifdef _WIN32
#include <windows.h>
#define pthread_t HANDLE
#define pthread_create(pntid, NULL, thread_func, param) \
	(\
	*pntid = CreateThread(NULL, 0, thread_func, param, 0, NULL),\
	CloseHandle(*pntid),\
	(*pntid == 0) ? -1 : 0\
	);

#define THREAD_FUNC_RETURN_TYPE DWORD WINAPI
#else
#include <stdarg.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#define Sleep(time) usleep(time*1000)
#define THREAD_FUNC_RETURN_TYPE void*
#endif

THREAD_FUNC_RETURN_TYPE ThreadFunc(void* lpParameter)
{
	int i = 0;
	for (; i < 100000000; i++)
	{
		LOG_WRITE(LOG_LEVEL_ERROR, "LOG_WRITE %d\n", LOG_LEVEL_ERROR);
		LOG_WRITE_P(LOG_LEVEL_ERROR, "LOG_WRITE_P %d\n", LOG_LEVEL_ERROR);
		LOG_ALERT("LOG_ALERT\n");
		LOG_ALERT_P("LOG_ALERT_P\n");
		LOG_DEBUG("LOG_DEBUG\n");
		LOG_DEBUG_P("LOG_DEBUG_P\n");
	}
	Sleep(1000000);
	return 0;
}

int main()
{
	int i = 0;
    printf("Hello World!\n"); 
	log_open(&g_log, "", LOG_LEVEL_ERROR, 10, 3000000, 0);
	for (; i < 4; i++)
	{
		pthread_t ntid;
		int err = pthread_create(&ntid, NULL, ThreadFunc, (void*)NULL);
		if (err != 0)
		{
			LOG_ERROR("Failed to create thread\n");
			exit(EXIT_FAILURE);
		}
	}
	Sleep(10000);
	log_close(&g_log);
	Sleep(100);
	return  1;
}
