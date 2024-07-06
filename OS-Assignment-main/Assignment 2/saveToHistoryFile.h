#ifndef SAVE_TO_HISTORY_FILE_H
#define SAVE_TO_HISTORY_FILE_H

#include <stdio.h>
#include <time.h>
#include <sys/types.h>

void saveToHistoryFile(pid_t pid, time_t StartTheDuration, double duration, char *command);

#endif
