#ifndef LOG_MODULE_201505081409
#define LOG_MODULE_201505081409

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#ifndef MAX_LOG_PATH_NAME_T
#define MAX_LOG_PATH_NAME_T 0x400
#endif

//��  ��: д��־,Ĭ�������־�ȼ�ΪLOG_LEVEL_ERROR
//��  ��: level������־��Ϣ�ȼ�,����Ĳ�����printf
//����ֵ: 
#define LOG_WRITE(level, msgfmt, ...) log_write(&g_log, level, msgfmt, ## __VA_ARGS__)
#define LOG_EMERG(fmt, ...)		LOG_WRITE(LOG_LEVEL_EMERG, fmt, ## __VA_ARGS__)
#define LOG_ALERT(fmt, ...)		LOG_WRITE(LOG_LEVEL_ALERT, fmt, ## __VA_ARGS__)
#define LOG_CRIT(fmt, ...)		LOG_WRITE(LOG_LEVEL_CRIT, fmt, ## __VA_ARGS__)
#define LOG_ERROR(fmt, ...)		LOG_WRITE(LOG_LEVEL_ERROR, fmt, ## __VA_ARGS__)
#define LOG_WARN(fmt, ...)		LOG_WRITE(LOG_LEVEL_WARN, fmt, ## __VA_ARGS__)
#define LOG_NOTICE(fmt, ...)	LOG_WRITE(LOG_LEVEL_NOTICE, fmt, ## __VA_ARGS__)
#define LOG_INFO(fmt, ...)		LOG_WRITE(LOG_LEVEL_INFO, fmt, ## __VA_ARGS__)
#define LOG_DEBUG(fmt, ...)		LOG_WRITE(LOG_LEVEL_DEBUG, fmt, ## __VA_ARGS__)

#define LOG_WRITE_P(level, msgfmt, ...) log_write(&g_log, level, "[%s][%s][%d]" msgfmt,  __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__)
#define LOG_EMERG_P(fmt, ...)		LOG_WRITE_P(LOG_LEVEL_EMERG, fmt, ## __VA_ARGS__)
#define LOG_ALERT_P(fmt, ...)		LOG_WRITE_P(LOG_LEVEL_ALERT, fmt, ## __VA_ARGS__)
#define LOG_CRIT_P(fmt, ...)		LOG_WRITE_P(LOG_LEVEL_CRIT, fmt, ## __VA_ARGS__)
#define LOG_ERROR_P(fmt, ...)		LOG_WRITE_P(LOG_LEVEL_ERROR, fmt, ## __VA_ARGS__)
#define LOG_WARN_P(fmt, ...)		LOG_WRITE_P(LOG_LEVEL_WARN, fmt, ## __VA_ARGS__)
#define LOG_NOTICE_P(fmt, ...)		LOG_WRITE_P(LOG_LEVEL_NOTICE, fmt, ## __VA_ARGS__)
#define LOG_INFO_P(fmt, ...)		LOG_WRITE_P(LOG_LEVEL_INFO, fmt, ## __VA_ARGS__)
#define LOG_DEBUG_P(fmt, ...)		LOG_WRITE_P(LOG_LEVEL_DEBUG, fmt, ## __VA_ARGS__)

//LOG_LEVEL_EMERG������״��
//LOG_LEVEL_ALERT�������ȼ����⣬����˵���ݿ�����ȣ�����Ҫ������ȡ��Ӧ�ж�
//LOG_LEVEL_CRIT����Ҫ״������������Ӳ������
//LOG_LEVEL_ERROR��������
//LOG_LEVEL_WARNING�����淢��
//LOG_LEVEL_NOTICE��һ��״������Ҫ����ע��
//LOG_LEVEL_INFO����Ϣ״��
//LOG_LEVEL_DEBUG��������Ϣ

typedef enum {  
	LOG_LEVEL_EMERG,
	LOG_LEVEL_ALERT,
	LOG_LEVEL_CRIT,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_WARN,
	LOG_LEVEL_NOTICE,//Ĭ�������־�ȼ�
	LOG_LEVEL_INFO,
	LOG_LEVEL_DEBUG
}E_LOG_LEVEL;

typedef struct tag_log
{
	char strPathName[MAX_LOG_PATH_NAME_T];//��־�ļ�·����ǰ׺,Ϊ��ʱ���Զ���ȡ
	FILE* fp;//��־�ļ�ָ��
	size_t seq;//д��seq����־����һ���ļ��и��ж�
	size_t hour;//����ʱ��Сʱ��,����ʱ��ᱻɾ��
	size_t count;//��־�ļ��������,Ϊ0ʱ�������ļ�,Ϊ1ʱ�����ļ������ˢ����д
	size_t size;//��־�ļ���С���������
	E_LOG_LEVEL level;//��ӡ��־����
}S_LOG;
extern S_LOG g_log;

//��  ��: ����Ĭ����־�ļ�����ȼ�������
//��  ��: 	char strPathName[MAX_LOG_PATH_NAME_T];//��־�ļ�·����ǰ׺,Ϊ��ʱ���Զ���ȡ
//			size_t hour;//����ʱ��Сʱ��,����ʱ��ᱻɾ��
//			size_t count;//��־�ļ��������,Ϊ0ʱ�������ļ�,Ϊ1ʱ�����ļ������ˢ����д
//			size_t size;//��־�ļ���С���������
//			E_LOG_LEVEL level;//��ӡ��־����
//����ֵ:   0Ϊ�ɹ�
extern int log_open(S_LOG *pLog, const char *strPathName, E_LOG_LEVEL level, size_t count, size_t size, size_t hour);

//��  ��: д��־��ָ������־�ļ���
//��  ��: pLogָ������־�ṹ��,level������־��Ϣ�ȼ�,����Ĳ�����printf
//����ֵ: 
extern void log_write(S_LOG *pLog, E_LOG_LEVEL level, const char *msgfmt, ...);

//��  ��: �ͷ���־�ļ���Ӧ���ļ�ָ��
//��  ��: pLogָ������־�ṹ��
//����ֵ: 
extern void log_close(S_LOG *pLog);

#ifdef __cplusplus
}
#endif


#endif
