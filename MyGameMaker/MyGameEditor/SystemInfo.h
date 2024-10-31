#ifndef SYSTEMINFO_H
#define SYSTEMINFO_H

#include <string>

class SystemInfo
{
public:
    static std::string GetOSInfo();
    static std::string GetCPUInfo();
    static std::string GetRAMInfo();
    static std::string GetGPUInfo();

    static std::string GetFullSystemInfo();


    static std::string GetFullLibraryVerions();
};

#endif // SYSTEMINFO_H
