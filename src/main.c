#include "../include/LinkedListAPI.h"
#include "../include/CalendarParser.h"

//Main loop for testing purposes ONLY, remove when submitting
int main(void) {
    Calendar *cal;
    createCalendar("src/test1.ics", &cal);
    printf("%s", printCalendar(cal));
    return 0;
}
