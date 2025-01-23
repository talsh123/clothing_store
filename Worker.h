#ifndef WORKER_H
#define WORKER_H

typedef struct worker {
    char* username;
    char* privateName;
    char* password;
    char* permissionLevel;
} Worker;

Worker* createWorker(char* username, char* privateName, char* password, char* permissionLevel);
void writeWorker(Worker* worker, const char* fileName);
int checkCredentials(const char* username, const char* password, const char* fileName);

#endif // WORKER_H