#ifndef TIMES_H
#define TIMES_H

#include <gtk/gtk.h>

void initTimes();
void reCalculateTimes();
void saveSettings();
void updateTimeLeftLabel();

extern GtkWidget* mainWindow;
#endif
