#include <syslog.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "Conf.h"

using  namespace std;
static const  int  MAXFILE = 65535;

void dameno()
{
    pid_t pid;
    int i;
    pid = fork();     //创建一个进程用来做守护进程
    if(pid<0)
    {
        syslog(LOG_ERR, "dameno error fork \n");
        exit(1);
    }
    else if(pid>0)
    {
        exit(0);    //结束父进程
    }

    setsid();       //使子进程独立1.摆脱原会话控制 2.摆脱原进程组的控制 3.摆脱控制中端的控制
    chdir("/");     //改变当前工作目录，这也是为了摆脱父进程的影响
    umask(0);      //重设文件权限掩码
    for(i=0;i<MAXFILE;i++)
    {
        close(i);
    }
}

int main()
{
    openlog("Waring", LOG_CONS | LOG_PID, LOG_USER);

    MyConf* myconf = MyConf::getInstance();
    cout << myconf << endl;

    closelog();
    return 0;
}
