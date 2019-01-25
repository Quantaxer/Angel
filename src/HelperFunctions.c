#include "HelperFunctions.h"

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
