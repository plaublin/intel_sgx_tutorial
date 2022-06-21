#ifndef ENCLAVESHIM_ECALLS_H_
#define ENCLAVESHIM_ECALLS_H_

void* initialize_database(const char* dbname);
int execute_statement(const void* dbhandle, const char* stmt);
void close_database(const void* dbhandle);

#endif
