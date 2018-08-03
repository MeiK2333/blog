#include "config.h"

#include <fstream>
#include <iostream>
#include <string>

#include "utils.h"

Config::Config(std::string configPath) {
    using namespace std;
    ifstream cfg(configPath);
    string line, key, value;
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
            Config::listenPort = stoi(value);
        } else if (key == "rootPath") {
            Config::rootPath = value;
        } else if (key == "logLevel") {
            if (value == "DEBUG") {
                Config::logLevel = LOG_DEBUG;
            } else if (value == "INFO") {
                Config::logLevel = LOG_INFO;
            } else if (value == "NOTICE") {
                Config::logLevel = LOG_NOTICE;
            } else if (value == "ERROR") {
                Config::logLevel = LOG_ERROR;
            } else {
                Config::logLevel = LOG_INFO;
            }
        } else if (key == "logPath") {
            Config::logPath = value;
        }
    }

    if (Config::logLevel == LOG_DEBUG) {
        Config::show();
    }
}

void Config::show() {
    using namespace std;
    cout << "Config:" << endl;
    cout << "  listenPort = " << listenPort << endl;
    cout << "  rootPath   = " << rootPath << endl;
    cout << "  logPath    = " << logPath << endl;
    cout << "  logLevel   = " << logLevel << endl;
    cout << endl;
}