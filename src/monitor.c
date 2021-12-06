#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>

#include "linkedList.h"
#include "BST.h"

unsigned acceptedRequests = 0;
unsigned rejectedRequests = 0;

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

unsigned flag_usr1sig = 0;
unsigned flag_intsig = 0;
unsigned flag_quitsig = 0;

void sigusr1_handler(int sig)
{
    flag_usr1sig = 1;
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
    signal(SIGUSR1, &sigusr1_handler);
    signal(SIGINT, &sigint_handler);
    signal(SIGQUIT, &sigquit_handler);
    int bufferSize;
    //Reading the size of the buffer
    for (int i = 0; i < argc - 1; i++)
        if (!strcmp(argv[i], "-b"))
        {
            bufferSize = atoi(argv[i + 1]);
            break;
        }
    unsigned bloomsize;
    //Reading the number of bytes that the bloomfilter bit array will have
    for (int i = 0; i < argc - 1; i++)
        if (!strcmp(argv[i], "-s"))
        {
            //Multipluing by 8 to convert from bytes to bits
            bloomsize = 8 * (atoi(argv[i + 1]));
            break;
        }
    char *writepipe = malloc(sizeof(char) * strlen(argv[1]));
    char *readpipe = malloc(sizeof(char) * strlen(argv[2]));
    strcpy(writepipe, argv[1]);
    strcpy(readpipe, argv[2]);
    char buffer[30];
    int readp = open(readpipe, O_RDONLY);

    unsigned numberOfPaths = 0;

    if (bufferSize >= 4)
        read(readp, &numberOfPaths, bufferSize);
    else
    {
        short firsthalf, lasthalf;
        read(readp, &lasthalf, bufferSize);
        read(readp, &firsthalf, bufferSize);
        numberOfPaths = (firsthalf << 16) | lasthalf;
    }
    char path[numberOfPaths][20];
    unsigned n = 0;
    for (int i = 0; i < numberOfPaths; i++)
    {
        n = 0;
        if (bufferSize >= 4)
            read(readp, &n, bufferSize);
        else
        {
            short firsthalf, lasthalf;
            read(readp, &lasthalf, bufferSize);
            read(readp, &firsthalf, bufferSize);
            n = (firsthalf << 16) | lasthalf;
        }
        for (int j = 0; j < n; j++)
        {
            read(readp, path[i] + (j * bufferSize), bufferSize);
        }
    }
    close(readp);
    //-------------------------------------------------------------------------------------//
    CTPtr countryTree = malloc(sizeof(CT));
    countryTree_init(countryTree);

    BSTPtr citizenTree = malloc(sizeof(BST));
    BST_init(citizenTree);

    linkedListPtr virusList = malloc(sizeof(linkedList));
    list_init(virusList);

    virusPtr currVirus;
    citizenRecordPtr record;
    citizenRecordPtr duplicate;
    cTreeNodePtr countryNode;
    bloomFilterPtr monitorBloom = malloc(sizeof(bloomFilter));
    bloomFilter_init(monitorBloom, NUMOFHASHFUNCTIONS, bloomsize);
    int numOfFiles = 0;
    for (int pathIndex = 0; pathIndex < numberOfPaths; pathIndex++)
    {
        struct dirent *in_file;
        DIR *input_dir = opendir(path[pathIndex]);
        while ((in_file = readdir(input_dir)) != NULL)
        {
            if (!strcmp(in_file->d_name, "."))
                continue;
            if (!strcmp(in_file->d_name, ".."))
                continue;
            numOfFiles++;
            char *pathToNCountry = malloc(sizeof(char) * (strlen(in_file->d_name) + strlen(path[pathIndex] + 1)));
            strcpy(pathToNCountry, path[pathIndex]);
            strcat(pathToNCountry, "/");
            strcat(pathToNCountry, in_file->d_name);
            printf("This is monitor: %s Opening file:: %s and full path: %s\n", argv[0], in_file->d_name, pathToNCountry);
            FILE *inputFile = NULL;
            inputFile = fopen(pathToNCountry, "r");

            if (inputFile == NULL)
            {
                printf("errno = %d\n", errno);
                perror("fopen");
            }
            //This will hold the data from its line that is read
            char line[100];
            //This will hold the data from its line that is read separetly
            char data[10][30];
            //To be used on the separation of the line inyo its compoonents
            char *key;

            //------------------------------------Reading File------------------------------------//
            while (fgets(line, sizeof(line), inputFile))
            {
                //Reading line by line from the file given
                line[strcspn(line, "\n")] = 0;
                //Separating the lines into the usefull info
                key = strtok(line, " ");
                for (int i = 0; key != NULL; i++)
                {
                    strcpy(data[i], key);
                    key = strtok(NULL, " ");
                }
                //Checking if citizen id is within the limits
                if (atoi(data[0]) > 0 && atoi(data[0]) < 10000)
                {
                    //Checking if age is valid
                    if (atoi(data[4]) > 0)
                    {
                        //Checking  if date is valid
                        char dateStr[10];
                        strcpy(dateStr, data[7]);
                        DatePtr date = date_init(data[7]);
                        if ((date->day > 0 && date->day <= 30 && date->month > 0 && date->month <= 12 && date->year > 1900 && date->year < 3000 && !strcmp(data[6], "YES")) || !strcmp(data[6], "NO"))
                            //Checking if a citizen is already in the citizen tree
                            if ((duplicate = findcitizenRecord(citizenTree, atoi(data[0]))) == NULL)
                            {
                                //Here I check if the country of the current citizen has been added to the country
                                //tree if not I create a new node in the country tree for the new country
                                if ((countryNode = countryTree_findCountry(countryTree, data[3], atoi(data[4]))) == NULL)
                                    countryNode = countryTree_insertCountry(countryTree, data[3], atoi(data[4]));
                                //Creating new citizen and inserting the unique elements only so
                                //that country, virus, date and yes/no will not be duplicated
                                record = malloc(sizeof(citizenRecord));
                                record->citizenId = atoi(data[0]);
                                record->name = malloc(sizeof(char) * strlen(data[1]));
                                strcpy(record->name, data[1]);
                                record->surname = malloc(sizeof(char) * strlen(data[2]));
                                strcpy(record->surname, data[2]);
                                //For the country each citizen has a pointer to their country
                                //Citizens from the same country point to the same country node
                                record->country = countryNode;
                                record->age = atoi(data[4]);
                                //Inserting the new citizen to the tree that contains all the citizens of every country
                                if (insertcitizenRecord(citizenTree, record) < 0)
                                {
                                    printf("Monitor: %s Error: couldn't insert record in citizen tree\n", argv[0]);
                                }
                                //I search in the list of viruses if the virus insetred already exists or not
                                if ((currVirus = list_searchElement(virusList, data[5])) != NULL)
                                {
                                    //Then I insert the citizen to the virus
                                    virus_insert(currVirus, record, data[0], data[6], dateStr);
                                    if (!strcmp(data[6], "YES"))
                                    {
                                        bloom_insertElement(monitorBloom, data[0]);
                                    }
                                }
                                else
                                {
                                    //If the inserted virus doesn't exist in the virus list I create it and I insert it in the virus list
                                    currVirus = malloc(sizeof(virus));

                                    virus_init(currVirus, data[5], NUMOFHASHFUNCTIONS, bloomsize);
                                    //I insert the new citizen to the newly created virus
                                    virus_insert(currVirus, record, data[0], data[6], dateStr);
                                    if (!strcmp(data[6], "YES"))
                                    {
                                        bloom_insertElement(monitorBloom, data[0]);
                                    }
                                    //I insert the new virus to the virus list
                                    list_insertItem(virusList, currVirus);
                                }
                            }
                            else
                            {
                                //If the inserted citizen already exists in the citizen tree then I take the citizen
                                //and insert them in the virus that was given
                                if ((currVirus = list_searchElement(virusList, data[5])) != NULL)
                                {
                                    //If the virus given already exists then I check if the citizen has already has been
                                    //inserted in thatvirus
                                    if (find_inVirus(currVirus, duplicate->citizenId) > 0)
                                    {
                                        //If the citizen is not in that virus I insert them there
                                        virus_insert(currVirus, duplicate, data[0], data[6], dateStr);
                                        if (!strcmp(data[6], "YES"))
                                        {
                                            bloom_insertElement(monitorBloom, data[0]);
                                        }
                                    }
                                    else
                                        //If the citizen is already in the virus I print error
                                        printf("Monitor: %s ERROR IN RECORD %s\n", argv[0], line);
                                }
                                else
                                {
                                    //If the virus given doesn't exist I create it
                                    currVirus = malloc(sizeof(virus));
                                    virus_init(currVirus, data[5], NUMOFHASHFUNCTIONS, bloomsize);
                                    //I insert the citizen to the newly created virus
                                    virus_insert(currVirus, duplicate, data[0], data[6], dateStr);
                                    if (!strcmp(data[6], "YES"))
                                    {
                                        bloom_insertElement(monitorBloom, data[0]);
                                    }
                                    //I insert the new virus to the virus list
                                    list_insertItem(virusList, currVirus);
                                }
                            }
                        else
                            printf("Monitor: %s ERROR! Date is invalid please enter a valid date in record %s\n", argv[0], line);
                        dateDestructor(date);
                    }
                    else
                        printf("Monitor: %s ERROR! Age is a negatice number in record %s\n", argv[0], line);
                }
                else
                    printf("Monitor: %s ERROR! Citizen id is out of limits in record %s\n", argv[0], line);
            }
            fclose(inputFile);
            free(pathToNCountry);
            free(key);
        }
    }
    char *filesArray[numOfFiles];
    int index = 0;
    for (int pathIndex = 0; pathIndex < numberOfPaths; pathIndex++)
    {
        struct dirent *in_file;
        DIR *input_dir = opendir(path[pathIndex]);
        while ((in_file = readdir(input_dir)) != NULL)
        {
            if (!strcmp(in_file->d_name, "."))
                continue;
            if (!strcmp(in_file->d_name, ".."))
                continue;
            filesArray[index] = malloc(sizeof(char) * strlen(in_file->d_name));
            strcpy(filesArray[index], in_file->d_name);
            index++;
        }
    }
    int writep = open(writepipe, O_WRONLY);
    //Sending BloomFilter
    n = ((bloomsize / 8) + (bloomsize % 8 > 0 ? 1 : 0));
    for (int j = 0; j < n; j++)
    {
        write(writep, monitorBloom->bloomBitArray + j, bufferSize);
    }
    close(writep);

    //Application
    unsigned endState = 1;
    unsigned command = 0;

    slNodePtr Node;
    while (endState > 0)
    {
        readp = open(readpipe, O_RDONLY);
        if (bufferSize >= 4)
            read(readp, &command, bufferSize);
        else
        {
            short firsthalf, lasthalf;
            read(readp, &lasthalf, bufferSize);
            read(readp, &firsthalf, bufferSize);
            command = (firsthalf << 16) | lasthalf;
        }
        if (flag_usr1sig)
        {
            command = 3;
            flag_usr1sig = 0;
        }
        if (command == 1)
        {
            //Reading citizen id
            char citizenId[5];
            n = 0;
            if (bufferSize >= 4)
                read(readp, &n, bufferSize);
            else
            {
                short firsthalf, lasthalf;
                read(readp, &lasthalf, bufferSize);
                read(readp, &firsthalf, bufferSize);
                n = (firsthalf << 16) | lasthalf;
            }
            for (int j = 0; j < n; j++)
            {
                read(readp, citizenId + (j * bufferSize), bufferSize);
            }

            //Reading date
            char date[12];
            n = 0;
            if (bufferSize >= 4)
                read(readp, &n, bufferSize);
            else
            {
                short firsthalf, lasthalf;
                read(readp, &lasthalf, bufferSize);
                read(readp, &firsthalf, bufferSize);
                n = (firsthalf << 16) | lasthalf;
            }
            for (int j = 0; j < n; j++)
            {
                read(readp, date + (j * bufferSize), bufferSize);
            }

            //Reading virusName
            char virusName[20];
            n = 0;
            if (bufferSize >= 4)
                read(readp, &n, bufferSize);
            else
            {
                short firsthalf, lasthalf;
                read(readp, &lasthalf, bufferSize);
                read(readp, &firsthalf, bufferSize);
                n = (firsthalf << 16) | lasthalf;
            }
            for (int j = 0; j < n; j++)
            {
                read(readp, virusName + (j * bufferSize), bufferSize);
            }
            printf("Command 1\nCitizen id: %s\nDate: %s\nVirus Name: %s\n", citizenId, date, virusName);
            char message[5];
            if ((currVirus = list_searchElement(virusList, virusName)) != NULL)
            {
                if (virus_checkIfVaccinatedBloom(currVirus, citizenId))
                {
                    if ((Node = skipList_findRecord(currVirus->vaccinatedVirusSL, atoi(citizenId))) != NULL)
                    {
                        strcpy(message, "YES");
                        sprintf(date, "%d-%d-%d", Node->date->day, Node->date->month, Node->date->year);
                        printf("Date Of vaccination: %s", date);
                        acceptedRequests++;
                    }
                    else
                    {
                        strcpy(message, "NO");
                        rejectedRequests++;
                    }
                }
                else
                {
                    strcpy(message, "NO");
                    rejectedRequests++;
                }
            }
            else
            {
                strcpy(message, "NO");
                rejectedRequests++;
            }

            int writep = open(writepipe, O_WRONLY);

            //Sending back answer
            char *str = malloc(sizeof(char) * strlen(message));
            strcpy(str, message);
            float n_f = ((float)strlen(str) + 1.0) / (float)bufferSize;
            n = ceil(n_f);
            char **ss = divideStr(str, bufferSize, n);
            if (bufferSize >= 4)
                write(writep, &n, bufferSize);
            else
            {
                short qq = n & 0xFFFF;
                short ll = n >> 16;
                write(writep, &qq, bufferSize);
                write(writep, &ll, bufferSize);
            }
            for (int j = 0; j < n; j++)
            {
                //printf("Sending: %s\n", ss[j]);
                write(writep, ss[j], bufferSize);
            }
            for (int i = 0; i < n; i++)
            {
                free(ss[i]);
            }
            free(ss);
            free(str);

            //Sending date if message is YES
            if (!strcmp(message, "YES"))
            {
                str = malloc(sizeof(char) * strlen(date));
                strcpy(str, date);
                n_f = ((float)strlen(str) + 1.0) / (float)bufferSize;
                n = ceil(n_f);
                ss = divideStr(str, bufferSize, n);
                if (bufferSize >= 4)
                    write(writep, &n, bufferSize);
                else
                {
                    short qq = n & 0xFFFF;
                    short ll = n >> 16;
                    write(writep, &qq, bufferSize);
                    write(writep, &ll, bufferSize);
                }
                for (int j = 0; j < n; j++)
                {
                    //printf("Sending: %s\n", ss[j]);
                    write(writep, ss[j], bufferSize);
                }
                for (int i = 0; i < n; i++)
                {
                    free(ss[i]);
                }
                free(ss);
                free(str);
            }
            close(writep);
        }
        else if (command == 3)
        {
            int i;
            for (int pathIndex = 0; pathIndex < numberOfPaths; pathIndex++)
            {
                struct dirent *in_file;
                DIR *input_dir = opendir(path[pathIndex]);
                while ((in_file = readdir(input_dir)) != NULL)
                {
                    if (!strcmp(in_file->d_name, "."))
                        continue;
                    if (!strcmp(in_file->d_name, ".."))
                        continue;

                    for (i = 0; i < index; i++)
                    {
                        if (!strcmp(filesArray[i], in_file->d_name))
                        {
                            break;
                        }
                    }
                    if (i >= index)
                    {
                        char *pathToNCountry = malloc(sizeof(char) * (strlen(in_file->d_name) + strlen(path[pathIndex] + 1)));
                        strcpy(pathToNCountry, path[pathIndex]);
                        strcat(pathToNCountry, "/");
                        strcat(pathToNCountry, in_file->d_name);
                        printf("This is monitor: %s Opening file:: %s and full path: %s\n", argv[0], in_file->d_name, pathToNCountry);
                        FILE *inputFile = NULL;
                        inputFile = fopen(pathToNCountry, "r");

                        if (inputFile == NULL)
                        {
                            printf("errno = %d\n", errno);
                            perror("fopen");
                        }
                        //This will hold the data from its line that is read
                        char line[100];
                        //This will hold the data from its line that is read separetly
                        char data[10][30];
                        //To be used on the separation of the line inyo its compoonents
                        char *key;

                        //------------------------------------Reading File------------------------------------//
                        while (fgets(line, sizeof(line), inputFile))
                        {
                            //Reading line by line from the file given
                            line[strcspn(line, "\n")] = 0;
                            //Separating the lines into the usefull info
                            key = strtok(line, " ");
                            for (int i = 0; key != NULL; i++)
                            {
                                strcpy(data[i], key);
                                key = strtok(NULL, " ");
                            }
                            //Checking if citizen id is within the limits
                            if (atoi(data[0]) > 0 && atoi(data[0]) < 10000)
                            {
                                //Checking if age is valid
                                if (atoi(data[4]) > 0)
                                {
                                    //Checking  if date is valid
                                    char dateStr[10];
                                    strcpy(dateStr, data[7]);
                                    DatePtr date = date_init(data[7]);
                                    if ((date->day > 0 && date->day <= 30 && date->month > 0 && date->month <= 12 && date->year > 1900 && date->year < 3000 && !strcmp(data[6], "YES")) || !strcmp(data[6], "NO"))
                                        //Checking if a citizen is already in the citizen tree
                                        if ((duplicate = findcitizenRecord(citizenTree, atoi(data[0]))) == NULL)
                                        {
                                            //Here I check if the country of the current citizen has been added to the country
                                            //tree if not I create a new node in the country tree for the new country
                                            if ((countryNode = countryTree_findCountry(countryTree, data[3], atoi(data[4]))) == NULL)
                                                countryNode = countryTree_insertCountry(countryTree, data[3], atoi(data[4]));
                                            //Creating new citizen and inserting the unique elements only so
                                            //that country, virus, date and yes/no will not be duplicated
                                            record = malloc(sizeof(citizenRecord));
                                            record->citizenId = atoi(data[0]);
                                            record->name = malloc(sizeof(char) * strlen(data[1]));
                                            strcpy(record->name, data[1]);
                                            record->surname = malloc(sizeof(char) * strlen(data[2]));
                                            strcpy(record->surname, data[2]);
                                            //For the country each citizen has a pointer to their country
                                            //Citizens from the same country point to the same country node
                                            record->country = countryNode;
                                            record->age = atoi(data[4]);
                                            //Inserting the new citizen to the tree that contains all the citizens of every country
                                            if (insertcitizenRecord(citizenTree, record) < 0)
                                            {
                                                printf("Monitor: %s Error: couldn't insert record in citizen tree\n", argv[0]);
                                            }
                                            //I search in the list of viruses if the virus insetred already exists or not
                                            if ((currVirus = list_searchElement(virusList, data[5])) != NULL)
                                            {
                                                //Then I insert the citizen to the virus
                                                virus_insert(currVirus, record, data[0], data[6], dateStr);
                                                if (!strcmp(data[6], "YES"))
                                                {
                                                    bloom_insertElement(monitorBloom, data[0]);
                                                }
                                            }
                                            else
                                            {
                                                //If the inserted virus doesn't exist in the virus list I create it and I insert it in the virus list
                                                currVirus = malloc(sizeof(virus));

                                                virus_init(currVirus, data[5], NUMOFHASHFUNCTIONS, bloomsize);
                                                //I insert the new citizen to the newly created virus
                                                virus_insert(currVirus, record, data[0], data[6], dateStr);
                                                if (!strcmp(data[6], "YES"))
                                                {
                                                    bloom_insertElement(monitorBloom, data[0]);
                                                }
                                                //I insert the new virus to the virus list
                                                list_insertItem(virusList, currVirus);
                                            }
                                        }
                                        else
                                        {
                                            //If the inserted citizen already exists in the citizen tree then I take the citizen
                                            //and insert them in the virus that was given
                                            if ((currVirus = list_searchElement(virusList, data[5])) != NULL)
                                            {
                                                //If the virus given already exists then I check if the citizen has already has been
                                                //inserted in thatvirus
                                                if (find_inVirus(currVirus, duplicate->citizenId) > 0)
                                                {
                                                    //If the citizen is not in that virus I insert them there
                                                    virus_insert(currVirus, duplicate, data[0], data[6], dateStr);
                                                    if (!strcmp(data[6], "YES"))
                                                    {
                                                        bloom_insertElement(monitorBloom, data[0]);
                                                    }
                                                }
                                                else
                                                    //If the citizen is already in the virus I print error
                                                    printf("Monitor: %s ERROR IN RECORD %s\n", argv[0], line);
                                            }
                                            else
                                            {
                                                //If the virus given doesn't exist I create it
                                                currVirus = malloc(sizeof(virus));
                                                virus_init(currVirus, data[5], NUMOFHASHFUNCTIONS, bloomsize);
                                                //I insert the citizen to the newly created virus
                                                virus_insert(currVirus, duplicate, data[0], data[6], dateStr);
                                                if (!strcmp(data[6], "YES"))
                                                {
                                                    bloom_insertElement(monitorBloom, data[0]);
                                                }
                                                //I insert the new virus to the virus list
                                                list_insertItem(virusList, currVirus);
                                            }
                                        }
                                    else
                                        printf("Monitor: %s ERROR! Date is invalid please enter a valid date in record %s\n", argv[0], line);
                                    dateDestructor(date);
                                }
                                else
                                    printf("Monitor: %s ERROR! Age is a negatice number in record %s\n", argv[0], line);
                            }
                            else
                                printf("Monitor: %s ERROR! Citizen id is out of limits in record %s\n", argv[0], line);
                        }
                        i = 0;
                        break;
                    }
                    else
                    {
                        i = 1;
                    }
                }
            }
            if (i == 1)
            {
                printf("There is no new file to add\n");
            }
        }
        else if (command == 4)
        {
            //Recieving citizen ID
            char citizenId[5];
            n = 0;
            if (bufferSize >= 4)
                read(readp, &n, bufferSize);
            else
            {
                short firsthalf, lasthalf;
                read(readp, &lasthalf, bufferSize);
                read(readp, &firsthalf, bufferSize);
                n = (firsthalf << 16) | lasthalf;
            }
            for (int j = 0; j < n; j++)
            {
                read(readp, citizenId + (j * bufferSize), bufferSize);
            }
            close(readp);
            writep = open(writepipe, O_WRONLY);
            //Searching if citizen id is in the citizen tree
            unsigned message = 0;
            citizenRecordPtr record = NULL;
            if ((record = findcitizenRecord(citizenTree, atoi(citizenId))) != NULL)
            {
                message = 1;
            }

            //Sending back to the travel monitor the results
            if (bufferSize >= 4)
                write(writep, &message, bufferSize);
            else
            {
                short lastHalf = message & 0xFFFF;
                short firstHalf = message >> 16;
                write(writep, &lastHalf, bufferSize);
                write(writep, &firstHalf, bufferSize);
            }
            close(writep);
            if (message)
            {
                //Sending citizen name

                char *str = malloc(sizeof(char) * strlen(record->name));
                strcpy(str, record->name);
                float n_f = ((float)strlen(str) + 1.0) / (float)bufferSize;
                unsigned n = ceil(n_f);
                char **ss = divideStr(str, bufferSize, n);
                writep = open(writepipe, O_WRONLY);
                if (bufferSize >= 4)
                    write(writep, &n, bufferSize);
                else
                {
                    short lastHalf = n & 0xFFFF;
                    short firstHalf = n >> 16;
                    write(writep, &lastHalf, bufferSize);
                    write(writep, &firstHalf, bufferSize);
                }
                for (int j = 0; j < n; j++)
                {
                    write(writep, ss[j], bufferSize);
                }
                for (int i = 0; i < n; i++)
                {
                    free(ss[i]);
                }
                free(ss);
                free(str);

                //Sending citizen surname
                str = malloc(sizeof(char) * strlen(record->surname));
                strcpy(str, record->surname);
                n_f = ((float)strlen(str) + 1.0) / (float)bufferSize;
                n = ceil(n_f);
                ss = divideStr(str, bufferSize, n);
                if (bufferSize >= 4)
                    write(writep, &n, bufferSize);
                else
                {
                    short lastHalf = n & 0xFFFF;
                    short firstHalf = n >> 16;
                    write(writep, &lastHalf, bufferSize);
                    write(writep, &firstHalf, bufferSize);
                }
                for (int j = 0; j < n; j++)
                {
                    write(writep, ss[j], bufferSize);
                }
                for (int i = 0; i < n; i++)
                {
                    free(ss[i]);
                }
                free(ss);
                free(str);
                //Sending citizen age
                if (bufferSize >= 4)
                    write(writep, &record->age, bufferSize);
                else
                {
                    short lastHalf = record->age & 0xFFFF;
                    short firstHalf = record->age >> 16;
                    write(writep, &lastHalf, bufferSize);
                    write(writep, &firstHalf, bufferSize);
                }

                //Sending citizen country
                str = malloc(sizeof(char) * strlen(record->country->countryName));
                strcpy(str, record->country->countryName);
                n_f = ((float)strlen(str) + 1.0) / (float)bufferSize;
                n = ceil(n_f);
                ss = divideStr(str, bufferSize, n);
                if (bufferSize >= 4)
                    write(writep, &n, bufferSize);
                else
                {
                    short lastHalf = n & 0xFFFF;
                    short firstHalf = n >> 16;
                    write(writep, &lastHalf, bufferSize);
                    write(writep, &firstHalf, bufferSize);
                }
                for (int j = 0; j < n; j++)
                {
                    write(writep, ss[j], bufferSize);
                }
                for (int i = 0; i < n; i++)
                {
                    free(ss[i]);
                }
                free(ss);
                free(str);
                char vacToViruses[virusList->numOfElements][20];
                char dates[virusList->numOfElements][12];
                unsigned vacIndex = 0;
                char nonVacToViruses[virusList->numOfElements][20];
                unsigned nonVacIndex = 0;
                listNodePtr currVi = virusList->Begining;
                while (currVi != NULL)
                {
                    if (find_inVirus(currVi->virus, record->citizenId) < 0)
                    {
                        if ((Node = skipList_findRecord(currVi->virus->vaccinatedVirusSL, record->citizenId)) != NULL)
                        {
                            sprintf(dates[vacIndex], "%d-%d-%d", Node->date->day, Node->date->month, Node->date->year);
                            strcpy(vacToViruses[vacIndex], currVi->virus->name);
                            vacIndex++;
                        }
                        else if (skipList_findRecord(currVi->virus->notVaccinatedVirusSL, record->citizenId) != NULL)
                        {
                            strcpy(nonVacToViruses[nonVacIndex], currVi->virus->name);
                            nonVacIndex++;
                        }
                    }

                    currVi = currVi->nextNode;
                }
                unsigned totalViruses = vacIndex + nonVacIndex;
                if (bufferSize >= 4)
                    write(writep, &totalViruses, bufferSize);
                else
                {
                    short lastHalf = totalViruses & 0xFFFF;
                    short firstHalf = totalViruses >> 16;
                    write(writep, &lastHalf, bufferSize);
                    write(writep, &firstHalf, bufferSize);
                }
                for (int ii = 0; ii < vacIndex; ii++)
                {
                    //Sending virus name
                    str = malloc(sizeof(char) * strlen(vacToViruses[ii]));
                    strcpy(str, vacToViruses[ii]);
                    n_f = ((float)strlen(str) + 1.0) / (float)bufferSize;
                    n = ceil(n_f);
                    ss = divideStr(str, bufferSize, n);
                    if (bufferSize >= 4)
                        write(writep, &n, bufferSize);
                    else
                    {
                        short lastHalf = n & 0xFFFF;
                        short firstHalf = n >> 16;
                        write(writep, &lastHalf, bufferSize);
                        write(writep, &firstHalf, bufferSize);
                    }
                    for (int j = 0; j < n; j++)
                    {
                        write(writep, ss[j], bufferSize);
                    }
                    for (int i = 0; i < n; i++)
                    {
                        free(ss[i]);
                    }
                    free(ss);
                    free(str);

                    //Sending yes vaccination
                    str = malloc(sizeof(char) * 4);
                    strcpy(str, "YES");
                    n_f = ((float)strlen(str) + 1.0) / (float)bufferSize;
                    n = ceil(n_f);
                    ss = divideStr(str, bufferSize, n);
                    if (bufferSize >= 4)
                        write(writep, &n, bufferSize);
                    else
                    {
                        short lastHalf = n & 0xFFFF;
                        short firstHalf = n >> 16;
                        write(writep, &lastHalf, bufferSize);
                        write(writep, &firstHalf, bufferSize);
                    }
                    for (int j = 0; j < n; j++)
                    {
                        write(writep, ss[j], bufferSize);
                    }
                    for (int i = 0; i < n; i++)
                    {
                        free(ss[i]);
                    }
                    free(ss);
                    free(str);

                    //Sending date of vaccination
                    str = malloc(sizeof(char) * 12);
                    strcpy(str, dates[ii]);
                    n_f = ((float)strlen(str) + 1.0) / (float)bufferSize;
                    n = ceil(n_f);
                    ss = divideStr(str, bufferSize, n);
                    if (bufferSize >= 4)
                        write(writep, &n, bufferSize);
                    else
                    {
                        short lastHalf = n & 0xFFFF;
                        short firstHalf = n >> 16;
                        write(writep, &lastHalf, bufferSize);
                        write(writep, &firstHalf, bufferSize);
                    }
                    for (int j = 0; j < n; j++)
                    {
                        write(writep, ss[j], bufferSize);
                    }
                    for (int i = 0; i < n; i++)
                    {
                        free(ss[i]);
                    }
                    free(ss);
                    free(str);
                }
                for (int ii = 0; ii < nonVacIndex; ii++)
                {
                    //Sending virus name

                    str = malloc(sizeof(char) * strlen(nonVacToViruses[ii]));
                    strcpy(str, nonVacToViruses[ii]);
                    n_f = ((float)strlen(str) + 1.0) / (float)bufferSize;
                    n = ceil(n_f);
                    ss = divideStr(str, bufferSize, n);
                    if (bufferSize >= 4)
                        write(writep, &n, bufferSize);
                    else
                    {
                        short lastHalf = n & 0xFFFF;
                        short firstHalf = n >> 16;
                        write(writep, &lastHalf, bufferSize);
                        write(writep, &firstHalf, bufferSize);
                    }
                    for (int j = 0; j < n; j++)
                    {
                        write(writep, ss[j], bufferSize);
                    }
                    for (int i = 0; i < n; i++)
                    {
                        free(ss[i]);
                    }
                    free(ss);
                    free(str);

                    //Sending no vaccination
                    str = malloc(sizeof(char) * 4);
                    strcpy(str, "NO");
                    n_f = ((float)strlen(str) + 1.0) / (float)bufferSize;
                    n = ceil(n_f);
                    ss = divideStr(str, bufferSize, n);
                    if (bufferSize >= 4)
                        write(writep, &n, bufferSize);
                    else
                    {
                        short lastHalf = n & 0xFFFF;
                        short firstHalf = n >> 16;
                        write(writep, &lastHalf, bufferSize);
                        write(writep, &firstHalf, bufferSize);
                    }
                    for (int j = 0; j < n; j++)
                    {
                        write(writep, ss[j], bufferSize);
                    }
                    for (int i = 0; i < n; i++)
                    {
                        free(ss[i]);
                    }
                    free(ss);
                    free(str);
                }
                close(writep);
            }
        }
        else
            close(readp);
        endState = command;
        if (flag_intsig || flag_quitsig)
        {
            //Logfile creation
            char logfilename[25];
            sprintf(logfilename, "log_files/log_file.%d", getpid());
            printf("logfile: %s\n", logfilename);
            FILE *log_file = fopen(logfilename, "w");
            char data[2][20];
            for (int i = 0; i < numberOfPaths; i++)
            {
                char *key = strtok(path[i], "/");
                for (int i = 0; key != NULL; i++)
                {
                    strcpy(data[i], key);
                    key = strtok(NULL, "/");
                }
                fprintf(log_file, "%s\n", data[1]);
            }
            fprintf(log_file, "TOTAL TRAVEL REQUESTS %d\nACCEPTED %d\nREJECTED %d\n", acceptedRequests + rejectedRequests, acceptedRequests, rejectedRequests);
            fclose(log_file);
        }
    }

    countryTree_destructor(countryTree);
    list_deleteList(virusList);
    BST_destructor(citizenTree);
    free(writepipe);
    free(readpipe);
    printf("Ending monitor %s\n", argv[0]);
    return 0;
}