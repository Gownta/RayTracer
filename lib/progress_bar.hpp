#ifndef GOWNTA_PROGRESS_BAR_H
#define GOWNTA_PROGRESS_BAR_H

#include <string>

void log_progress(const std::string & message, double progress, bool local_time = false);
void complete_progress(const std::string & message);

#endif // GOWNTA_PROGRESS_BAR_H

