#include "times.h"
#include <itl/prayer.h>

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "support.h"
#include "interface.h"

char** labelText;
char* labelNames[] = { "fajrLabel", "shuruqLabel", "zuhrLabel",
                        "asrLabel", "maghribLabel", "ishaLabel" };
char* lblLabels[] = { "Fajr", "Shuruq", "Dhuhr", "Asr", "Maghrib", "Isha" };
char* images[] = { "fajr.png", "", "dhuhr.png",
                   "asr.png", "maghrib.png",
                   "isha.png" };
char* defaultText = "<b>Next Salat: ";

int dst;
int method;
int remind;
double gmtDiff;
double seaLevel;
double latitude;
double longitude;
char* locationName;

int timeleft;
int nextsalat;

void restoreSettings(){
   FILE* fp;
   char str[80];
   char* filestr;

   asprintf(&filestr, "%s%s", getenv("HOME"), "/.praytime_config");
   fp = fopen(filestr, "r");
   free(filestr);

   if (fp == NULL){
      latitude = 24.4833;
      longitude = 54.35;
      method = 7;
      gmtDiff = 4;
      dst = 0;
      remind = 0;
      locationName = strdup("Abu Dhabi");
      return;
   }

   while (fgets(str, 80, fp)!=NULL){
      char* tmp = strchr(str, ' ');
      if (!tmp) continue;
      tmp++;
      
      if (strstr(str, "location: ")!=NULL){
         locationName = strdup(tmp);
         tmp = strrchr(locationName, '\n');
         if (tmp != NULL) *tmp = '\0';
      }
      else if (strstr(str, "latitude: ")!=NULL)
         latitude = strtod(tmp, NULL);
      else if (strstr(str, "longitude: ")!=NULL)
         longitude = strtod(tmp, NULL);
      else if (strstr(str, "gmtDiff: ")!=NULL)
         gmtDiff = strtod(tmp, NULL);
      else if (strstr(str, "seaLevel: ")!=NULL)
         seaLevel = strtod(tmp, NULL);
      else if (strstr(str, "dst: ")!=NULL)
         dst = atoi(tmp);
      else if (strstr(str, "method: ")!=NULL)
         method = atoi(tmp);
      else if (strstr(str, "remind: ")!=NULL)
         remind = atoi(tmp);
   }
}

void saveSettings(){
   FILE* fp;
   char* outfile;

   asprintf(&outfile, "%s%s", getenv("HOME"), "/.praytime_config");
   fp = fopen(outfile, "w");
   free(outfile);

   fprintf(fp, "location: %s\n", locationName);
   fprintf(fp, "latitude: %f\n", latitude);
   fprintf(fp, "longitude: %f\n", longitude);
   fprintf(fp, "gmtDiff: %f\n", gmtDiff);
   fprintf(fp, "seaLevel: %f\n", seaLevel);
   fprintf(fp, "dst: %d\n", dst);
   fprintf(fp, "method: %d\n", method);
   fprintf(fp, "remind: %d\n", remind);
   fprintf(fp, "###");
   fclose(fp);
}

void findSalatTimes(){
   int i, val, timeint;
   Date date;
   Method conf;
   Location loc;
   time_t curTime;
   struct tm* timeStruct;
   static int initialized = 0;
   int times[6] = { 0, 0, 0, 0, 0, 0 };
   Prayer ptList[6];

   loc.degreeLat = latitude;
   loc.degreeLong = longitude;

   loc.dst = dst;
   loc.gmtDiff = gmtDiff;

   loc.seaLevel = seaLevel;
   loc.pressure = 1010;
   loc.temperature = 10;

   curTime = time(NULL);
   timeStruct = localtime(&curTime);
   date.day = timeStruct->tm_mday;
   date.month = timeStruct->tm_mon + 1;
   date.year = timeStruct->tm_year + 1900;

   getMethod(method, &conf);
   conf.round = 0;
   getPrayerTimes(&loc, &conf, &date, ptList);

   if (initialized == 0){
      labelText = malloc(6 * sizeof(char*));
      initialized++;
   }
   else {
      for (i=0; i<6; i++)
         free(labelText[i]);
   }

   for (i=0; i<6; i++)
      times[i] = ptList[i].hour * 60 + ptList[i].minute;
   timeint = timeStruct->tm_hour * 60 + timeStruct->tm_min;

   val = 0;
   for (i=0; i<6; i++){
      if (times[i] > timeint){
         val = i;
         break;
      }
   }
   if (val == 1) val = 2;
   nextsalat = val;

   val = (val == 0)? 5 : val - 1;
   if (val == 1) val = 6;

   timeleft = times[nextsalat] - timeint;
   if (timeleft < 0) timeleft = times[nextsalat] + ((23 * 60 + 59) - timeint);
   
   for (i=0; i<6; i++)
      asprintf(&(labelText[i]), 
         "<span size=\"small\" foreground=\"%s\"><b>%s:</b></span><span size=\"small\"> %3d:%02d:%02d%s</span>", 
         (i==val)? "red" : "black", lblLabels[i],
         (ptList[i].hour > 12)? ptList[i].hour-12 :
         ptList[i].hour, ptList[i].minute, 
         ptList[i].second, (ptList[i].hour>=12)?
         " pm" : " am");
}

static void on_salatTime_response(GtkDialog* dialog, gint response){
   gtk_widget_hide((GtkWidget*)dialog);
   gtk_widget_destroy(GTK_WIDGET(dialog));
}

int checkSalatTime(gpointer data){
   int timeint;
   time_t curTime;
   struct tm* timeStruct;

   curTime = time(NULL);
   timeStruct = localtime(&curTime);
   timeint = timeStruct->tm_hour * 60 + timeStruct->tm_min;
   if (timeleft == 0){
      if (remind == 1){
         GtkWidget* dialog = create_salatTimeDialog();
         g_signal_connect(dialog, "response",
                          G_CALLBACK(on_salatTime_response), NULL);
         gtk_widget_show(dialog);
      }
      reCalculateTimes();
   }
   else {
      timeleft--;
      updateTimeLeftLabel();
   }
   return TRUE;
}

void initTimes(){
   restoreSettings();
   reCalculateTimes();
   g_timeout_add(1000 * 60, checkSalatTime, NULL);
}

void reCalculateTimes(){
   int i;
   GdkPixbuf* pixbuf;
   GtkWidget* label, *image;

   findSalatTimes();

   for (i=0; i<6; i++){
      label = lookup_widget(GTK_WIDGET(mainWindow), labelNames[i]);
      gtk_label_set_text((GtkLabel*)label, labelText[i]);
      gtk_label_set_use_markup((GtkLabel*)label, TRUE);
   }

   image = lookup_widget(GTK_WIDGET(mainWindow), "image1");
   pixbuf = create_pixbuf(images[nextsalat]);
   gtk_image_set_from_pixbuf(GTK_IMAGE(image), pixbuf);
   updateTimeLeftLabel();
}

void updateTimeLeftLabel(){
   char* nextSalatStr;
   GtkWidget* label;

   asprintf(&nextSalatStr, "<span size=\"small\">%s%s</b>\n ~%d hours and %d minutes left</span>",
            defaultText, lblLabels[nextsalat], timeleft / 60, timeleft % 60);
   label = lookup_widget(mainWindow, "nextSalatLabel");
   gtk_label_set_text((GtkLabel*)label, nextSalatStr);
   gtk_label_set_use_markup((GtkLabel*)label, TRUE);
}
