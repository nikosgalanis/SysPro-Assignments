# SysPro Assignments

This set of assignments aims to construct C programs that will take as input virus data, they will process and record them, and will finally answer queries about those cases.

## DiseaseMonitor

DiseaseMonitor is centered around the data structures created and used, such as an AVL tree, and a complicated Hash Table. The programs takes as input various cases from different countries, and answers to queries about those records.


### Compiling & running 

- Compile the project simply by typing `make`
- Run like this: `build/diseaseMonitor -p patientRecordsFile –h1 diseaseHashtableNumOfEntries –h2 countryHashtableNumOfEntries –b bucketSize` where bucketsize stands for the hash table node size in bytes.

## DiseaseAggregator

DiseaseAggregator is centered around inter-process communication, using mostly pipes and signals. A bash script generating sample data is also provided. The queries are the same with Monitor.

### Compiling & running 

- Compile the project simply by typing `make`
- Run like this: `./diseaseAggregator –w numWorkers -b bufferSize -i input_dir`  where buffersize stands for the size of the buffer used to exchange data through the pipes.

## DiseaseServer

DiseaseServer is centered around network commutication, following the client-server model through TCP, using threads. The application-layer protocol makes use of verification messages to preserve data integrity across processes. The dataa structures used, as well as the queries are similar to the two previous projects.

### Compiling & running 

- Compile the project simply by typing `make`
- Run like this: `./master –w numWorkers -b bufferSize –s serverIP –p serverPort -i input_dir`  where buffersize stands for the size of the buffer used to exchange data through the pipes.

------------------------------------

## Available Queries

The queries available for every project are the following 

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

## Input type

An patient record could do like this
` 889 Mary Smith COVID-2019 China 25-1-2020 27-1-2020`

## Input files

In every project, in the directory `input`, several input files can be found

## Getting Started

Make sure you have a gcc compiler

## Downloading

Download source code by typing: 

 ```git clone https://github.com/nikosgalanis/SysPro-Assignments.git```