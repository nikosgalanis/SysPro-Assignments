#!/bin/bash

# Error messages for later use
USAGE="Usage: ./create_infiles.sh diseasesFile countriesFile input_dir numFilesPerDirectory numRecordsPerFile"
ARG_ERROR="Problem with the args provided"
rm -rf input/
# Check if exactly 5 args are provided
if [ $# -ne 5 ]; then
    echo ${USAGE} ;
    exit 1
fi

# Keep all the arguments
DISEASES_FILE=$1
COUNTRIES_FILE=$2
INPUT_DIR=$3
NUM_FILES_PER_DIR=$4
NUM_RECS_PER_FILE=$5

# Check if the disease and country files (mandatory) exists, and if the two numbers given are grater than 0.
if [ ! -e ${DISEASES_FILE} -o ! -e ${COUNTRIES_FILE} -o ${NUM_FILES_PER_DIR} -lt "0" -o ${NUM_RECS_PER_FILE} -lt "0" ]
    then echo ${ARG_ERROR};
    exit 1
fi

# Create 4 arrays containing all the diseases and all the countries available, as well as the names

names=(Michael Christopher Jessica Matthew Ashley Jennifer Joshua Amanda Daniel David James Robert John Joseph Andrew Ryan Brandon Jason Justin Sarah William Jonathan Stephanie Brian Nicole Nicholas
Anthony Heather Eric Elizabeth Adam Megan Melissa Kevin Steven Thomas Timothy Christina Kyle Rachel Laura Lauren Amber Brittany Danielle Richard Kimberly Jeffrey Amy Crystal Michelle Tiffany Jeremy Benjamin Mark Emily Aaron Charles Rebecca Jacob Stephen Patrick Sean Erin Zachary Jamie Kelly Samantha Nathan Sara Dustin Paul Angela Tyler Scott
Katherine Andrea Gregory Erica Mary Travis Lisa Kenneth Bryan Lindsey Kristen Jose Alexander Jesse KatieLindsay Shannon Vanessa Courtne Christine Alicia Cody Allison Bradley Samuel Shawn April Derek Kathryn Kristin Chad Jenna Tara Maria Krystal Jared Anna Edward Julie Peter Holly Marcus Kristina Natalie Jordan)

surnames=(SMITH JOHNSON WILLIAMS BROWN JONES MILLER DAVIS GARCIA RODRIGUEZ WILSON MARTINEZ ANDERSON TAYLOR THOMAS HERNANDEZ MOORE MARTIN JACKSON THOMPSON WHITE LOPEZ LEE GONZALEZ HARRIS CLARK LEWIS ROBINSON WALKER PEREZ HALL YOUNG ALLEN SANCHEZ WRIGHT KING SCOTT GREEN BAKER
ADAMS NELSON HILL RAMIREZ CAMPBELL MITCHELL ROBERTS CARTE PHILLIPS EVANS TURNER TORRES PARKER COLLINS EDWARDS STEWART FLORES MORRIS NGUYEN MURPHY RIVERA COOK ROGERS MORGAN PETERSON COOPER REE BAILEY BELL
GOMEZ KELLY HOWARD WARD COX DIAZ RICHARDSON WOOD WATSON BROOKS BENNETT GRAY JAMES REYES CRUZ HUGHES PRICE MYERS LONG FOSTER SANDERS ROSS MORALES POWELL SULLIVAN RUSSELL ORTIZ JENKINS GUTIERREZ PERRY BUTLER BARNES FISHER HENDERSON COLEMAN SIMMONS PATTERSON JORDAN REYNOLDS HAMILTON GRAHAMKIM GONZALES ALEXANDER RAMOS WALLACE GRIFFIN WEST COLE HAYES CHAVEZ )

diseases+=($(cat ${DISEASES_FILE}))
countries+=($(cat ${COUNTRIES_FILE}))

# Create the input dir if it is not already there
if [ ! -e ${INPUT_DIR} ]
    then mkdir ${INPUT_DIR};
fi

# Get the length of the arrays so we can iterate them
num_countries=${#countries[@]}
num_diseases=${#diseases[@]}
num_names=${#names[@]}
num_surnames=${#surnames[@]}

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
                id=$((1 + RANDOM % (2 * $NUM_RECS_PER_FILE * $NUM_FILES_PER_DIR)))
                age=$((1 + RANDOM % 120))
                a=$((RANDOM % $num_diseases))
                b=$((RANDOM % $num_names))
                c=$((RANDOM % $num_surnames))
                name=${names[b]}
                surname=${surnames[c]}
                disease=${diseases[a]}
                person="$name $surname $disease $age"
                if [ ${action} -lt "30" ]; then
                    echo $id EXIT $person >> $file_name
                else 
                    echo $id ENTER $person >> $file_name

                fi
            done
        fi
    done
    cd ../
done