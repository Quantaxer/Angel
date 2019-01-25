/*
  Peter Hudel
  1012673
  CIS*2750
*/

#include "CalendarParser.h"

//Helper function to tell what the current item you are reading in is (alarm, event, ical property)
void updateState(int *event, int *alarm, char *first, char *ptr, Event **evt, Calendar **cal, Alarm **alm, ICalErrorCode *error) {
    if ((strcmp(first, "BEGIN") == 0) && (strcmp(ptr, "VEVENT") == 0)) {
        //Updates event to be true and creates memory for it
        *evt = malloc(sizeof(Event));
        strcpy((*evt)->UID, "bananorama");
        (*evt)->startDateTime.date[0] = 0;
        (*evt)->creationDateTime.date[0] = 0;
        (*evt)->properties = initializeList((*printProperty), (*deleteProperty), (*compareProperties));
        (*evt)->alarms = initializeList((*printAlarm), (*deleteAlarm), (*compareAlarms));
        *event = 1;
    }
    else if ((strcmp(first, "END") == 0) && (strcmp(ptr, "VEVENT") == 0)) {
        //Resets event
        *event = 0;
        if (strcmp((*evt)->UID, "bananorama") == 0) {
            *error = INV_EVENT;
        }
        else if ((*evt)->startDateTime.date[0] == 0) {
            *error = INV_EVENT;
        }
        else if ((*evt)->creationDateTime.date[0] == 0) {
            *error = INV_EVENT;
        }
        else {
          //Appends event to the iCal list
          insertBack((*cal)->events, *evt);
        }
    }
    else if ((strcmp(first, "BEGIN") == 0) && (strcmp(ptr, "VALARM") == 0)) {
        //Updates alarm to be true
        *alm = malloc(sizeof(Alarm));
        //So I kinda need to initialize my strings to something otherwise I'll get a mem error. These are my temp values, there is NO WAY someone will
        //have a value as this.
        strcpy((*alm)->action, "bananorama");
        (*alm)->trigger = malloc(sizeof(char) * 11);
        strcpy((*alm)->trigger, "bananorama");
        (*alm)->properties = initializeList((*printProperty), (*deleteProperty), (*compareProperties));
        *alarm = 1;
    }
    else if ((strcmp(first, "END") == 0) && (strcmp(ptr, "VALARM") == 0)) {
        //Resets alarm
        *alarm = 0;
        //Error checking to see if it is a valid alarm: trigger and action must both exist
        if (strcmp((*alm)->trigger, "bananorama") == 0) {
            *error = INV_ALARM;
        }
        else if (strcmp((*alm)->action, "bananorama") == 0) {
            *error = INV_ALARM;
        }
        else {
          //Appends current alarm to the event list
          insertBack((*evt)->alarms, *alm);
        }
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
void addToEvent(char *first, char *ptr, Calendar **obj, Event **evt, int unfolded, ICalErrorCode *err) {
    //Adds the UID property to the struct
    if (strcmp(first, "UID") == 0) {
        if (strcmp((*evt)->UID, "bananorama") == 0) {
            strcpy((*evt)->UID, ptr);
        }
        else {
            *err = INV_EVENT;
        }
    }
    //Creates a new DateTime struct, and appends it to the startDT property
    else if (strcmp(first, "DTSTART") == 0) {
        char *time;
        char temp[strlen(ptr) + 1];
        strcpy(temp, ptr);
        //Seperates the Date from the Time
        time = strtok(temp, "T");
        if (strlen(time) == 8) {
            time = strtok(NULL, "T");
            if ((strlen(time) == 6) || (strlen(time) == 7)) {
                DateTime *dt =  malloc(sizeof(DateTime));
                createDate(ptr, &dt);
                (*evt)->startDateTime = *dt;
                free(dt);
            }
            else {
                *err = INV_DT;
            }
        }
        else {
            *err = INV_DT;
        }
    }
    //Creates a new DateTime struct and appends it to the dateCreated property
    else if (strcmp(first, "DTSTAMP") == 0) {
      char *time;
      char temp[strlen(ptr) + 1];
      strcpy(temp, ptr);
      //Seperates the Date from the Time
      time = strtok(temp, "T");
      if (strlen(time) == 8) {
          time = strtok(NULL, "T");
          if ((strlen(time) == 6) || (strlen(time) == 7)) {
              DateTime *dt =  malloc(sizeof(DateTime));
              createDate(ptr, &dt);
              (*evt)->creationDateTime = *dt;
              free(dt);
          }
          else {
              *err = INV_DT;
          }
      }
      else {
          *err = INV_DT;
      }
    }
    //Adds the rest of the properties into the misc category
    else if (strcmp(first, "BEGIN") != 0) {
        if (strcmp(first, "END") != 0) {
            //Create a new property struct, append the values and place it into the list
            Property *prop = malloc(sizeof(Property) + (strlen(ptr) + 1) * sizeof(char));
            strcpy(prop->propName, first);
            strcpy(prop->propDescr, ptr);
            //ADD TO EVENT LIST
            if (unfolded == 1) {
                Property *toDelete = deleteDataFromList((*evt)->properties , getFromBack((*evt)->properties));
                free(toDelete);
                //memory leak?
            }
            insertBack((*evt)->properties, prop);
        }
    }
}

//Helper function to add a property to an alarm
void addToAlarm(char *first, char *ptr, Event **evt, Alarm **alm, int unfolded) {
    //Adds an action
    if (strcmp(first, "ACTION") == 0) {
        strcpy((*alm)->action, ptr);
    }
    //Adds a trigger
    else if (strcmp(first, "TRIGGER") == 0) {
        free((*alm)->trigger);
        (*alm)->trigger = malloc((sizeof(char) + 1) * strlen(ptr));
        strcpy((*alm)->trigger, ptr);
    }
    //Adds any other property
    else if (strcmp(first, "BEGIN") != 0) {
        if (strcmp(first, "END") != 0) {
          Property *prop = malloc(sizeof(Property) + (strlen(ptr) + 1) * sizeof(char));
          strcpy(prop->propName, first);
          strcpy(prop->propDescr, ptr);
          //ADD TO ALARM LIST
          if (unfolded == 1) {
              Property *toDelete = deleteDataFromList((*alm)->properties , getFromBack((*alm)->properties));
              free(toDelete);
              //memory leak?
          }
          insertBack((*alm)->properties, prop);
        }
    }
}

//Helper function to add a property to the iCal file
void addToCal(char *first, char *ptr, Calendar **obj, int unfolded, ICalErrorCode *err, int *isVersion) {
    if (strcmp(first, "VERSION") == 0) {
        if (*isVersion == 1) {
            *err = DUP_VER;
        }
        else {
            (*obj)->version = atof(ptr);
            *isVersion = 1;
        }
    }
    //Adds the PRODID
    else if (strcmp(first, "PRODID") == 0) {
        if (strlen((*obj)->prodID) != 0) {
            *err = DUP_PRODID;
        }
        else {
            strcpy((*obj)->prodID, ptr);
        }
    }
    //Adds anything that isn't begin or end as a property
    else if (strcmp(first, "BEGIN") != 0) {
        if (strcmp(first, "END") != 0) {
            Property *prop = malloc(sizeof(Property) + (strlen(ptr) + 1) * sizeof(char));
            strcpy(prop->propName, first);
            strcpy(prop->propDescr, ptr);
            //ADD TO iCAL LIST
            if (unfolded == 1) {
                Property *toDelete = deleteDataFromList((*obj)->properties , getFromBack((*obj)->properties));
                free(toDelete);
                //memory leak?
            }
            insertBack((*obj)->properties, prop);
        }
    }
}

ICalErrorCode createCalendar(char* fileName, Calendar** obj) {
    //Variables go here
    FILE *fp;
    char *first, *ptr, prev[1000], otherPrev[1000], *x;
    char line[1000];
    char *temp;
    int wrapCount = 1;
    Event *evt = NULL;
    Alarm *alm = NULL;
    ICalErrorCode err = OK;
    int lineCount = 0;
    char fileNameCopy[strlen(fileName)];
    char *isICS;
    int isEvent = 0;
    int isAlarm = 0;
    int isUnfolding = 0;
    int isVersion = 0;

    //Create an iCal struct
    *obj = malloc(sizeof(Calendar));
    fp = fopen(fileName, "r");

    if (fp == NULL) {
        *obj = NULL;
        return INV_FILE;
    }
    //Check for valid file type
    strcpy(fileNameCopy, fileName);
    isICS = strtok(fileNameCopy, ".");
    isICS = strtok(NULL, ".");
    if (strcmp(isICS, "ics") != 0) {
        *obj = NULL;
        return INV_FILE;
    }
    //If good, initialize structure
    (*obj)->version = -1;
    strcpy((*obj)->prodID, "");
    (*obj)->properties = initializeList((*printProperty), (*deleteProperty), (*compareProperties));
    (*obj)->events = initializeList((*printEvent), (*deleteEvent), (*compareEvents));

    //Main loop for reading the file
    while (fgets(line, sizeof(line), fp)) {
        //Strip new line character from end of fgets
        strtok(line, "\n");
        //Check for comments
        if (line[0] != ';') {
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
                isUnfolding = 1;
                //Add value to appropriate struct
                if ((isEvent == 0) && (isAlarm == 0)) {
                    addToCal(otherPrev, prev, obj, isUnfolding, &err, &isVersion);
                }
                else if ((isEvent == 1) && (isAlarm == 0)) {
                    addToEvent(otherPrev, prev, obj, &evt, isUnfolding, &err);
                }
                else if ((isEvent == 1) && (isAlarm == 1)) {
                    addToAlarm(otherPrev, prev, &evt, &alm, isUnfolding);
                }

            }
            //If the current line does NOT need unfolding, go here
            else {
                wrapCount = 1;
                isUnfolding = 0;
                //Seperate into first and last part of line, and add to calendar
                ptr = strtok(line, ":;");
                first = ptr;
                ptr = strtok(NULL, "");
                strcpy(prev, ptr);
                strcpy(otherPrev, first);

                updateState(&isEvent, &isAlarm, first, ptr, &evt, obj, &alm, &err);
                if (err == OK) {
                    //Determine what state the program is in
                    if ((isEvent == 0) && (isAlarm == 0)) {
                        addToCal(first, ptr, obj, isUnfolding, &err, &isVersion);
                    }
                    else if ((isEvent == 1) && (isAlarm == 0)) {
                        addToEvent(first, ptr, obj, &evt, isUnfolding, &err);
                    }
                    else if ((isEvent == 1) && (isAlarm == 1)) {
                        addToAlarm(first, ptr, &evt, &alm, isUnfolding);
                    }
                }
                //Do error checking if it occurs in one of the addition functions
                else if (err == INV_DT) {
                    fclose(fp);
                    deleteEvent(evt);
                    deleteCalendar(*obj);
                    *obj = NULL;
                    return err;
                }
                else if (err == INV_EVENT) {
                    fclose(fp);
                    deleteEvent(evt);
                    deleteCalendar(*obj);
                    *obj = NULL;
                    return err;
                }
                else if (err == INV_ALARM) {
                    fclose(fp);
                    deleteEvent(evt);
                    deleteAlarm(alm);
                    deleteCalendar(*obj);
                    *obj = NULL;
                    return err;
                }
                else {
                    fclose(fp);
                    deleteCalendar(*obj);
                    *obj = NULL;
                    return err;
                }
            }
        }
        //File format error checking
        if ((lineCount == 0) && ((strcmp(first, "BEGIN") != 0) || (strcmp(ptr, "VCALENDAR") != 0))) {
            return INV_CAL;
        }

        //Increment line counters
        lineCount++;
    }

    fclose(fp);

    //Error checking occurs here
    //Checks if last line is correct
    if ((strcmp(first, "END") != 0) || (strcmp(ptr, "VCALENDAR") != 0)) {
        deleteCalendar(*obj);
        *obj = NULL;
        return INV_CAL;
    }
    if (isEvent == 1) {
        deleteCalendar(*obj);
        *obj = NULL;
        return INV_EVENT;
    }
    if (isAlarm == 1) {
      deleteCalendar(*obj);
      *obj = NULL;
      return INV_ALARM;
    }
    //Check if it is the correct version/ if it exists
    if ((*obj)->version != 2.0) {
        deleteCalendar(*obj);
        *obj = NULL;
        return INV_VER;
    }
    //Check if prodID exists
    if (strlen((*obj)->prodID) == 0) {
        deleteCalendar(*obj);
        *obj = NULL;
        return INV_CAL;
    }
    //Check if the cal has at least one event
    temp = toString((*obj)->events);
    if (strcmp(temp, "") == 0) {
        deleteCalendar(*obj);
        free(temp);
        *obj = NULL;
        return INV_CAL;
    }
    free(temp);
    //Check if event is missing closing tag

    return OK;
}

void deleteCalendar(Calendar* obj) {
    //need to free: list of events (call delete event), list of properties
    freeList(obj->events);
    freeList(obj->properties);
    free(obj);
}

char* printCalendar(const Calendar* obj) {
    char *str;
    char version[4];
    char *temp = toString(obj->properties);
    char *eventTemp = toString(obj->events);
    str = malloc((49 + strlen(obj->prodID) + strlen(temp) + strlen(eventTemp)) *sizeof(char));
    strcpy(str, "Version: ");
    snprintf(version, sizeof(str), "%.1f", obj->version);
    strcat(str, version);
    strcat(str, " ProdID: ");
    strcat(str, obj->prodID);
    strcat(str, "\niCal Properties: ");
    strcat(str, temp);
    strcat(str, "\nEvents: ");
    strcat(str, eventTemp);
    free(temp);
    free(eventTemp);
    return str;
}

char* printError(ICalErrorCode err) {
    if (err == INV_FILE) {
        return "Invalid file";
    }
    else if (err == INV_CAL) {
        return "Invalid calendar";
    }
    else if (err == INV_VER) {
        return "Invalid version";
    }
    else if (err == DUP_VER) {
        return "Duplicate version";
    }
    else if (err == INV_PRODID) {
        return "Invalid product ID";
    }
    else if (err == DUP_PRODID) {
        return "Duplicate product ID";
    }
    else if (err == INV_EVENT) {
        return "Invalid event";
    }
    else if (err == INV_DT) {
        return "Invalid datetime";
    }
    else if (err == INV_ALARM) {
        return "Invalid alarm";
    }
    else {
        return "Other error";
    }
}

ICalErrorCode writeCalendar(char* fileName, const Calendar* obj) {
    return OK;
}

ICalErrorCode validateCalendar(const Calendar* obj) {
    return OK;
}

void deleteEvent(void* toBeDeleted){
    //Need to free: List of properties, list of alarms(should call deleteAlarm),
    //creationdatetime struct, startdatetime struct
    Event *evt = (Event*)toBeDeleted;
    freeList(evt->properties);
    freeList(evt->alarms);
    free(toBeDeleted);
}

int compareEvents(const void* first, const void* second) {
    return 0;
}

char* printEvent(void* toBePrinted) {
    Event *evt = (Event*)toBePrinted;
    char *str;
    char *tempCreateDate = printDate(&evt->creationDateTime);
    char *tempStartDate = printDate(&evt->startDateTime);
    char *tempAlarm = toString(evt->alarms);
    char *tempProp = toString(evt->properties);
    str = malloc(sizeof(char) * (strlen(tempAlarm) + strlen(tempProp) + strlen(evt->UID) + 47 + strlen(tempStartDate) + strlen(tempCreateDate)));
    strcpy(str, "UID: ");
    strcat(str, evt->UID);
    strcat(str, "\nCreation: ");
    strcat(str, tempCreateDate);
    strcat(str, "\nStart: ");
    strcat(str, tempStartDate);
    strcat(str, "\nProperties: ");
    strcat(str, tempProp);
    strcat(str, "\nAlarms: ");
    strcat(str, tempAlarm);
    free(tempAlarm);
    free(tempProp);
    free(tempStartDate);
    free(tempCreateDate);
    return str;
}

void deleteAlarm(void* toBeDeleted) {
    //(need to free: List of properties, trigger char* array)
    Alarm *alm = (Alarm*)toBeDeleted;
    freeList(alm->properties);
    free(alm->trigger);
    free(toBeDeleted);
}

int compareAlarms(const void* first, const void* second) {
    return 0;
}

char* printAlarm(void* toBePrinted) {
    Alarm *alm = (Alarm*)toBePrinted;
    char *str;
    char *tempProp = toString(alm->properties);
    str = malloc(sizeof(char) * (strlen(alm->action) + strlen(alm->trigger) + strlen(tempProp) + 32));
    strcpy(str, "Action: ");
    strcat(str, alm->action);
    strcat(str, " Trigger: ");
    strcat(str, alm->trigger);
    strcat(str, "\nProperties: ");
    strcat(str, tempProp);
    free(tempProp);
    return str;
}

void deleteProperty(void* toBeDeleted) {
    free(toBeDeleted);
}

int compareProperties(const void* first, const void* second) {
    Property *temp1;
    Property *temp2;

    if (first == NULL || second == NULL){
  		return 0;
  	}

    temp1 = (Property*)first;
    temp2 = (Property*)second;

    return (strcmp(temp1->propName, temp2->propName) && strcmp(temp1->propDescr, temp2->propDescr));
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
    free(toBeDeleted);
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
