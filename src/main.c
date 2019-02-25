#include "LinkedListAPI.h"
#include "CalendarParser.h"

//Main loop for testing purposes ONLY, remove when submitting
/*int main(void) {
    Calendar *cal;
    Event *evt;
    ICalErrorCode err;
    Alarm *alm;
    Property *prop;
    Property *prop3;
    cal = malloc(sizeof(Calendar));
    //Create a valid calendar
    cal->version = 2.0;
    strcpy(cal->prodID, "hello");
    cal->properties = initializeList((*printProperty), (*deleteProperty), (*compareProperties));
    cal->events = initializeList((*printEvent), (*deleteEvent), (*compareEvents));

    //Create a valid event
    evt = malloc(sizeof(Event));
    strcpy(evt->UID, "hi");
    strcpy(evt->startDateTime.date, "19990615");
    strcpy(evt->startDateTime.time, "121212");
    strcpy(evt->creationDateTime.date, "19990615");
    strcpy(evt->creationDateTime.time, "121212");
    evt->properties = initializeList((*printProperty), (*deleteProperty), (*compareProperties));
    evt->alarms = initializeList((*printAlarm), (*deleteAlarm), (*compareAlarms));

    //Remeber to allocate size of flexible array

    alm = malloc(sizeof(Alarm));
    strcpy(alm->action, "hi");
    alm->trigger = "bye";
    alm->properties = initializeList((*printProperty), (*deleteProperty), (*compareProperties));

    prop = malloc(sizeof(Property) + 2);
    strcpy(prop->propName, "DURATION");
    strcpy(prop->propDescr, "s");
    insertBack(alm->properties, prop);

    prop3 = malloc(sizeof(Property) + 2);
    strcpy(prop3->propName, "REPEAT");
    strcpy(prop3->propDescr, "a");
    insertBack(alm->properties, prop3);

    insertBack(evt->alarms, alm);
    insertBack(cal->events, evt);


    err = validateCalendar(cal);
    if (err != 0) {
        printf("Validated: %s\n", printError(err));
    }
    else {
        printf("Validated: %s\n", "OK");
    }*/

//Main loop for testing purposes ONLY, remove when submitting
int main(void) {
    Calendar *cal;
    ICalErrorCode err = createCalendar("src/mLinePropTab1.ics", &cal);
    if (err == OK) {
      char *printString = printCalendar(cal);
      printf("%s\n", printString);
      free(printString);
      writeCalendar("src/testWrite.ics", cal);
      char *j = calendarToJSON(cal);
      printf("%s\n", j);
      free(j);
      err = validateCalendar(cal);
      if (err != 0) {
          printf("Validated: %s\n", printError(err));
      }
      else {
          printf("Validated: %s\n", "OK");
      }
      deleteCalendar(cal);
    }
    else {
      printf("%s\n", printError(err));
    }
    /*char *str = "{\"version\":245,\"prodID\":\"-//hacksw/handcal//NONSGML v1.0//EN\"}";
    char *str2 = "{\"UID\":\"1234\"}";
    cal = JSONtoCalendar(str);
    Event *evt = JSONtoEvent(str2);
    if (cal != NULL) {
        printf("%d, %s\n", (int)cal->version, cal->prodID);
    }
    if (evt != NULL) {
        printf("%s\n", evt->UID);
    }
    free(cal);
    free(evt);*/
    return 0;
}
