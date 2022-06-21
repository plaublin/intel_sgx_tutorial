#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sqlite3.h"
#ifdef COMPILE_WITH_INTEL_SGX
#include "enclaveshim_ocalls.h"
#include "sgx_trts.h"
#include "sgx_thread.h"
#else
#include <pthread.h>
#endif

#ifndef COMPILE_WITH_INTEL_SGX
	#define my_printf(format, ...) printf(format, ##__VA_ARGS__)
#endif

struct dbhandle {
	sqlite3* db;
#ifdef COMPILE_WITH_INTEL_SGX
	sgx_thread_mutex_t lock;
#else
	pthread_mutex_t lock;
#endif
};


/*
 * Arguments:
 *
 *   unused - Ignored in this case, see the documentation for sqlite3_exec
 *    count - The number of columns in the result set
 *     data - The row's data
 *  columns - The column names
 */
static int my_special_callback(void *unused, int count, char **data, char **columns)
{
	(void)unused;
	(void)columns;

	int idx;
	for (idx = 0; idx < count; idx++) {
		my_printf(" %s |", data[idx]);
	}
	my_printf("\n");

	return 0;
}

int execute_stmt(sqlite3 *db, const char *stmt, int (*cb)(void*,int,char**,char**)) {
	int ret = 0;
	char *zErrMsg = 0;

	//my_printf("Executing: %s\n", stmt);
	if (sqlite3_exec(db, stmt, cb, 0, &zErrMsg) != SQLITE_OK){
		my_printf("SQL error: %s, statement: %s\n", zErrMsg, stmt);
		sqlite3_free(zErrMsg);
		ret = -1;
	}
	return ret;
}

void* initialize_database(const char* dbname) {
	struct dbhandle* dbhandle = (struct dbhandle*)malloc(sizeof(*dbhandle));
	if (!dbhandle) {
		my_printf("%s:%i Unable to allocate memory\n", __func__, __LINE__);
		return NULL;
	}

	my_printf("Opening Sqlite database (%s)...\n", dbname);
	if(sqlite3_open(dbname, &dbhandle->db)) {
		my_printf("%s:%i Unable to open Sqlite database (%s): %s.\n", __func__, __LINE__, dbname, sqlite3_errmsg(dbhandle->db));
		exit(1);
	}

#ifdef COMPILE_WITH_INTEL_SGX
	sgx_thread_mutex_init(&dbhandle->lock, NULL);
#else
	pthread_mutex_init(&dbhandle->lock, NULL);
#endif

	return (void*)dbhandle;
}

void* ecall_initialize_database(const char* dbname) {
	return initialize_database(dbname);
}

int execute_statement(const void* dbhandle, const char* stmt) {
	int ret;
	struct dbhandle* dh = (struct dbhandle*)dbhandle;

#ifdef COMPILE_WITH_INTEL_SGX
	sgx_thread_mutex_lock(&dh->lock);
#else
	pthread_mutex_lock(&dh->lock);
#endif

	ret = execute_stmt(dh->db, stmt, (!strncmp(stmt, "SELECT", 6) ? my_special_callback : NULL));

#ifdef COMPILE_WITH_INTEL_SGX
	sgx_thread_mutex_unlock(&dh->lock);
#else
	pthread_mutex_unlock(&dh->lock);
#endif
	
	return ret;
}

int ecall_execute_statement(const void* dbhandle, const char* stmt) {
	return execute_statement(dbhandle, stmt);
}

void close_database(const void* dbhandle) {
	struct dbhandle* dh = (struct dbhandle*)dbhandle;
	if (sqlite3_close(dh->db) != SQLITE_OK) {
		my_printf("%s:%i Unable to close Sqlite database: %s.\n", __func__, __LINE__, sqlite3_errmsg(dh->db));
		exit(1);
	}
#ifdef COMPILE_WITH_INTEL_SGX
	sgx_thread_mutex_destroy(&dh->lock);
#else
	pthread_mutex_destroy(&dh->lock);
#endif
	free(dh);
}

void ecall_close_database(const void* dbhandle) {
	return close_database(dbhandle);
}
