#include "LinkedListAPI.h"
#include "CalendarParser.h"

//Main loop for testing purposes ONLY, remove when submitting
int main(void) {
    Calendar *cal;
    ICalErrorCode err = createCalendar("src/testCalEvtPropAlm.ics", &cal);
    if (err == OK) {
      char *printString = printCalendar(cal);
      printf("%s\n", printString);
      free(printString);
      deleteCalendar(cal);
    }
    else {
      printf("%s\n", printError(err));
    }
    return 0;
}
