#include "config.h"

#include <fstream>
#include <iostream>
#include <string>

#include "utils.h"

Config::Config(std::string configPath) {
    using namespace std;
    ifstream cfg(configPath);
    string line, key, value;
    /* 从配置文件中读取配置 */
    while (getline(cfg, line)) {
        trim(line);
        if (line[0] == '#') {
            continue;
        }
        int pos = line.find("=");
        if (pos == -1) {
            continue;
        }
        key = line.substr(0, pos);
        trim(key);
        value = line.substr(pos + 1, line.length());
        trim(value);

        if (key == "listenPort") {
            this->listenPort = stoi(value);
        } else if (key == "rootPath") {
            this->rootPath = value;
        } else if (key == "logLevel") {
            if (value == "DEBUG") {
                this->logLevel = LOG_DEBUG;
            } else if (value == "INFO") {
                this->logLevel = LOG_INFO;
            } else if (value == "NOTICE") {
                this->logLevel = LOG_NOTICE;
            } else if (value == "ERROR") {
                this->logLevel = LOG_ERROR;
            } else {
                this->logLevel = LOG_INFO;
            }
        } else if (key == "logPath") {
            this->logPath = value;
        }
    }

    /* 读取系统 CPU 核数 */
    this->cpuCount = get_nprocs();

    if (this->logLevel == LOG_DEBUG) {
        this->show();
    }
}

void Config::show() {
    using namespace std;
    cout << "Config:" << endl;
    cout << "  listenPort = " << listenPort << endl;
    cout << "  rootPath   = " << rootPath << endl;
    cout << "  logPath    = " << logPath << endl;
    cout << "  logLevel   = " << logLevel << endl;
    cout << "  cpuCount   = " << cpuCount << endl;
    cout << endl;
}