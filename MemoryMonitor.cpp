
#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <dirent.h>
#include <cstring>

using namespace std;
int GetProcessIdByName(const char* processName) {
    FILE* pipe = nullptr;
    char buffer[128];
    std::string command = "ps aux | grep ";
    command += processName;
    command += " | grep -v grep | awk '{print $2}'";

    pipe = popen(command.c_str(), "r");
    if (!pipe) {
        std::cerr << "popen() failed." << std::endl;
        return -1;
    }

    fgets(buffer, 128, pipe);
    int pid = atoi(buffer);
    pclose(pipe);
    return pid;
}



// Function to get memory usage of current process
double printMemoryUsage(int pid)
{
//    int pid = getpid();
    string statmfile="/proc/"+to_string(pid)+"/statm";
    std::ifstream stat_stream(statmfile.c_str());
    double mem;
    if (stat_stream.is_open())
    {
        unsigned long size, resident, share, text, lib, data, dt;
        stat_stream >> size >> resident >> share >> text >> lib >> data >> dt;
        stat_stream.close();

        long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // KB per page
        mem=(double)resident * page_size_kb/1024;
//        std::cout << "Memory usage: " << mem << " MB" << std::endl;
    }
    else
    {
        std::cerr << "Failed to open "<<statmfile<< std::endl;
        return -1;
    }
    return mem;
}

// Function to get thread count of current process
int printThreadCount(int pid)
{
//    int pid = getpid();
    DIR *dir;
    struct dirent *entry;
    int threadCount = 0;
    string taskdir="/proc/"+to_string(pid)+"/task";
    if ((dir = opendir(taskdir.c_str())) != nullptr)
    {
        while ((entry = readdir(dir)) != nullptr)
        {
            if (entry->d_type == DT_DIR)
            {
                std::string task_dirname = entry->d_name;
                if (task_dirname.find_first_not_of("0123456789") == std::string::npos)
                {
                    threadCount++;
                }
            }
        }
        closedir(dir);
//        std::cout << "Thread count: " << threadCount << std::endl;
    }
    else
    {
        std::cerr << "Failed to open "<<taskdir<<" directory" << std::endl;
        return -1;
    }
    return threadCount;
}

int main(int argc, char* argv[])
{
    const char* processName="track_test"; // Replace with your process name
    if (argc > 1) {
        processName = argv[1];
    }

    while(true)
    {
        struct tm *cur_time;
        time_t local_time;
        time(&local_time);
        cur_time = localtime(&local_time);

        /* 获取当前时间命名日志文件 */
        char time_str[200];
        strftime(time_str, 100, "[%Y-%m-%d %H:%M:%S]", cur_time);
        int pid = GetProcessIdByName(processName);
        double mem=printMemoryUsage(pid);
        int threadcnt=printThreadCount(pid);
        printf("%s processName:%s pid:%d Memory usage: %3f MB,Thread count: %d\n",time_str,processName,pid,mem,threadcnt);
        sleep(10);
    }
        return 0;
}
