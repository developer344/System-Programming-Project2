#!/bin/bash

function checkCountry() {
    test=0
    for ((i=0;i<$numOfCountries;i++));
    do
        if [[ $country == ${existingCountriesArray[i]} ]];
        then
            test=1
            break
        fi
    done
    if [[ $test == 0    ]];
    then
        existingCountriesArray[$numOfCountries]=$country
        numOfCountries=$(($numOfCountries+1))
    fi
}

if [ $# -eq 3 ];
then
        echo "Total arguments : $#"
        inputFile=$1
        if [ -f "$inputFile" ];
        then
            echo "Input File : $inputFile"
        else
            echo "FILE: $inputFile does not exist."
            exit 1
        fi
        input_dir=$2
        if [ -d "$input_dir" ];
        then
            echo "DIRECTORY: $input_dir already exists."
            exit 1
        else
            echo "Input directory = $input_dir"
        fi
        numFilesPerDirectory=$3        
        echo "Number of files per directory = $numFilesPerDirectory"
else
        echo "Incorrect number of arguments passed"
        echo "There must be a total of 3 arguments" 
        exit 1
fi

mkdir $input_dir

declare -a existingCountriesArray
numOfCountries=0
numLine=0
while read -r line
do
    array=($line)
    country=${array[3]}
    echo $country
    checkCountry
    if [[ $test == 0 ]];
    then
        mkdir "$input_dir/$country"
        for ((j=1;j<=$numFilesPerDirectory;j++));
        do
            touch "$input_dir/$country/$country-$j.txt"
        done
        numInputLine=0
        roundRobinIndex=1;
        while read -r inputline
        do
            if [[ $numInputLine > $numLine ]];
            then
                inputarray=($inputline)
                inputcountry=${inputarray[3]}
                if [[ $inputcountry == $country ]];
                then
                    echo $inputline >> "$input_dir/$country/$country-$roundRobinIndex.txt"
                    roundRobinIndex=$(( ($roundRobinIndex) % ($numFilesPerDirectory) + 1))
                fi
            fi
            numInputLine=$(($numInputLine+1))
        done < $inputFile
    fi
    numLine=$(($numLine+1))
done < $inputFile