#ifndef _DATE_H_
#define _DATE_H_

//Defining struct date and pointer date
typedef struct Date Date;
typedef struct Date *DatePtr;

typedef struct Date
{
    int day;
    int month;
    int year;
} Date;

DatePtr date_init(char *date);
int datecmp(DatePtr date1, DatePtr date2);
void dateDestructor(DatePtr date);
int check_dates(DatePtr vaccinationDate, DatePtr travelDate);

#endif