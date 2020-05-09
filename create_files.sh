#!/bin/bash

# Error messages for later use
USAGE="Usage: ./create_infiles.sh diseasesFile countriesFile input_dir numFilesPerDirectory numRecordsPerFile"
ARG_ERROR="Problem with the args provided"
rm -rf input/
# Check if exactly 5 args are provided
if [ $# -ne 7 ]; then
    echo ${USAGE} ;
    exit 1
fi

# Keep all the arguments
DISEASES_FILE=$1
COUNTRIES_FILE=$2
INPUT_DIR=$3
NUM_FILES_PER_DIR=$4
NUM_RECS_PER_FILE=$5
NAMES_FILE=$6
SURNAMES_FILE=$7

# Check if the disease and country files (mandatory) exists, and if the two numbers given are grater than 0.
if [ ! -e ${DISEASES_FILE} -o ! -e ${COUNTRIES_FILE} -o ${NUM_FILES_PER_DIR} -lt "0" -o ${NUM_RECS_PER_FILE} -lt "0" ]
    then echo ${ARG_ERROR};
    exit 1
fi

# Create 2 arrays containing all the diseases and all the countries available
diseases=($(cat ${DISEASES_FILE}))
countries=($(cat ${COUNTRIES_FILE}))
names=($(cat ${NAMES_FILE}))
surnames=($(cat ${SURNAMES_FILE}))

# Create the input dir if it is not already there
if [ ! -e ${INPUT_DIR} ]
    then mkdir ${INPUT_DIR};
fi

# Get the length of the arrays so we can iterate them
num_countries=${#countries[@]}
num_diseases=${#diseases[@]}
num_names=${#names[@]}
num_surnames=${#surnames[@]}

a=$((RANDOM % $num_diseases))
b=$((RANDOM % $num_names))
c=$((RANDOM % $num_surnames))
echo $b $c
name=${names[b]}
surname=${surnames[c]}
disease=${diseases[a]}
echo $name $surname
# store the ids that have entred in a map
declare -A entries
# go to the input dir
cd $INPUT_DIR

# For each country
for ((i = 0; i < num_countries; i++)); do 
    # Create the apropriate subdir
    mkdir ${countries[i]}
    # get into there
    cd ${countries[i]}
    # For the number of files that we want to create
    for ((j = 0; j < $NUM_FILES_PER_DIR; j++)); do
        # Randomize the date 
        dd=$((1 + RANDOM % 31))
        mm=$((1 + RANDOM % 13))
        yyyy=$((2018 + RANDOM % 3)) 
        # Check that both day and month are 2 digits, else fix them
        if [ $dd -lt "10" ]
           then dd="0${dd}";
        fi
        if [ $mm -lt "10" ]
           then mm="0${mm}";
        fi
        file_name=$dd-$mm-$yyyy
        if [ ! -e $file_name ]; then
            touch $file_name
            for ((k = 0; k < $NUM_RECS_PER_FILE; k++)); do
                action=$((RANDOM % 100))
                # Smart way to create somewhat unique ids. Yeah, if u chavent noticed, I'm pretty smart.
                id=$((1 + RANDOM % (2 * $NUM_RECS_PER_FILE * $NUM_FILES_PER_DIR)))
                age=$((1 + RANDOM % 120))
                # a=$((RANDOM % $num_diseases))
                # b=$((RANDOM % $num_names))
                # c=$((RANDOM % $num_surnames))
                # name=${names[b]}
                # surname=${surnames[c]}
                # disease=${diseases[a]}
                # echo $name $surname
                # echo $name
                # echo ${names[$(RANDOM % $num_names)]} ${surnames[$(RANDOM % $num_surnames)]} ${diseases[$(RANDOM % $num_diseases)]} ${age}
                # if [ ${action} -lt "30" -a -n ${entred[$id]} ]; then
                #     echo $id EXIT "${entered[id]}" #>> $dd/$mm/$yyyy
                #     #TODO: Delete the value from the map
                # else 
                    # entred[id]= "${names[$((RANDOM % $num_names))]} ${surnames[$((RANDOM % $num_surnames))]} ${diseases[$((RANDOM % $num_diseases))} ${age}"
                #     echo $id ENTER ${entered[id]} #>> $dd/$mm/$yyyy

                # fi
            done
        fi
    done
    cd ../
done