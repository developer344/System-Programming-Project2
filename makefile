CC = gcc
CCFLAGS = -lm 
FLAGS = -g
INCLUDE = -I ./include
SRC = ./src
BIN = ./bin

all: main1 main2

main1: $(SRC)/travelMonitor.c
	$(CC) $(INCLUDE) -o $(BIN)/travelMonitor $(SRC)/travelMonitor.c $(SRC)/bloomfilter.c $(SRC)/CountryReferenceList.c $(SRC)/date.c $(SRC)/requests.c $(CCFLAGS)

main2: $(SRC)/monitor.c
	$(CC) $(INCLUDE) -o $(BIN)/monitor $(SRC)/monitor.c $(SRC)/bloomfilter.c $(SRC)/BST.c $(SRC)/date.c $(SRC)/countryTree.c $(SRC)/linkedList.c $(SRC)/skipList.c $(SRC)/Virus.c $(CCFLAGS)

clean:
	rm $(BIN)/travelMonitor $(BIN)/monitor
