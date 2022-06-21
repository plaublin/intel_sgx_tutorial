#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "database.h"

#define INPUT_LENGTH 256

int main(int argc, char** argv) {
	if (argc != 2) {
		printf("Usage: %s <dbname>\n", argv[0]);
		return -1;
	}

	char* dbname = argv[1];
	void* dbhandle = initialize_database(dbname);

	char* stmt = (char*)malloc(sizeof(*stmt)*INPUT_LENGTH);
	if (!stmt) {
		printf("Cannot allocate %d bytes for the statement buffer\n", INPUT_LENGTH);
		return -1;
	}

	printf("Enter SQL commands; QUIT to terminate the program.\n");
	while (1) {
		printf("> ");
		int i = 0;
		bzero(stmt, INPUT_LENGTH);
		while (i<INPUT_LENGTH) {
			char c = getchar();
			if (c == '\n' || c == '\0') {
				stmt[i] = '\0';
				break;
			} else {
				stmt[i] = c;
				i++;
			}
		}

		if (!strcmp(stmt, "QUIT")) {
			printf("\n");
			break;
		}

		int r = execute_statement(dbhandle, stmt);
		if (r != 0) {
			printf("Error while executing statement {%s}: %d\n", stmt, r);
			break;
		}
	}

	close_database(dbhandle);
	free(stmt);
	return 0;
}
