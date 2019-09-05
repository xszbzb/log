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

//功  能: 写日志,默认输出日志等级为LOG_LEVEL_ERROR
//参  数: level代表日志信息等级,后面的参数如printf
//返回值: 
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

//LOG_LEVEL_EMERG：紧急状况
//LOG_LEVEL_ALERT：高优先级问题，比如说数据库崩溃等，必须要立即采取反应行动
//LOG_LEVEL_CRIT：重要状况发生，比如硬件故障
//LOG_LEVEL_ERROR：错误发生
//LOG_LEVEL_WARNING：警告发生
//LOG_LEVEL_NOTICE：一般状况，需要引起注意
//LOG_LEVEL_INFO：信息状况
//LOG_LEVEL_DEBUG：调试消息

typedef enum {  
	LOG_LEVEL_EMERG,
	LOG_LEVEL_ALERT,
	LOG_LEVEL_CRIT,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_WARN,
	LOG_LEVEL_NOTICE,//默认输出日志等级
	LOG_LEVEL_INFO,
	LOG_LEVEL_DEBUG
}E_LOG_LEVEL;

typedef struct tag_log
{
	char strPathName[MAX_LOG_PATH_NAME_T];//日志文件路径名前缀,为空时会自动获取
	FILE* fp;//日志文件指针
	size_t seq;//写入seq次日志后做一次文件切割判断
	size_t hour;//保存时间小时数,超过时间会被删除
	size_t count;//日志文件保存个数,为0时不换新文件,为1时单个文件超大后刷新重写
	size_t size;//日志文件大小超过多大换新
	E_LOG_LEVEL level;//打印日志级别
}S_LOG;
extern S_LOG g_log;

//功  能: 设置默认日志文件输出等级和名字
//参  数: 	char strPathName[MAX_LOG_PATH_NAME_T];//日志文件路径名前缀,为空时会自动获取
//			size_t hour;//保存时间小时数,超过时间会被删除
//			size_t count;//日志文件保存个数,为0时不换新文件,为1时单个文件超大后刷新重写
//			size_t size;//日志文件大小超过多大换新
//			E_LOG_LEVEL level;//打印日志级别
//返回值:   0为成功
extern int log_open(S_LOG *pLog, const char *strPathName, E_LOG_LEVEL level, size_t count, size_t size, size_t hour);

//功  能: 写日志到指定的日志文件中
//参  数: pLog指定的日志结构体,level代表日志信息等级,后面的参数如printf
//返回值: 
extern void log_write(S_LOG *pLog, E_LOG_LEVEL level, const char *msgfmt, ...);

//功  能: 释放日志文件对应的文件指针
//参  数: pLog指定的日志结构体
//返回值: 
extern void log_close(S_LOG *pLog);

#ifdef __cplusplus
}
#endif


#endif
