
#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <process.h>
#pragma comment( lib,"winmm.lib" )

#define gettimeofday(ptv, NULL)\
	(*ptv).tv_usec = timeGetTime();\
	(*ptv).tv_sec = (*ptv).tv_usec / 1000;\
	(*ptv).tv_usec %= 1000;\
	(*ptv).tv_usec *= 1000;

#define		pthread_mutex_init(pMutex, attr) !(pMutex = CreateMutexA(NULL, FALSE, NULL))
#define 	pthread_mutex_lock(pMutex) WaitForSingleObject(pMutex, INFINITE);
#define 	pthread_mutex_unlock(pMutex) ReleaseMutex(pMutex)
#define 	pthread_mutex_destroy(pMutex) CloseHandle(pMutex)
#define		pthread_mutex_t void
static HANDLE ghMutex = NULL;
#else

#include <stdarg.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#define _snprintf snprintf
#define Sleep(time) usleep(time*1000)
static pthread_mutex_t s_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t *ghMutex = &s_mutex;
#endif

#include "log.h"

#define MAX_LOG_SEQ 0x400
#define MAX_LOG_LINE_SIZE 0x1000
#define DEL_LOG_FILE_SIZE 20000000

static const char *s_strLogType[] =
{
	"EMERG",
	"ALERT",
	"CRIT",
	"ERROR",
	"WARN",
	"NOTICE",
	"INFO",
	"DEBUG"
};

S_LOG g_log = {0};

//��  ��: ɾ����ʱ��־�ļ�
static void log_del_timeout_file(S_LOG *pLog)
{
	if (pLog->hour == 0)
		return;
#ifdef _WIN32
	{
		//system("del *.log");
		const DWORD nBufferLength = MAX_LOG_PATH_NAME_T;
		WIN32_FIND_DATAA ffd;
		char delFile[MAX_LOG_PATH_NAME_T] = {0};
		HANDLE hFind = NULL;
		FILETIME SystemTimeAsFileTime;
		hFind = FindFirstFileA("*.log",&ffd);
		GetSystemTimeAsFileTime(&SystemTimeAsFileTime);
		while (INVALID_HANDLE_VALUE != hFind && strcmp(ffd.cFileName, delFile))
		{
			strcpy(delFile, ffd.cFileName);
			FindNextFileA(hFind, &ffd);
			//ʱ����10����Ϊ��λ,6����60����,144��24Сʱ
			if (ffd.ftLastWriteTime.dwHighDateTime + pLog->hour * 6 < SystemTimeAsFileTime.dwHighDateTime)
			{
				DeleteFileA(delFile);
			}
		}
		FindClose(hFind);
	}
#else
	{
		char delFile[MAX_LOG_PATH_NAME_T] = {0};
		//ʱ���Է���Ϊ��λ
		sprintf(delFile, "find -mmin +%u -name \"*.log\" -exec rm -f {} \\;", (unsigned int)(pLog->hour * 60));
		system(delFile);
	}
#endif
}

//��  ��: ����һ���µ���־�ļ�
//��  ��: pLogָ������־�ṹ��
//����ֵ: 
static void log_new_file(S_LOG *pLog)
{
	int len = 0;

	log_del_timeout_file(pLog);
	
	if (NULL != pLog->fp && (len = fclose(pLog->fp)))
	{
		printf("fclose failed ! pLog->strPathName=%s.\r\n", pLog->strPathName);
		return;
	}

	if (pLog->count > 1)
	{
		char buf[MAX_LOG_PATH_NAME_T] = { 0 };
		char buf2[MAX_LOG_PATH_NAME_T] = { 0 };
		sprintf(buf, "%s.%u", pLog->strPathName, (unsigned int)(pLog->count - 1));
		remove(buf);
		for (len = pLog->count - 2; len > 0; len--)
		{
			sprintf(buf, "%s.%u", pLog->strPathName, len);
			sprintf(buf2, "%s.%u", pLog->strPathName, len + 1);
			rename(buf, buf2);
		}
		rename(pLog->strPathName, buf);
	}

	if(NULL == (pLog->fp = fopen(pLog->strPathName, "wb+")))
	{
		printf("fopen failed ! pLog->strPathName=%s.\r\n", pLog->strPathName);
		return;
	}
	pLog->seq = 0;
	return;
}

//��  ��: �ͷ���־�ļ���Ӧ���ļ�ָ��
//��  ��: pLogָ������־�ṹ��
//����ֵ: 
void log_close(S_LOG *pLog)
{
	if (pLog != NULL && pLog->fp != NULL)
	{
		fclose(pLog->fp);
		pLog->fp = NULL;
	}
	if (NULL != ghMutex)
	{
		pthread_mutex_destroy(ghMutex);
		ghMutex = NULL;
	}
}

//��  ��: д��־��ָ������־�ļ���
//��  ��: pLogָ������־�ṹ��,level������־��Ϣ�ȼ�,����Ĳ�����printf
//����ֵ: 
void log_write(S_LOG *pLog, E_LOG_LEVEL level, const char *msgfmt, ...)
{
	if (level > pLog->level)
	{
		return;
	}
	{
		va_list ap;
		char *pos, message[MAX_LOG_LINE_SIZE+1] = {0};
		size_t sz, len;
		time_t t;
		struct timeval tv;

		time(&t);
		len = strftime(message, MAX_LOG_LINE_SIZE,"%Y-%m-%d %H:%M:%S", localtime(&t));
		gettimeofday(&tv, NULL);
		_snprintf(&message[len], MAX_LOG_LINE_SIZE-len, ".%ld", tv.tv_usec);
		len = strlen(message);
		_snprintf(&message[len], MAX_LOG_LINE_SIZE-len, "[%s]", s_strLogType[level]);
		message[MAX_LOG_LINE_SIZE] = '\0';
		sz = strlen(message);
		pos = (char*)message + sz;
		va_start(ap, msgfmt);
		vsnprintf(pos, MAX_LOG_LINE_SIZE - sz, msgfmt, ap);  
		va_end(ap);

		if (pLog->count != 0)
		{
			pthread_mutex_lock(ghMutex);
			if (++pLog->seq > MAX_LOG_SEQ)
			{
				if ((long)pLog->size < ftell(pLog->fp))
				{
					log_new_file(pLog);
				}
				else
				{
					pLog->seq = 0;
				}
			}
			fprintf(pLog->fp, "%s", (char*)message);
			fflush(pLog->fp);
			pthread_mutex_unlock(ghMutex);
		}
		else
		{
			fprintf(pLog->fp, "%s", (char*)message);
			fflush(pLog->fp);
		}
	}
}

//��  ��: ����Ĭ����־�ļ�����ȼ�������
//��  ��: 	char strPathName[MAX_LOG_PATH_NAME_T];//��־�ļ�·����ǰ׺,Ϊ��ʱ���Զ���ȡ
//			size_t seq;//д��seq����־����һ���ļ��и��ж�
//			size_t hour;//����ʱ��Сʱ��,����ʱ��ᱻɾ��
//			size_t count;//��־�ļ��������,Ϊ0ʱ�������ļ�,Ϊ1ʱ�����ļ������ˢ����д
//			size_t size;//��־�ļ���С���������
//			E_LOG_LEVEL level;//��ӡ��־����
//����ֵ:   0Ϊ�ɹ�
int log_open(S_LOG *pLog, const char *strPathName, E_LOG_LEVEL level, size_t count, size_t size, size_t hour)
{
	int len = 0;
	char szPath[MAX_LOG_PATH_NAME_T] = { 0 };

	if (pLog == NULL)
	{
		pLog = &g_log;
	}
	pLog->size = size;
	if (pLog->size == 0)
		pLog->size = DEL_LOG_FILE_SIZE;
	pLog->hour = hour;
	pLog->count = count;
	pLog->level = level;
	if (level < LOG_LEVEL_EMERG || level > LOG_LEVEL_DEBUG)
	{
		printf("Log level setting error\n");
		return -1;
	}

	if(strPathName != NULL && strPathName[0] != 0)
		strncpy(pLog->strPathName, strPathName, sizeof(pLog->strPathName));
	else
	{
#ifdef _WIN32
		if (!GetModuleFileNameA(NULL, szPath, MAX_LOG_PATH_NAME_T))
		{
			printf("GetModuleFileName failed (%d)\n", GetLastError());
			return -2;
		}
		len = strlen(szPath);
		while (len > 0 && szPath[--len] != '\\')
		{
			if (szPath[len] == '.')
			{
				szPath[len] = '\0';
				break;
			}
		}
#else
		readlink("/proc/self/exe", szPath, MAX_LOG_PATH_NAME_T);
#endif
		strncat(pLog->strPathName, &szPath[0], sizeof(pLog->strPathName) - strlen(pLog->strPathName) - 1);
		strncat(pLog->strPathName, ".log", sizeof(pLog->strPathName) - strlen(pLog->strPathName) - 1);
		pLog->strPathName[sizeof(pLog->strPathName) - 1] = '\0';
	}

	len = pthread_mutex_init(ghMutex, NULL);
	if (len != 0)
	{
		printf("Create mutex error\n");
		return -3;
	}
	pthread_mutex_unlock(ghMutex);

	if (NULL == (pLog->fp = fopen(pLog->strPathName, "ab+")))
	{
		printf("fopen failed ! pLog->strPathName=%s.\r\n", pLog->strPathName);
		return -4;
	}

	return 0;
}

#ifdef __cplusplus
}
#endif


