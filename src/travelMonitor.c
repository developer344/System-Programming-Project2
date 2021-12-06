#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include "CountryReferenceList.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>

#include "bloomfilter.h"
#include "date.h"
#include "requests.h"

char **divideStr(char *str, int buffer_size, int n)
{

    int str_size = strlen(str);
    char **str_parts = malloc(sizeof(char *) * n);
    int index = 0;
    for (int i = 0; i < n; i++)
    {
        str_parts[i] = malloc(sizeof(char) * buffer_size);
        strncpy(str_parts[i], str + (i * buffer_size), buffer_size);
    }
    return str_parts;
}

char *curr_date()
{
    //------------------------------------Current date------------------------------------//
    //This function calculates the current date every time its called and returns
    //a string of the date
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char year[5];
    char month[4];
    char day[4];
    char *currDate = malloc(sizeof(char) * 12);
    sprintf(year, "%d", tm.tm_year + 1900);
    sprintf(month, "%d", tm.tm_mon + 1);
    sprintf(day, "%d", tm.tm_mday);
    strcpy(currDate, day);
    strcat(currDate, "-");
    strcat(currDate, month);
    strcat(currDate, "-");
    strcat(currDate, year);
    return currDate;
}

int flag_intsig = 0;

int flag_quitsig = 0;

int flag_chldsig = 1;

void sigchld_handler(int sig)
{
    flag_chldsig = 0;
}

void sigint_handler(int sig)
{
    flag_intsig = 1;
}

void sigquit_handler(int sig)
{
    flag_quitsig = 1;
}

int main(int argc, char **argv)
{
    signal(SIGCHLD, &sigchld_handler);
    signal(SIGINT, &sigint_handler);
    signal(SIGQUIT, &sigquit_handler);
    //Checking the number of arguments
    if (argc != 9)
    {
        perror("ERROR: Wrong number of arguments!");
        exit(1);
    }
    int numMonitors;
    int bufferSize;
    int sizeOfBloom;
    char *bloomSize;
    char *input_dir;
    //Reading the number of monitors that the application will create during its run time
    for (int i = 0; i < argc - 1; i++)
    {
        if (!strcmp(argv[i], "-m"))
        {
            numMonitors = atoi(argv[i + 1]);
            break;
        }
    }
    //Reading the size of the buffer
    for (int i = 0; i < argc - 1; i++)
        if (!strcmp(argv[i], "-b"))
        {
            bufferSize = atoi(argv[i + 1]);
            break;
        }
    //Reading the number of bytes that the bloomfilter bit array will have
    for (int i = 0; i < argc - 1; i++)
    {
        if (!strcmp(argv[i], "-s"))
        {
            //Multipluing by 8 to convert from bytes to bits
            sizeOfBloom = 8 * (atoi(argv[i + 1]));
            bloomSize = malloc(sizeof(char) * (strlen(argv[i + 1]) + 1));
            strcpy(bloomSize, argv[i + 1]);
            break;
        }
    }
    //Reading the input directory from the command line
    for (int i = 0; i < argc - 1; i++)
        if (!strcmp(argv[i], "-i"))
        {
            input_dir = malloc(sizeof(char) * strlen(argv[i + 1]));
            strcpy(input_dir, argv[i + 1]);
            break;
        }
    printf("Number of monitors: %d\nBuffer size: %d\nSize of bloomfilter: %d\nInput Directory: %s\n", numMonitors, bufferSize, sizeOfBloom, input_dir);
    char **readp = malloc(sizeof(char *) * numMonitors);
    char **writep = malloc(sizeof(char *) * numMonitors);
    unsigned *numOfCountriesPerMonitor = malloc(sizeof(int) * numMonitors);
    mkdir("FiFos", 0777);
    for (int i = 0; i < numMonitors; i++)
    {
        numOfCountriesPerMonitor[i] = 0;
        readp[i] = malloc(sizeof(char) * 30);
        writep[i] = malloc(sizeof(char) * 30);
        strcpy(writep[i], "FiFos/MonitorToTravelMon");
        strcpy(readp[i], "FiFos/TravelMonToMonitor");
        char *num = malloc(sizeof(char) * 5);
        sprintf(num, "%d", i);
        strcat(readp[i], num);
        strcat(writep[i], num);
        mkfifo(readp[i], 0777);
        mkfifo(writep[i], 0777);
        free(num);
    }
    int readpipes[numMonitors];
    int writepipes[numMonitors];
    char **arguments = malloc(sizeof(sizeof(char *) * 2));
    arguments[0] = malloc(sizeof(sizeof(char) * 20));
    arguments[1] = malloc(sizeof(sizeof(char) * 20));
    strcpy(arguments[0], "./bin/monitor");
    pid_t pid[numMonitors];
    for (int i = 0; i < numMonitors; i++)
    {
        // arguments[1] = malloc(sizeof(char) * 20);
        // arguments[2] = malloc(sizeof(char) * 20);
        // strcpy(arguments[1], readp[i]);
        // strcpy(arguments[0], write[i]);
        sprintf(arguments[1], "%d", i);

        pid[i] = fork();
        if (pid[i] == -1)
        {
            printf("Error in fork");
        }
        else if (pid[i] == 0)
        {
            //Creating monitors
            char buffSize[10];
            sprintf(buffSize, "%d", bufferSize);
            sprintf(arguments[1], "%d", i);
            execl(arguments[0], arguments[1], readp[i], writep[i], "-b", buffSize, "-s", bloomSize, NULL);
        }
        else
            writepipes[i] = open(writep[i], O_WRONLY);
    }

    //Opening input directory and sending subdirectories to the monitors
    struct dirent *in_file;
    DIR *inputDir = opendir(input_dir);
    if (inputDir == NULL)
    {
        printf("Could not open current directory");
        exit(1);
    }
    int monitor_index = 0;
    CountryRefListPtr CList = malloc(sizeof(CountryRefList));

    CountryRefListInit(CList);
    while ((in_file = readdir(inputDir)) != NULL)
    {
        //Only the true files will be used
        if (!strcmp(in_file->d_name, "."))
            continue;
        if (!strcmp(in_file->d_name, ".."))
            continue;
        char *countryName = malloc(sizeof(char) * strlen(in_file->d_name));
        strcpy(countryName, in_file->d_name);
        CRLinsertRecord(CList, countryName, monitor_index);
        numOfCountriesPerMonitor[monitor_index]++;
        monitor_index = (monitor_index + 1) % numMonitors;
        free(countryName);
    }
    for (monitor_index = 0; monitor_index < numMonitors; monitor_index++)
    {
        if (bufferSize >= 4)
            write(writepipes[monitor_index], &numOfCountriesPerMonitor[monitor_index], bufferSize);
        else
        {
            short lastHalf = numOfCountriesPerMonitor[monitor_index] & 0xFFFF;
            short firstHalf = numOfCountriesPerMonitor[monitor_index] >> 16;
            write(writepipes[monitor_index], &lastHalf, bufferSize);
            write(writepipes[monitor_index], &firstHalf, bufferSize);
        }
    }
    CountryRefListNodePtr currNode = CList->FirstNode;
    while (currNode != NULL)
    {
        char *str = malloc(sizeof(char) * (strlen(currNode->countryName) + strlen(input_dir) + 1));
        strcpy(str, input_dir);
        strcat(str, "/");
        strcat(str, currNode->countryName);
        float n_f = ((float)strlen(str) + 1.0) / (float)bufferSize;
        unsigned int n = ceil(n_f);
        char **ss = divideStr(str, bufferSize, n);
        if (bufferSize >= 4)
            write(writepipes[currNode->monitorIndex], &n, bufferSize);
        else
        {
            short lastHalf = n & 0xFFFF;
            short firstHalf = n >> 16;
            write(writepipes[currNode->monitorIndex], &lastHalf, bufferSize);
            write(writepipes[currNode->monitorIndex], &firstHalf, bufferSize);
        }
        for (int j = 0; j < n; j++)
        {
            //printf("Sending: %s\n", ss[j]);
            write(writepipes[currNode->monitorIndex], ss[j], bufferSize);
        }
        for (int i = 0; i < n; i++)
        {
            free(ss[i]);
        }
        free(ss);
        free(str);
        currNode = currNode->nextNode;
    }
    bloomFilterPtr bloomMonitorArray[numMonitors];
    for (monitor_index = 0; monitor_index < numMonitors; monitor_index++)
        close(writepipes[monitor_index]);

    for (monitor_index = 0; monitor_index < numMonitors; monitor_index++)
    {
        readpipes[monitor_index] = open(readp[monitor_index], O_RDONLY);
        bloomMonitorArray[monitor_index] = malloc(sizeof(bloomFilter));
        bloomFilter_init(bloomMonitorArray[monitor_index], NUMOFHASHFUNCTIONS, sizeOfBloom);
        unsigned n = ((sizeOfBloom / 8) + (sizeOfBloom % 8 > 0 ? 1 : 0));
        for (int j = 0; j < n; j++)
        {
            read(readpipes[monitor_index], bloomMonitorArray[monitor_index]->bloomBitArray + j, bufferSize);
        }
        close(readpipes[monitor_index]);
    }

    closedir(inputDir);
    //This will hold the data from its line that is read
    char line[100];
    //This will hold the data from its line that is read separetly
    char data[10][30];
    //To be used on the separation of the line inyo its compoonents
    char *key;
    //------------------------------------Options------------------------------------//
    int numOfArguments;
    unsigned command;
    requestListPtr RList = malloc(sizeof(requestList));
    Rlist_init(RList);
    do
    {
        //I print the options available
        printf("========\n");
        printf("OPTIONS:\n");
        printf("=================================================================\n");
        printf("/travelRequest citizenID date countryFrom countryTovirusName\n");
        printf("/travelStats virusName date1 date2 [country]\n");
        printf("/addVaccinationRecords country\n");
        printf("/searchVaccinationStatus citizenID\n");
        printf("/exit\n");
        printf("=================================================================\n");

        //I read the users input

        fgets(line, sizeof(line), stdin);

        line[strcspn(line, "\n")] = 0;

        key = strtok(line, " ");

        for (numOfArguments = 0; key != NULL; numOfArguments++)
        {

            strcpy(data[numOfArguments], key);
            key = strtok(NULL, " ");
        }
        if (flag_chldsig == 0)
        {
            int stat;
            int deadchild_index = -1;
            for (int i = 0; i < numMonitors; i++)
            {
                if (waitpid(pid[i], &stat, WNOHANG))
                {
                    deadchild_index = i;
                    break;
                }
            }
            if (deadchild_index >= 0)
            {
                pid[deadchild_index] = fork();
                if (pid[deadchild_index] == -1)
                {
                    printf("Error in fork");
                }
                else if (pid[deadchild_index] == 0)
                {
                    //Creating monitors
                    char buffSize[10];
                    sprintf(buffSize, "%d", bufferSize);
                    sprintf(arguments[1], "%d", deadchild_index);
                    execl(arguments[0], arguments[1], readp[deadchild_index], writep[deadchild_index], "-b", buffSize, "-s", bloomSize, NULL);
                }
                else
                    writepipes[deadchild_index] = open(writep[deadchild_index], O_WRONLY);

                if (bufferSize >= 4)
                    write(writepipes[deadchild_index], &numOfCountriesPerMonitor[deadchild_index], bufferSize);
                else
                {
                    short lastHalf = numOfCountriesPerMonitor[deadchild_index] & 0xFFFF;
                    short firstHalf = numOfCountriesPerMonitor[deadchild_index] >> 16;
                    write(writepipes[deadchild_index], &lastHalf, bufferSize);
                    write(writepipes[deadchild_index], &firstHalf, bufferSize);
                }
                currNode = CList->FirstNode;
                while (currNode != NULL)
                {
                    char *str = malloc(sizeof(char) * (strlen(currNode->countryName) + strlen(input_dir) + 1));
                    strcpy(str, input_dir);
                    strcat(str, "/");
                    strcat(str, currNode->countryName);
                    float n_f = ((float)strlen(str) + 1.0) / (float)bufferSize;
                    unsigned int n = ceil(n_f);
                    char **ss = divideStr(str, bufferSize, n);
                    if (bufferSize >= 4)
                        write(writepipes[currNode->monitorIndex], &n, bufferSize);
                    else
                    {
                        short lastHalf = n & 0xFFFF;
                        short firstHalf = n >> 16;
                        write(writepipes[currNode->monitorIndex], &lastHalf, bufferSize);
                        write(writepipes[currNode->monitorIndex], &firstHalf, bufferSize);
                    }
                    for (int j = 0; j < n; j++)
                    {
                        //printf("Sending: %s\n", ss[j]);
                        write(writepipes[currNode->monitorIndex], ss[j], bufferSize);
                    }
                    for (int i = 0; i < n; i++)
                    {
                        free(ss[i]);
                    }
                    free(ss);
                    free(str);
                    currNode = currNode->nextNode;
                }
                bloom_destructor(bloomMonitorArray[deadchild_index]);
                close(writepipes[deadchild_index]);

                readpipes[deadchild_index] = open(readp[deadchild_index], O_RDONLY);
                bloomMonitorArray[deadchild_index] = malloc(sizeof(bloomFilter));
                bloomFilter_init(bloomMonitorArray[deadchild_index], NUMOFHASHFUNCTIONS, sizeOfBloom);
                unsigned n = ((sizeOfBloom / 8) + (sizeOfBloom % 8 > 0 ? 1 : 0));
                for (int j = 0; j < n; j++)
                {
                    read(readpipes[deadchild_index], bloomMonitorArray[deadchild_index]->bloomBitArray + j, bufferSize);
                }
                close(readpipes[deadchild_index]);
            }
            flag_chldsig == 1;
        }
        if (!strcmp(data[0], "/travelRequest"))
        {
            command = 1;
            if (numOfArguments == 6)
            {
                //Checking if citizen id is within the limits
                if (atoi(data[1]) > 0 && atoi(data[1]) < 10000)
                {
                    //Checking  if date is valid
                    char dateStr[12];
                    strcpy(dateStr, data[2]);
                    DatePtr date = date_init(data[2]);
                    if (date->day > 0 && date->day <= 30 && date->month > 0 && date->month <= 12 && date->year > 1900 && date->year < 3000)
                    {
                        currNode = CList->FirstNode;
                        monitor_index = -1;
                        while (currNode != NULL)
                        {
                            if (!strcmp(data[3], currNode->countryName))
                            {
                                monitor_index = currNode->monitorIndex;
                                break;
                            }
                            currNode = currNode->nextNode;
                        }
                        if (monitor_index != -1)
                        {
                            char *countryName = malloc(sizeof(char) * strlen(data[3]));
                            strcpy(countryName, data[3]);
                            char *virusName = malloc(sizeof(char) * strlen(data[3]));
                            strcpy(virusName, data[5]);
                            if (bloom_checkElement(bloomMonitorArray[monitor_index], data[1]) == 0)
                            {
                                printf("REQUEST REJECTED – YOU ARE NOT VACCINATED\n");
                                char datestr[12];
                                strcpy(datestr, dateStr);
                                DatePtr rejDate = date_init(datestr);
                                Rlist_insertItem(RList, 0, rejDate, countryName, virusName);
                            }
                            else
                            {
                                writepipes[monitor_index] = open(writep[monitor_index], O_WRONLY);
                                if (bufferSize >= 4)
                                    write(writepipes[monitor_index], &command, bufferSize);
                                else
                                {
                                    short lastHalf = command & 0xFFFF;
                                    short firstHalf = command >> 16;
                                    write(writepipes[monitor_index], &lastHalf, bufferSize);
                                    write(writepipes[monitor_index], &firstHalf, bufferSize);
                                }

                                //Sending citizen id
                                char *str = malloc(sizeof(char) * strlen(data[1]));
                                strcpy(str, data[1]);
                                float n_f = ((float)strlen(str) + 1.0) / (float)bufferSize;
                                unsigned int n = ceil(n_f);
                                char **ss = divideStr(str, bufferSize, n);
                                if (bufferSize >= 4)
                                    write(writepipes[currNode->monitorIndex], &n, bufferSize);
                                else
                                {
                                    short lastHalf = n & 0xFFFF;
                                    short firstHalf = n >> 16;
                                    write(writepipes[currNode->monitorIndex], &lastHalf, bufferSize);
                                    write(writepipes[currNode->monitorIndex], &firstHalf, bufferSize);
                                }
                                for (int j = 0; j < n; j++)
                                {
                                    write(writepipes[currNode->monitorIndex], ss[j], bufferSize);
                                }
                                for (int i = 0; i < n; i++)
                                {
                                    free(ss[i]);
                                }
                                free(ss);
                                free(str);

                                //Sending dates
                                str = malloc(sizeof(char) * strlen(dateStr));
                                strcpy(str, dateStr);
                                n_f = ((float)strlen(str) + 1.0) / (float)bufferSize;
                                n = ceil(n_f);
                                ss = divideStr(str, bufferSize, n);
                                if (bufferSize >= 4)
                                    write(writepipes[currNode->monitorIndex], &n, bufferSize);
                                else
                                {
                                    short lastHalf = n & 0xFFFF;
                                    short firstHalf = n >> 16;
                                    write(writepipes[currNode->monitorIndex], &lastHalf, bufferSize);
                                    write(writepipes[currNode->monitorIndex], &firstHalf, bufferSize);
                                }
                                for (int j = 0; j < n; j++)
                                {
                                    write(writepipes[currNode->monitorIndex], ss[j], bufferSize);
                                }
                                for (int i = 0; i < n; i++)
                                {
                                    free(ss[i]);
                                }
                                free(ss);
                                free(str);

                                //Sending virusName
                                str = malloc(sizeof(char) * strlen(data[5]));
                                strcpy(str, data[5]);
                                n_f = ((float)strlen(str) + 1.0) / (float)bufferSize;
                                n = ceil(n_f);
                                ss = divideStr(str, bufferSize, n);
                                if (bufferSize >= 4)
                                    write(writepipes[currNode->monitorIndex], &n, bufferSize);
                                else
                                {
                                    short lastHalf = n & 0xFFFF;
                                    short firstHalf = n >> 16;
                                    write(writepipes[currNode->monitorIndex], &lastHalf, bufferSize);
                                    write(writepipes[currNode->monitorIndex], &firstHalf, bufferSize);
                                }
                                for (int j = 0; j < n; j++)
                                {
                                    write(writepipes[currNode->monitorIndex], ss[j], bufferSize);
                                }
                                for (int i = 0; i < n; i++)
                                {
                                    free(ss[i]);
                                }
                                free(ss);
                                free(str);

                                close(writepipes[monitor_index]);

                                //Reading message
                                char message[5];
                                readpipes[monitor_index] = open(readp[monitor_index], O_RDONLY);
                                n = 0;
                                if (bufferSize >= 4)
                                    read(readpipes[monitor_index], &n, bufferSize);
                                else
                                {
                                    short firsthalf, lasthalf;
                                    read(readpipes[monitor_index], &lasthalf, bufferSize);
                                    read(readpipes[monitor_index], &firsthalf, bufferSize);
                                    n = (firsthalf << 16) | lasthalf;
                                }
                                for (int j = 0; j < n; j++)
                                {
                                    read(readpipes[monitor_index], message + (j * bufferSize), bufferSize);
                                }

                                char dateOfVaccination[12];
                                if (!strcmp(message, "YES"))
                                {
                                    n = 0;
                                    if (bufferSize >= 4)
                                        read(readpipes[monitor_index], &n, bufferSize);
                                    else
                                    {
                                        short firsthalf, lasthalf;
                                        read(readpipes[monitor_index], &lasthalf, bufferSize);
                                        read(readpipes[monitor_index], &firsthalf, bufferSize);
                                        n = (firsthalf << 16) | lasthalf;
                                    }
                                    for (int j = 0; j < n; j++)
                                    {
                                        read(readpipes[monitor_index], dateOfVaccination + (j * bufferSize), bufferSize);
                                    }
                                }
                                close(readpipes[monitor_index]);
                                if (!strcmp(message, "YES"))
                                {
                                    DatePtr dateVac = date_init(dateOfVaccination);

                                    if (check_dates(dateVac, date))
                                    {
                                        printf("REQUEST ACCEPTED – HAPPY TRAVELS\n");
                                        char datestr[12];
                                        strcpy(datestr, dateStr);
                                        DatePtr accDate = date_init(datestr);
                                        Rlist_insertItem(RList, 0, accDate, countryName, virusName);
                                    }
                                    else
                                    {
                                        printf("REQUEST REJECTED – YOU WILL NEED ANOTHER VACCINATIONBEFORE TRAVEL DATE\n");
                                        char datestr[12];
                                        strcpy(datestr, dateStr);
                                        DatePtr rejDate = date_init(datestr);
                                        Rlist_insertItem(RList, 0, rejDate, countryName, virusName);
                                    }
                                }
                                else
                                {
                                    printf("REQUEST REJECTED – YOU ARE NOT VACCINATED\n");
                                    char datestr[12];
                                    strcpy(datestr, dateStr);
                                    DatePtr rejDate = date_init(datestr);
                                    Rlist_insertItem(RList, 0, rejDate, countryName, virusName);
                                }
                            }
                        }
                        else
                        {
                            printf("ERROR! Country does not exist!\n");
                        }
                    }
                    else
                        printf("ERROR! Date is invalid please enter a valid date!\n");
                    dateDestructor(date);
                }
                else
                    printf("ERROR! Citizen id is out of limits\n");
            }
            else
                printf("ERROR! Wrong number of arguments!\n");
        }
        else if (!strcmp(data[0], "/travelStats"))
        {
            command = 2;
            //Checking if the dates are valid
            DatePtr date1 = date_init(data[2]);
            DatePtr date2 = date_init(data[3]);
            if (date1->day > 0 && date1->day <= 30 && date1->month > 0 && date1->month <= 12 && date1->year > 1900 && date1->year < 3000)
                if (date2->day > 0 && date2->day <= 30 && date2->month > 0 && date2->month <= 12 && date2->year > 1900 && date2->year < 3000)
                {
                    if (numOfArguments == 4)
                    {
                        int accReq = 0, rejReq = 0;
                        requestNodePtr currnode = RList->Begining;
                        while (currnode != NULL)
                        {
                            if (datecmp(currnode->date, date1) > 0 && datecmp(currnode->date, date2) < 0)
                            {
                                if (currnode->typeOfRequest)
                                {
                                    accReq++;
                                }
                                else
                                {
                                    rejReq++;
                                }
                            }
                            currnode = currnode->nextNode;
                        }
                        printf("TOTAL REQUESTS %d\n", accReq + rejReq);
                        printf("ACCEPTED %d\n", accReq);
                        printf("REJECTED %d\n", rejReq);
                    }
                    else if (numOfArguments == 5)
                    {
                        int accReq = 0, rejReq = 0;
                        requestNodePtr currnode = RList->Begining;
                        while (currnode != NULL)
                        {
                            if (!strcmp(currnode->countryName, data[4]) && !strcmp(currnode->virusName, data[1]))
                            {
                                if (datecmp(currnode->date, date1) > 0 && datecmp(currnode->date, date2) < 0)
                                {
                                    if (currnode->typeOfRequest)
                                    {
                                        accReq++;
                                    }
                                    else
                                    {
                                        rejReq++;
                                    }
                                }
                            }
                            currnode = currnode->nextNode;
                        }
                        printf("TOTAL REQUESTS %d\n", accReq + rejReq);
                        printf("ACCEPTED %d\n", accReq);
                        printf("REJECTED %d\n", rejReq);
                    }
                    else
                        printf("ERROR! Wrong number of arguments!\n");
                }
                else
                    printf("ERROR! First date is invalid please enter a valid date!\n");
            else
                printf("ERROR! Second date is invalid please enter a valid date!\n");
            dateDestructor(date1);
            dateDestructor(date2);
        }
        else if (!strcmp(data[0], "/addVaccinationRecords"))
        {
            currNode = CList->FirstNode;
            monitor_index = -1;
            while (currNode != NULL)
            {
                if (!strcmp(currNode->countryName, data[1]))
                {
                    monitor_index = currNode->monitorIndex;
                    break;
                }
                currNode = currNode->nextNode;
            }
            if (monitor_index >= 0)
            {
                kill(pid[monitor_index], SIGUSR1);
                command = 32;
                writepipes[monitor_index] = open(writep[monitor_index], O_WRONLY);
                if (bufferSize >= 4)
                    write(writepipes[monitor_index], &command, bufferSize);
                else
                {
                    short lastHalf = command & 0xFFFF;
                    short firstHalf = command >> 16;
                    write(writepipes[monitor_index], &lastHalf, bufferSize);
                    write(writepipes[monitor_index], &firstHalf, bufferSize);
                }
                close(writepipes[monitor_index]);
            }
            else
                printf("There is no country with name %s\n", data[1]);
            sleep(1);
        }
        else if (!strcmp(data[0], "/searchVaccinationStatus"))
        {
            command = 4;
            //Checking if citizen id is within the limits
            if (atoi(data[1]) > 0 && atoi(data[1]) < 10000)
            {
                unsigned message = 0;
                for (monitor_index = 0; monitor_index < numMonitors; monitor_index++)
                {
                    writepipes[monitor_index] = open(writep[monitor_index], O_WRONLY);
                    if (bufferSize >= 4)
                        write(writepipes[monitor_index], &command, bufferSize);
                    else
                    {
                        short lastHalf = command & 0xFFFF;
                        short firstHalf = command >> 16;
                        write(writepipes[monitor_index], &lastHalf, bufferSize);
                        write(writepipes[monitor_index], &firstHalf, bufferSize);
                    }
                    //Sending citizen id to all monitors
                    char *str = malloc(sizeof(char) * strlen(data[1]));
                    strcpy(str, data[1]);
                    float n_f = ((float)strlen(str) + 1.0) / (float)bufferSize;
                    unsigned n = ceil(n_f);
                    char **ss = divideStr(str, bufferSize, n);
                    if (bufferSize >= 4)
                        write(writepipes[monitor_index], &n, bufferSize);
                    else
                    {
                        short lastHalf = n & 0xFFFF;
                        short firstHalf = n >> 16;
                        write(writepipes[monitor_index], &lastHalf, bufferSize);
                        write(writepipes[monitor_index], &firstHalf, bufferSize);
                    }
                    for (int j = 0; j < n; j++)
                    {
                        write(writepipes[monitor_index], ss[j], bufferSize);
                    }
                    close(writepipes[monitor_index]);
                    for (int i = 0; i < n; i++)
                    {
                        free(ss[i]);
                    }
                    free(ss);
                    free(str);

                    readpipes[monitor_index] = open(readp[monitor_index], O_RDONLY);
                    if (bufferSize >= 4)
                        read(readpipes[monitor_index], &message, bufferSize);
                    else
                    {
                        short firsthalf, lasthalf;
                        read(readpipes[monitor_index], &lasthalf, bufferSize);
                        read(readpipes[monitor_index], &firsthalf, bufferSize);
                        message = (firsthalf << 16) | lasthalf;
                    }
                    close(readpipes[monitor_index]);
                    if (message)
                        break;
                }
                if (message)
                {
                    readpipes[monitor_index] = open(readp[monitor_index], O_RDONLY);

                    //Reading name
                    char name[12];
                    unsigned n = 0;
                    if (bufferSize >= 4)
                        read(readpipes[monitor_index], &n, bufferSize);
                    else
                    {
                        short firsthalf, lasthalf;
                        read(readpipes[monitor_index], &lasthalf, bufferSize);
                        read(readpipes[monitor_index], &firsthalf, bufferSize);
                        n = (firsthalf << 16) | lasthalf;
                    }
                    for (int j = 0; j < n; j++)
                    {
                        read(readpipes[monitor_index], name + (j * bufferSize), bufferSize);
                    }
                    //Reading surname
                    char surname[12];
                    n = 0;
                    if (bufferSize >= 4)
                        read(readpipes[monitor_index], &n, bufferSize);
                    else
                    {
                        short firsthalf, lasthalf;
                        read(readpipes[monitor_index], &lasthalf, bufferSize);
                        read(readpipes[monitor_index], &firsthalf, bufferSize);
                        n = (firsthalf << 16) | lasthalf;
                    }
                    for (int j = 0; j < n; j++)
                    {
                        read(readpipes[monitor_index], surname + (j * bufferSize), bufferSize);
                    }
                    //Reading age
                    unsigned age;
                    if (bufferSize >= 4)
                        read(readpipes[monitor_index], &age, bufferSize);
                    else
                    {
                        short firsthalf, lasthalf;
                        read(readpipes[monitor_index], &lasthalf, bufferSize);
                        read(readpipes[monitor_index], &firsthalf, bufferSize);
                        age = (firsthalf << 16) | lasthalf;
                    }
                    //Reading country
                    char country[20];
                    n = 0;
                    if (bufferSize >= 4)
                        read(readpipes[monitor_index], &n, bufferSize);
                    else
                    {
                        short firsthalf, lasthalf;
                        read(readpipes[monitor_index], &lasthalf, bufferSize);
                        read(readpipes[monitor_index], &firsthalf, bufferSize);
                        n = (firsthalf << 16) | lasthalf;
                    }
                    for (int j = 0; j < n; j++)
                    {
                        read(readpipes[monitor_index], country + (j * bufferSize), bufferSize);
                    }

                    printf("%s %s %s %s\n", data[1], name, surname, country);
                    printf("AGE %d\n", age);
                    //Reading number of viruses
                    unsigned numOfViruses;
                    if (bufferSize >= 4)
                        read(readpipes[monitor_index], &numOfViruses, bufferSize);
                    else
                    {
                        short firsthalf, lasthalf;
                        read(readpipes[monitor_index], &lasthalf, bufferSize);
                        read(readpipes[monitor_index], &firsthalf, bufferSize);
                        numOfViruses = (firsthalf << 16) | lasthalf;
                    }

                    for (int i = 0; i < numOfViruses; i++)
                    {
                        char virusName[20];
                        n = 0;
                        if (bufferSize >= 4)
                            read(readpipes[monitor_index], &n, bufferSize);
                        else
                        {
                            short firsthalf, lasthalf;
                            read(readpipes[monitor_index], &lasthalf, bufferSize);
                            read(readpipes[monitor_index], &firsthalf, bufferSize);
                            n = (firsthalf << 16) | lasthalf;
                        }
                        for (int j = 0; j < n; j++)
                        {
                            read(readpipes[monitor_index], virusName + (j * bufferSize), bufferSize);
                        }
                        printf("%s ", virusName);
                        char yesno[4];
                        n = 0;
                        if (bufferSize >= 4)
                            read(readpipes[monitor_index], &n, bufferSize);
                        else
                        {
                            short firsthalf, lasthalf;
                            read(readpipes[monitor_index], &lasthalf, bufferSize);
                            read(readpipes[monitor_index], &firsthalf, bufferSize);
                            n = (firsthalf << 16) | lasthalf;
                        }
                        for (int j = 0; j < n; j++)
                        {
                            read(readpipes[monitor_index], yesno + (j * bufferSize), bufferSize);
                        }
                        if (!strcmp(yesno, "YES"))
                        {
                            char date[12];
                            if (bufferSize >= 4)
                                read(readpipes[monitor_index], &n, bufferSize);
                            else
                            {
                                short firsthalf, lasthalf;
                                read(readpipes[monitor_index], &lasthalf, bufferSize);
                                read(readpipes[monitor_index], &firsthalf, bufferSize);
                                n = (firsthalf << 16) | lasthalf;
                            }
                            for (int j = 0; j < n; j++)
                            {
                                read(readpipes[monitor_index], date + (j * bufferSize), bufferSize);
                            }
                            printf("VACCINATED ON %s\n", date);
                        }
                        else
                        {
                            printf("NOT YET VACCINATED\n");
                        }
                    }

                    close(readpipes[monitor_index]);
                }
                else
                    printf("This citizen id does not exist: %s", data[1]);
            }
            else
                printf("ERROR! Citizen id is out of limits\n");
        }

        if (flag_intsig || flag_quitsig)
        {
            flag_quitsig = 0;
            flag_intsig = 0;
            break;
        }

    } while (strcmp(data[0], "/exit"));

    command = 0;
    //Destruction of data structures and freeing of memory
    for (monitor_index = 0; monitor_index < numMonitors; monitor_index++)
    {
        writepipes[monitor_index] = open(writep[monitor_index], O_WRONLY);
        if (bufferSize >= 4)
            write(writepipes[monitor_index], &command, bufferSize);
        else
        {
            short lastHalf = command & 0xFFFF;
            short firstHalf = command >> 16;
            write(writepipes[monitor_index], &lastHalf, bufferSize);
            write(writepipes[monitor_index], &firstHalf, bufferSize);
        }
        bloom_destructor(bloomMonitorArray[monitor_index]);
        free(readp[monitor_index]);
        free(writep[monitor_index]);
        close(writepipes[monitor_index]);

        //killing children
        flag_chldsig = 1;

        char chldLogFileNamePath[30];

        kill(pid[monitor_index], SIGKILL);
        while (flag_chldsig)
            ;
    }
    //Logfile creation
    char logfilename[25];
    sprintf(logfilename, "log_files/log_file.%d", getpid());
    printf("logfile: %s\n", logfilename);
    FILE *log_file = fopen(logfilename, "w");
    currNode = CList->FirstNode;
    while (currNode != NULL)
    {
        fprintf(log_file, "%s\n", currNode->countryName);
        currNode = currNode->nextNode;
    }
    int accReq = 0, rejReq = 0;
    requestNodePtr currnode = RList->Begining;
    while (currnode != NULL)
    {
        if (currnode->typeOfRequest)
        {
            accReq++;
        }
        else
        {
            rejReq++;
        }
        currnode = currnode->nextNode;
    }
    fprintf(log_file, "TOTAL TRAVEL REQUESTS %d\nACCEPTED %d\nREJECTED %d\n", accReq + rejReq, accReq, rejReq);
    fclose(log_file);

    //Closing data bases
    free(arguments[0]);
    free(arguments[1]);
    free(arguments);
    free(numOfCountriesPerMonitor);
    CountryRefList_Delete(CList);
    free(writep);
    free(readp);
    free(input_dir);
    free(bloomSize);
    return 0;
}