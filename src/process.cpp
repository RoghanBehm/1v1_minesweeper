#include "process.hpp" 
#ifdef _WIN32
void spawn_process(const std::string &path, const std::vector<std::string> &args) {
    std::string cmdline = "\"" + path + "\"";
    for (const auto &arg : args) {
        cmdline += " " + arg;
    }

    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    if (CreateProcess(
            nullptr,
            cmdline.data(),  // Needs to be mutable!
            nullptr, nullptr, FALSE, 0, nullptr, nullptr,
            &si, &pi)) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        std::cerr << "CreateProcess failed: " << GetLastError() << "\n";
    }
}

#else
#include <spawn.h>
#include <sys/wait.h>
extern char **environ;
void spawn_process(const std::string &path, const std::vector<std::string> &args) {
    std::vector<char *> argv;
    argv.push_back(const_cast<char *>(path.c_str()));  // argv[0] = program name
    for (const auto &arg : args) {
        argv.push_back(const_cast<char *>(arg.c_str()));
    }
    argv.push_back(nullptr);

    pid_t pid;
    if (posix_spawn(&pid, path.c_str(), nullptr, nullptr, argv.data(), environ) == 0) {
        waitpid(pid, nullptr, 0);
    } else {
        perror("posix_spawn failed");
    }
}

#endif
