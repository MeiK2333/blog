#ifndef BLOG_UTILS_H
#define BLOG_UTILS_H

#include <time.h>
#include <string>

void cd(std::string);
std::string &trim(std::string &);
std::string getTime();
int get_nprocs();
void set_affnity(pid_t, int);

#endif