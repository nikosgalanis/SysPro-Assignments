#include "common_types.h"
#include "common_functions.h"
#include "HashTable.h"
#include "BalancedTree.h"
#include "Patient.h"
#include "LinkedList.h"
#include "WorkerMenu.h"
#include <dirent.h> 
#include "Queries.h"
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

extern HashTable dirs_to_workers;
extern List workers;
extern char* workers_ip;

void menu(char* instruction, int client_fd) {
	// initialize a connection to the aprropriate worker
	struct sockaddr_in worker;
	struct sockaddr* worker_ptr = (struct sockaddr*)&worker;
	struct hostent* rem;
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	rem = gethostbyname(workers_ip);
	worker.sin_family = AF_INET;
	memcpy(&worker.sin_addr, rem->h_addr, rem->h_length);
	if(strstr(instruction, "/diseaseFrequency")) {
		//if one country is provided
		bool single_country = (n_words(instruction) == 5);
		if (single_country) {
			char* country = nth_word(instruction, 5);
			// search for the worker that has taken this country
			HashEntry ent = hash_search(dirs_to_workers, country);
			if (ent) {
				// get the port of the worker
				int port = *(int*)ent->item;
				worker.sin_port = htons(port);
				connect(sock, worker_ptr, sizeof(worker));
				// inform __only__ this worker for the query
				write_to_socket(sock, instruction, strlen(instruction));
				// get his response and print it
				char* response = read_from_socket(sock);
				// close the connection
				close(sock);
				fprintf(stdout, "Query: %s\nAnswer:%d\n", instruction, atoi(response));
				// write the response in the client fd
				write_to_socket(client_fd, response, strlen(response));
				free(response);
			}
			else {
				write_to_socket(client_fd, "-", strlen("-"));
				fprintf(stderr, "Country provided not listed in input dir\n");
				return;
			}
		}
		// else the user wants all the countries, thus we must inform all workers 
		else {
			int result = 0;
			// write into every worker's fd
			for (int i = 0; i < list_size(workers); i++) {
				int port = *(int*)list_nth(workers, i);
				worker.sin_port = htons(port);
				connect(sock, worker_ptr, sizeof(worker));
				// inform __only__ this worker for the query
				write_to_socket(sock, instruction, strlen(instruction));
				// get his response and print it
				char* response = read_from_socket(sock);
				// add this to the total result
				result += atoi(response);
				// close the connection
				close(sock);
				fprintf(stdout, "Query: %s\nAnswer:%d\n", instruction, atoi(response));
				// write the response in the client fd
				free(response);
			}
			char* final = itoa(result);
			write_to_socket(client_fd, final, strlen(final));
			// inform the user about the result
			fprintf(stdout, "%d\n", result);
		}
	}
	else if(strstr(instruction, "/topk-AgeRanges")) {
		// the country is the 2nd word in our string
		char* country = nth_word(instruction, 3);
		// search for the worker that has taken this country
		HashEntry ent = hash_search(dirs_to_workers, country);
		if (ent) {
			// get the port of the worker
			int port = *(int*)ent->item;
			worker.sin_port = htons(port);
			connect(sock, worker_ptr, sizeof(worker));
			// inform __only__ this worker for the query
			write_to_socket(sock, instruction, strlen(instruction));
			// find how many times we will read from the pipe
			int k = atoi(read_from_socket(sock));
			char result[100];
			fprintf(stdout, "Query: %s\nAnswer:\n", instruction);
			// get his response and print it
			for (int i = 0; i < k; i++) {
				char* response = read_from_socket(sock);
				fprintf(stdout, "%s", response);
				// also collect all the responses to send back to client
				strcat(result, response);
				free(response);
			}
			// wirte the result to the client's fd
			write_to_socket(client_fd, result, strlen(result));
			close(sock);
		}
		else {
			write_to_socket(client_fd, "-", strlen("-"));
			fprintf(stderr, "Country provided not listed in input dir\n");
			return;
		}
	}
	else if(strstr(instruction, "/searchPatientRecord")) {
		// write into every worker's fd
		bool found = false;
		for (int i = 0; i < list_size(workers); i++) {
			int port = *(int*)list_nth(workers, i);
			worker.sin_port = htons(port);
			connect(sock, worker_ptr, sizeof(worker));
			// inform __only__ this worker for the query
			write_to_socket(sock, instruction, strlen(instruction));
			// get his response and print it
			char* response = read_from_socket(sock);
			// if such patient is found, print the record
			if (strcmp(response, "-")) {
				fprintf(stdout, "Query: %s\nAnswer:%s\n", instruction, response);
				// write the response in the client fd
				write_to_socket(client_fd, response, strlen(response));
				found = true;
				break;
			}
			// if no such record exists in all of our data
			if (!found) {
				fprintf(stdout, "Query: %s\nAnswer:-\n\n", instruction);
				write_to_socket(client_fd, "-", strlen("-"));
			}
			// close the connection
			close(sock);
			free(response);
		}
	}
	else if(strstr(instruction, "/numPatientAdmissions") || strstr(instruction, "/numPatientDischarges")) {
		// if 5 args are provided then the last is the country.
		bool single_country = (n_words(instruction) == 5);
		if (single_country) {
			char* country = nth_word(instruction, 5);
			// search for the worker that has taken this country
			HashEntry ent = hash_search(dirs_to_workers, country);
			if (ent) {
				// get the port of the worker
				int port = *(int*)ent->item;
				worker.sin_port = htons(port);
				connect(sock, worker_ptr, sizeof(worker));
				// inform __only__ this worker for the query
				write_to_socket(sock, instruction, strlen(instruction));
				// get his response and print it
				char* response = read_from_socket(sock);
				// close the connection
				close(sock);
				fprintf(stdout, "Query: %s\nAnswer:%s\n", instruction, response);
				// write the response in the client fd
				write_to_socket(client_fd, response, strlen(response));
				free(response);
			}
			else {
				write_to_socket(client_fd, "-", strlen("-"));
				fprintf(stderr, "Country provided not listed in input dir\n");
				return;
			}
		}
		// else the user wants all the countries, thus we must inform all workers 
		else {
			char result[STRING_SIZE];
			fprintf(stdout, "Query: %s\nAnswer:\n", instruction);
			// write into every worker's fd
			for (int i = 0; i < list_size(workers); i++) {
				int port = *(int*)list_nth(workers, i);
				worker.sin_port = htons(port);
				connect(sock, worker_ptr, sizeof(worker));
				// inform __only__ this worker for the query
				write_to_socket(sock, instruction, strlen(instruction));
				// get his response and print it
				char* response = read_from_socket(sock);
				// add this to the total result
				strcat(result, response);
				// close the connection
				close(sock);
				fprintf(stdout, "%d\n", atoi(response));
				// write the response in the client fd
				free(response);
			}
			// write the whole string to the client's fd
			write_to_socket(client_fd, result, strlen(result));
		}
	}
	else {
		printf("Query not recognized. Choose one of the default options\n");
	}
}