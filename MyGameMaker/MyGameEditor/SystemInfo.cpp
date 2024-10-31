#include "SystemInfo.h"
#include <Windows.h>
#include <string>
#include <sstream>
#include <iostream>
#include <VersionHelpers.h>

#include <iostream>


#include <vector>


#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <assimp/version.h>
#include <IL/il.h> 


static std::string GetSDLVersion()
{
    SDL_version compiled;
    SDL_VERSION(&compiled);

    std::ostringstream sdl;
    sdl << "SDL Version: " << (int)compiled.major << "." << (int)compiled.minor << "." << (int)compiled.patch;
    return sdl.str();
}

static std::string GetOpenGLVersion()
{
    std::ostringstream opengl;

    const GLubyte* version = glGetString(GL_VERSION);
    if (version)
    {
        opengl << "OpenGL Version: " << version;
    }
    else
    {
        opengl << "OpenGL Version: Not available (no context)";
    }
    return opengl.str();
}

static std::string GetAssimpVersion()
{
    std::ostringstream assimp;
    assimp << "Assimp Version: " << aiGetVersionMajor() << "."
        << aiGetVersionMinor() << "." << aiGetVersionPatch();
    return assimp.str();
}

static std::string GetDevILVersion()
{
    std::ostringstream devil;
    devil << "DevIL Version: " << IL_VERSION / 100 << "."
        << (IL_VERSION / 10) % 10 << "." << IL_VERSION % 10;
    return devil.str();
}




std::string SystemInfo::GetCPUInfo()
{
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    std::ostringstream cpu;
    cpu << "CPU Architecture: " << sysInfo.wProcessorArchitecture << ", Number of Cores: " << sysInfo.dwNumberOfProcessors;
    return cpu.str();
}

std::string SystemInfo::GetRAMInfo()
{
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);
    std::ostringstream ram;
    ram << "Total Physical RAM: " << statex.ullTotalPhys / (1024 * 1024) << " MB";
    return ram.str();
}

std::string SystemInfo::GetGPUInfo()
{
    const char* renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    const char* vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    const char* version = reinterpret_cast<const char*>(glGetString(GL_VERSION));

    std::ostringstream gpu;
    if (renderer && vendor && version)
    {
        gpu << "GPU Vendor: " << vendor << "\n"
            << "GPU Renderer: " << renderer ;
    }
    else
    {
        gpu << "GPU Info: Not available (no OpenGL context)";
    }
    return gpu.str();
}

std::string SystemInfo::GetFullSystemInfo()
{
    std::ostringstream fullInfo;
    fullInfo  << GetCPUInfo() << "\n" << GetRAMInfo() << "\n" << GetGPUInfo();


    return fullInfo.str();
}


std::string SystemInfo::GetFullLibraryVerions()
{
    std::ostringstream fullInfo;
    fullInfo << GetSDLVersion() << "\n" << GetOpenGLVersion() << "\n" << GetAssimpVersion() << "\n" << GetDevILVersion();


    return fullInfo.str();
}
