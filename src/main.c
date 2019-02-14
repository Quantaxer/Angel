#include "LinkedListAPI.h"
#include "CalendarParser_A2temp2.h"

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
          printf("%s\n", printError(err));
      }
      else {
          printf("%s\n", "OK");
      }
      deleteCalendar(cal);
    }
    else {
      printf("%s\n", printError(err));
    }
    return 0;
}
