# Disease Monitor

Application to proccess and answer queries based on disease cases. A file of patient records is given as an input, and is analysed using several data structures. Then, the user might ask some questions in the system.

## Available Queries

#### /globalDiseaseStats [date1 date2]

Returns for every virus recorded, the number of patients recorded in the system, optionally between 2 dates


#### /diseaseFrequency virusName date1 date2  [country]

Returns for every virus recorded, the number of patients recorded between 2 dates, optionally, in a specified countries.

#### /topk-Diseases k country [date1 date2]

Returns the top k diseases that are recorded in the country sepcified, allong with the number of cases, optionally between the 2 dates given.

#### /topk-Countries k disease [date1 date2]

Returns the top k countries that the specified disease is found, allong with the number of cases, optionally between the 2 dates given.

#### /insertPatientRecord recordID patientFirstName patientLastName diseaseID country entryDate [exitDate]

Inserts a new patient record in the systems

#### /recordPatientExit recordID exitDate

Adds an exit date to the specified patient

#### /numCurrentPatients [disease]

Prints the currently hospitalized patients, optionally with a specified disease

## Modules

Several data structures have been implemented (can be found in the modules directory), that ensure the best possible complexity for storing the data, and asking questions.

## Input type

An patient record could do like this
` 889 Mary Smith COVID-2019 China 25-1-2020 27-1-2020`

## Input files

In the directory `input`, several input files can be found

## Getting Started

Make sure you have a gcc compiler

### Downloading

Download source code by typing:

``` git clone https://github.com/nikosgalanis/B-Plus-Tree.git ```

## Compiling & running 

- Compile the project simply by typing `make`
- Run like this: `build/diseaseMonitor -p patientRecordsFile –h1 diseaseHashtableNumOfEntries
–h2 countryHashtableNumOfEntries –b bucketSize`

where bucketsize stands for the hash table node size in bytes.
