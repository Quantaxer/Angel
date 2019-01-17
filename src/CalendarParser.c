#include "../include/CalendarParser.h"

//Helper function to tell what the current item you are reading in is (alarm, event, ical property)
void updateState(int *event, int *alarm, char *first, char *ptr, Event **evt, Calendar **cal, Alarm **alm) {
    if ((strcmp(first, "BEGIN") == 0) && (strcmp(ptr, "VEVENT") == 0)) {
        //Updates event to be true and creates memory for it
        *evt = malloc(sizeof(Event));
        *event = 1;
    }
    else if ((strcmp(first, "END") == 0) && (strcmp(ptr, "VEVENT") == 0)) {
        //Resets event
        printf("%s\n", printEvent(*evt));
        *event = 0;
        //Appends event to the iCal list
        //TODO
    }
    else if ((strcmp(first, "BEGIN") == 0) && (strcmp(ptr, "VALARM") == 0)) {
        //Updates alarm to be true
        *alm = malloc(sizeof(Alarm));
        *alarm = 1;
    }
    else if ((strcmp(first, "END") == 0) && (strcmp(ptr, "VALARM") == 0)) {
        //Resets alarm
        printf("%s\n", printAlarm(*alm));
        *alarm = 0;
        //Appends current alarm to the iCal list
        //TODO
    }
}

//Helper function to create a new DateTime struct
void createDate(char *ptr, DateTime **dt) {
    char *time;
    //Seperates the Date from the Time
    time = strtok(ptr, "T");
    strcpy((*dt)->date, time);
    time = strtok(NULL, "");
    //Checks if UTC is true
    if (time[strlen(time) - 1] == 'Z') {
        //If it is, remove the Z from the string and set UTC to true
        (*dt)->UTC = true;
        time[strlen(time) - 1] = '\0';
    }
    strcpy((*dt)->time, time);
}

//Helper function to add a new event
void addToEvent(char *first, char *ptr, Calendar **obj, Event **evt) {
    //Adds the UID property to the struct
    if (strcmp(first, "UID") == 0) {
        strcpy((*evt)->UID, ptr);
    }
    //Creates a new DateTime struct, and appends it to the startDT property
    else if (strcmp(first, "DTSTART") == 0) {
        DateTime *dt =  malloc(sizeof(DateTime));
        createDate(ptr, &dt);
        (*evt)->startDateTime = *dt;
        //Testing print
        printf("%s\n", printDate(&(*evt)->startDateTime));
    }
    //Creates a new DateTime struct and appends it to the dateCreated property
    else if (strcmp(first, "DTSTAMP") == 0) {
        DateTime *dt =  malloc(sizeof(DateTime));
        createDate(ptr, &dt);
        (*evt)->creationDateTime = *dt;
        //Testing print
        printf("%s\n", printDate(&(*evt)->creationDateTime));
    }
    //Adds the rest of the properties into the misc category
    else if (strcmp(first, "BEGIN") != 0) {
        if (strcmp(first, "END") != 0) {
            //Create a new property struct, append the values and place it into the list
            Property *prop = malloc(sizeof(Property) + strlen(ptr) * sizeof(char));
            strcpy(prop->propName, first);
            strcpy(prop->propDescr, ptr);
            printf("%s\n", printProperty(prop));
            //ADD TO EVENT LIST
        }
    }
}

//Helper function to add a property to an alarm
void addToAlarm(char *first, char *ptr, Event **evt, Alarm **alm) {
    //Adds an action
    if (strcmp(first, "ACTION") == 0) {
        strcpy((*alm)->action, ptr);
    }
    //Adds a trigger
    else if (strcmp(first, "TRIGGER") == 0) {
        (*alm)->trigger = malloc(sizeof(char) * strlen(ptr));
        strcpy((*alm)->trigger, ptr);
    }
    //Adds any other property
    else if (strcmp(first, "BEGIN") != 0) {
        if (strcmp(first, "END") != 0) {
          Property *prop = malloc(sizeof(Property) + strlen(ptr) * sizeof(char));
          strcpy(prop->propName, first);
          strcpy(prop->propDescr, ptr);
          printf("%s\n", printProperty(prop));
          //ADD TO ALARM LIST
        }
    }
}

//Helper function to add a property to the iCal file
void addToCal(char *first, char *ptr, Calendar **obj) {
    if (strcmp(first, "VERSION") == 0) {
        //TODO: add error checking for duplicates
        (*obj)->version = atof(ptr);
    }
    //Adds the PRODID
    else if (strcmp(first, "PRODID") == 0) {
        //TODO: add error checking for duplicates
        strcpy((*obj)->prodID, ptr);
    }
    //Adds anything that isn't begin or end as a property
    else if (strcmp(first, "BEGIN") != 0) {
        if (strcmp(first, "END") != 0) {
            Property *prop = malloc(sizeof(Property) + strlen(ptr) * sizeof(char));
            strcpy(prop->propName, first);
            strcpy(prop->propDescr, ptr);
            //ADD TO iCAL LIST
        }
    }
}

ICalErrorCode createCalendar(char* fileName, Calendar** obj) {
    //Variables go here
    FILE *fp;
    char *first, *ptr, prev[1000], otherPrev[1000], *x;
    char line[1000];
    int wrapCount = 1;
    Event *evt;
    Alarm *alm;
    int lineCount = 0;
    int prevCount = 0;
    int isEvent = 0;
    int isAlarm = 0;
    //Create an iCal struct
    *obj = malloc(sizeof(Calendar));
    fp = fopen(fileName, "r");

    if (fp == NULL) {
        *obj = NULL;
        return INV_FILE;
    }

    //Main loop for reading the file
    while (fgets(line, sizeof(line), fp)) {
        //Strip new line character from end of fgets
        strtok(line, "\n");
        //line unfolding
        //Checks if it is not the first line, and if it has whitespace at the beginning
        if ((lineCount > 0) && (line[0] == ' ')) {
            //make temp string
            char temp[strlen(line) + strlen(prev)];
            //Append previous part to temp
            strcpy(temp, prev);
            //Remove first spaces
            x = strtok(line, "");
            memmove(x, x+wrapCount, strlen(x));
            //Add to end of list
            strcat(temp, x);
            //set prev value
            strcpy(prev, temp);
            lineCount++;
            wrapCount++;
        }
        //If the current line does NOT need unfolding, go here
        else {
            wrapCount = 1;
            //Checks if unfolding had just occurred
            if (lineCount - prevCount > 1) {
                //seperates first and last part of the line
                x = strtok(line, ":;");
                x = strtok(NULL, "");
                ptr = strtok(prev, "");

                updateState(&isEvent, &isAlarm, first, ptr, &evt, obj, &alm);

                //Determines what state the reading is in, and adds the current lines accordingly
                if ((isEvent == 0) && (isAlarm == 0)) {
                    addToCal(otherPrev, ptr, obj);
                    //Adds new line to calendar
                    addToCal(first, x, obj);
                }
                else if ((isEvent == 1) && (isAlarm == 0)) {
                    addToEvent(otherPrev, ptr, obj, &evt);
                    //Adds new line to event
                    addToEvent(first, x, obj, &evt);
                }
                else if ((isEvent == 1) && (isAlarm == 1)) {
                    addToAlarm(otherPrev, ptr, &evt, &alm);
                    //Adds new line to calendar
                    addToAlarm(first, x, &evt, &alm);
                }
            }
            //If no unfolding occurred, go here
            else {
                //Seperate into first and last part of line, and add to calendar
                ptr = strtok(line, ":;");
                first = ptr;
                ptr = strtok(NULL, "");
                strcpy(prev, ptr);
                strcpy(otherPrev, first);

                updateState(&isEvent, &isAlarm, first, ptr, &evt, obj, &alm);
                //Determine what state the program is in
                if ((isEvent == 0) && (isAlarm == 0)) {
                    addToCal(first, ptr, obj);
                }
                else if ((isEvent == 1) && (isAlarm == 0)) {
                    addToEvent(first, ptr, obj, &evt);
                }
                else if ((isEvent == 1) && (isAlarm == 1)) {
                    addToAlarm(first, ptr, &evt, &alm);
                }
            }

            //File format error checking
            if ((lineCount == 0) && ((strcmp(first, "BEGIN") != 0) || (strcmp(ptr, "VCALENDAR") != 0))) {
                return INV_FILE;
            }

            //Increment line counters
            prevCount = lineCount;
            lineCount++;
        }
    }

    fclose(fp);

    //Error checking occurs here
    //Checks if last line is correct
    if ((strcmp(first, "END") != 0) || (strcmp(ptr, "VCALENDAR") != 0)) {
        *obj = NULL;
        return INV_VER;
    }
    //Check if it is the correct version/ if it exists
    if ((*obj)->version != 2.0) {
        *obj = NULL;
        return INV_VER;
    }
    //Check if prodID exists
    if (strlen((*obj)->prodID) == 0) {
        *obj = NULL;
        return INV_PRODID;
    }

    return OK;
}

void deleteCalendar(Calendar* obj) {
    //need to free: list of events (call delete event), list of properties
    free(obj);
}

char* printCalendar(const Calendar* obj) {
    char *str;
    char version[4];
    str = malloc(12*sizeof(char));
    strcpy(str, "Version: ");

    str = realloc(str, (strlen(str) + 4) * sizeof(char));
    snprintf(version, sizeof(str), "%.1f", obj->version);
    strcat(str, version);

    str = realloc(str, (strlen(str) + 9) * sizeof(char));
    strcat(str, " ProdID: ");

    str = realloc(str, (strlen(str) + strlen(obj->prodID) + 1) * sizeof(char));
    strcat(str, obj->prodID);

    return str;
}

char* printError(ICalErrorCode err) {
    return "hi";
}

void deleteEvent(void* toBeDeleted){
    //Need to free: List of properties, list of alarms(should call deleteAlarm),
    //creationdatetime struct, startdatetime struct (free(blahblah->datetime shit))
}

int compareEvents(const void* first, const void* second) {
    return 0;
}

char* printEvent(void* toBePrinted) {
    Event *evt = (Event*)toBePrinted;
    char *str;
    return str;
}

void deleteAlarm(void* toBeDeleted) {
    //(need to free: List of properties, trigger char* array)
}

int compareAlarms(const void* first, const void* second) {
    return 0;
}

char* printAlarm(void* toBePrinted) {
    Alarm *alm = (Alarm*)toBePrinted;
    char *str;
    str = malloc(sizeof(char) * (strlen(alm->action) + strlen(alm->trigger) + 19));
    strcpy(str, "Action: ");
    strcat(str, alm->action);
    strcat(str, " Trigger: ");
    strcat(str, alm->trigger);
    return str;
}

void deleteProperty(void* toBeDeleted) {
    Property *propToPrint = (Property*)toBeDeleted;
    free(toBeDeleted);
}

int compareProperties(const void* first, const void* second) {
    return 0;
}

char* printProperty(void* toBePrinted) {
    char *str;
    Property *propToPrint = (Property*)toBePrinted;
    str = malloc(sizeof(char) * (strlen(propToPrint->propName) + strlen(propToPrint->propDescr) + 21));
    strcpy(str, "Name: ");
    strcat(str, propToPrint->propName);
    strcat(str, " Description: ");
    strcat(str, propToPrint->propDescr);
    strcat(str, "\0");
    return str;
}

void deleteDate(void* toBeDeleted) {
    DateTime *dt = (DateTime*)toBeDeleted;
    free(dt);
}

int compareDates(const void* first, const void* second) {
  return 0;
}

char* printDate(void* toBePrinted) {
    DateTime *dt = (DateTime*)toBePrinted;
    char *str;
    str = malloc(sizeof(char) * (strlen(dt->date) + strlen(dt->time) + 21));
    strcpy(str, "Date: ");
    strcat(str, dt->date);
    strcat(str, " Time: ");
    strcat(str, dt->time);
    strcat(str, " UTC: ");
    if (dt->UTC) {
        strcat(str, "1");
    }
    else {
        strcat(str, "0");
    }
    strcat(str, "\0");
    return str;
}
