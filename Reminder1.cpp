#include "Watchy.h"
#include "settings.h"
#include <Arduino_JSON.h>
#include "HealthWatchy.h"

void reminder1::drawWatchFace(){

  int hour;
  int minute;
  string message;

  GetReminderInfo(hour, minute, message);
  




}