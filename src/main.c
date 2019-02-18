#include "LinkedListAPI.h"
#include "CalendarParser.h"

//Main loop for testing purposes ONLY, remove when submitting
/*int main(void) {
    Calendar *cal;
    Event *evt;
    ICalErrorCode err;
    Alarm *alm;
    Property *prop;
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
    prop = malloc(sizeof(Property) + 2);
    strcpy(prop->propName, "DESCRIPTION");
    strcpy(prop->propDescr, "s");
    insertBack(evt->properties, prop);

    alm = malloc(sizeof(Alarm));
    strcpy(alm->action, "hi");
    alm->trigger = "bye";
    alm->properties = initializeList((*printProperty), (*deleteProperty), (*compareProperties));

    insertBack(evt->alarms, alm);
    insertBack(cal->events, evt);


    err = validateCalendar(cal);
    if (err != 0) {
        printf("Validated: %s\n", printError(err));
    }
    else {
        printf("Validated: %s\n", "OK");
    }

    return 0;
}*/

//Main loop for testing purposes ONLY, remove when submitting
int main(void) {
    Calendar *cal;
    ICalErrorCode err = createCalendar("src/testCalEvtPropAlm.ics", &cal);
    if (err == OK) {
      char *printString = printCalendar(cal);
      printf("%s\n", printString);
      free(printString);
      writeCalendar("src/testWrite.ics", cal);
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
    return 0;
}
