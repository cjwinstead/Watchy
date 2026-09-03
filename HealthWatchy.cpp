#include <Watchy.h>
#include "settings.h"
#include "icons.h"
#include <Arduino_JSON.h>
#include <HTTPClient.h>
using namespace std;


//how big is the display, how to know where to put cursor
//how does setcursor/getTextBounds/print1n/fillRect/drawbitmap functions work
//want display with time large, date underneath, and battery 
//change display when alarm goes off
//either hard code times for alarms or modify the menu to create an alarm

RTC_DATA_ATTR int reminderHour = -1;
RTC_DATA_ATTR int reminderMinute = -1;
RTC_DATA_ATTR char reminderMessage[64] = "";
RTC_DATA_ATTR int reminderFetchCounter = 0;



class HealthWatchy : public Watchy{
  public:
    using Watchy::Watchy;
    void drawWatchFace();

  private:
    void drawTime(int displayHour); //claude declaration, different from mine
    void drawDate();
    void drawBattery();
    void drawReminderScreen();


  
};

void HealthWatchy::drawWatchFace() { //function to run as top
  display.fillScreen(GxEPD_BLACK);
  display.setTextColor(GxEPD_WHITE);
  drawTime();
  drawDate();
  drawBattery();
   display.drawBitmap(116, 75, WIFI_CONFIGURED ? wifi : wifioff, 26, 18, GxEPD_WHITE);
  
}
//watchy has a size of 200X200 pixels with 0,0 being top left
void HealthWatchy::drawTime() { //overriding the drawTime module
  display.setFont(&DSEG7_Classic_Bold_25);
  display.setTextColor(GxEPD_WHITE);
  display.setCursor(50, 40);  //placed in top center of screen
  int displayHour;
  if(HOUR_12_24 == 12) {
    displayHour = ((currentTime.Hour + 11)%12) + 1;
  }
  else {
    displayHour = currentTime.Hour;
  }
  if(displayHour < 10){
    display.print("0");
  }
  display.print(displayHour);
  display.print(":");
  if(currentTime.Minute < 10) {
    display.print("0");
  }
  display.println(currentTime.Minute); //what's difference between print and println

  if((reminderHour == displayHour) && (reminderMinute == currentTime.Minute)) {
    //set new watch face
     display.fillscreen(GxEPD_BLACK);
    display.setTextColor(GxEPD_WHITE);
    display.setCursor(100, 20);
    display.print(reminderMessage);
    //if menu button, revert to previous screen
    
  }
}
//draw battery icon 
void HealthWatchy::drawBattery() {
  display.drawRect(164, 10, 26, 10, GxEPD_WHITE);
  display.fillRect(190, 13, 4, 5, GxEPD_WHITE);
  float VBAT = getBatteryVoltage();
    if(VBAT > 4.0){
        batteryLevel = 3;
        display.fillRect(164, 10, 26, 10, GxEPD_WHITE);
    }
    else if(VBAT > 3.6 && VBAT <= 4.0){
        batteryLevel = 2;
        display.fillRect(164, 10, 13, 10, GxEPD_WHITE);
    }
    else if(VBAT > 3.20 && VBAT <= 3.6){
        batteryLevel = 1;
        display.fillRect(164, 10, 6, 10, GxEPD_WHITE);
    }
    else if(VBAT <= 3.20){
        batteryLevel = 0;
        display.fillRect(164, 10, 26, 10, GxEPD_BLACK);
    }

}

void HealthWatchy::drawDate() {
  int16_t x1, y1;
  uint16_t w, h;
  display.setFont(&DSEG7_Classic_Bold_25); //want a bigger font here
  const char* daynames[] = {"Sunday", "Monday", "Tuesday", "Wednesday","Thursday", "Friday", "Saturday"};
  const char* dayOfWeek = daynames[currentTime.Wday -1 ];
  //confused on how all the pixel stuff is configured
  display.getTextBounds(dayOfWeek, 70, 80, &x1, &y1, &w, &h);
  display.setCursor(100 - (w/2) ,80); //print weekday in center of screen
  display.print(dayOfWeek);

  //don't know if syntax is correct, getting confused with verilog
  //want one string to represent the date
  //what is dayStr or monthShortStr or tmYearToCalendar variables?
  const char* months[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September",
  "October", "November", "December"};
  char date[24];
  snprintf(date, sizeof(date), "%s %02d %d", months[currentTime.Month-1],
  currentTime.Day, tmYearToCalendar(currentTime.Year));
  display.getTextBounds(date, 20, 120, &x1, &y1, &w, &h);
  display.setCursor(100 - (w/2) ,120); //print date in center of screen
  if(currentTime.Day < 10) {
    date = months[currentTime.Month-1] + " 0" + currentTime.Day + currentTime.Year}
  }
  display.println(date);
}

  void GetReminderInfo(int &reminderHour, int &reminderMinute, String &reminderMessage) {
    
    if(reminderFetchCounter >= 60){
    if(WIFI_CONFIGURED) {
      HTTPClient http;
      http.setConnectTimeout(3000);
      String ReminderURL;
      http.begin(ReminderURL.c_str()); //confused by this
      int httpResponseCode = http.GET(); //how do they know what information to take from the http
      if(httpResponseCode == 200) {
        String task = http.getString();
        JSONVar responseObject = JSON.parse(task);
        reminderHour = int(responseObject["hour"]);
        reminderMinute = int(responseObject["minute"]);
        reminderMessage =String(responseObject["message"]);
      }
      http.end();
      reminderCounter = 0;

    }
    }
    else {
      reminderCounter++;
    }
  }

//what is drawBitmap function?
