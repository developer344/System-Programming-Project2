#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "date.h"

//This function is used to create a Date date according to the char* date that is given
//Whether the form of the date is correct or not the function returns the date the validation
//of the date is done outside the function after its the date's creation
DatePtr date_init(char *date)
{
    char data[3][4];
    char *key = strtok(date, "-");
    for (int k = 0; key != NULL; k++)
    {
        strcpy(data[k], key);
        key = strtok(NULL, "-");
    }
    DatePtr returndate = malloc(sizeof(Date));
    returndate->day = atoi(data[0]);
    returndate->month = atoi(data[1]);
    returndate->year = atoi(data[2]);
    return returndate;
}

//This function is used to compare 2 dates and it returns the following in its case
//Returns -1 if date1<date2
//Returns 0 if date1=date2
//Returns 1 if date1>date2
int datecmp(DatePtr date1, DatePtr date2)
{
    if (date1->year < date2->year)
        return -1;
    else if (date1->year > date2->year)
        return 1;
    else
    {
        if (date1->month < date2->month)
            return -1;
        else if (date1->month > date2->month)
            return 1;
        else
        {
            if (date1->day < date2->day)
                return -1;
            else if (date1->day > date2->day)
                return 1;
            else
                return 0;
        }
    }
}

int check_dates(DatePtr vaccinationDate, DatePtr travelDate)
{
    if (datecmp(vaccinationDate, travelDate) < 0)

        if (vaccinationDate->year == travelDate->year)
            if (travelDate->month - vaccinationDate->month < 6)
                return 1;
            else if (travelDate->month - vaccinationDate->month == 6)
                if (vaccinationDate->day - travelDate->day >= 0)
                    return 1;
                else
                    return 0;
            else
                return 0;
        else if (travelDate->year > vaccinationDate->year)
            if (travelDate->year - vaccinationDate->year <= 1)
            {
                if (travelDate->month - vaccinationDate->month < -6)
                    return 1;
                else if (travelDate->month - vaccinationDate->month == -6)
                    if (vaccinationDate->day - travelDate->day >= 0)
                        return 1;
                    else
                        return 0;
                else
                    return 0;
            }
            else
                return 0;

        else
            return 0;

    else

        return 0;
}

//This function is used to destruct a date
void dateDestructor(DatePtr date)
{
    free(date);
}