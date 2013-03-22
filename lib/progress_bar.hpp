#ifndef GOWNTA_PROGRESS_BAR_H
#define GOWNTA_PROGRESS_BAR_H

#include <string>

void begin_progress();
void log_progress(const std::string & message, double progress);

#endif // GOWNTA_PROGRESS_BAR_H

