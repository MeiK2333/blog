#ifndef BLOG_UTILS_H
#define BLOG_UTILS_H

#include <time.h>
#include <string>

std::string &trim(std::string &str);
std::string getTime();
int get_nprocs();

#endif