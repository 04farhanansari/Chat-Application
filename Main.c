#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#define MESSAGE_SIZE 50
struct Message
 {
    pid_t sender_pid;
    char content[MESSAGE_SIZE];
};
int main()
{
    int pfd[2];
    pid_t child_pid;
    key_t shm_key = 1234;
    int shm_id;
    struct Message *shm_message;
    if (pipe(pfd) == -1) 
    {
        printf("Pipe creation failed");
        exit(EXIT_FAILURE);
    }
shm_id = shmget(shm_key, sizeof(struct Message), IPC_CREAT | 0666);
 if (shm_id == -1)
    {
        printf("Shared memory creation failed");
        exit(EXIT_FAILURE);
    }
shm_message = (struct Message*) shmat(shm_id, NULL, 0);
if (shm_message == (struct Message*) -1) 
    {
        printf("Shared memory attachment failed");
        exit(EXIT_FAILURE);
    }
    if ((child_pid = fork()) == -1)
    {
        printf("Fork failed");
        exit(EXIT_FAILURE);
    }
    if (child_pid == 0) 
    { 
        close(pfd[1]); 
        char received_message[MESSAGE_SIZE];
        read(pfd[0], received_message, MESSAGE_SIZE);
        printf("Child received message from parent: %s\n",                   received_message);
         printf(shm_message->content, MESSAGE_SIZE, "Hello from Child!");
        shm_message->sender_pid = getpid();
       printf("Child sent message to parent through shared memory.\n");
close(pfd[0]);  
 } 
    else
    {  
        close(pfd[0]);  
char message_to_child[] = "Hello from parent ";
write(pfd[1], message_to_child,            sizeof(message_to_child));
printf("Parent sent message to child: %s\n", message_to_child);
 wait(NULL);
 printf("Parent received message from child through shared  memory:\n");
 printf("Sender PID: %d\n", shm_message->sender_pid); close(pfd[1]);  
shmdt(shm_message);
 shmctl(shm_id, IPC_RMID, NULL); }
return 0;
}
