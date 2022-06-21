#ifndef DATABASE_H_
#define DATABASE_H_

#ifdef COMPILE_WITH_INTEL_SGX
void* ecall_initialize_database(const char* dbname);
int ecall_execute_statement(const void* dbhandle, const char* stmt);
void ecall_close_database(const void* dbhandle);
#endif

void* initialize_database(const char* dbname);
int execute_statement(const void* dbhandle, const char* stmt);
void close_database(const void* dbhandle);

#endif
