#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

const char *logs[] = {
    "2025-11-03 10:00:01 [INFO] Server started successfully.",
    "2025-11-03 10:00:02 [DEBUG] Loading configuration file: /etc/app/config.yaml",
    "2025-11-03 10:00:03 [INFO] Connecting to database...",
    "2025-11-03 10:00:04 [INFO] Database connection established.",
    "2025-11-03 10:00:05 [WARN] High memory usage detected: 78%.",
    "2025-11-03 10:00:06 [DEBUG] Cache miss for key: user_42",
    "2025-11-03 10:00:07 [INFO] Request handled: GET /api/v1/status",
    "2025-11-03 10:00:08 [ERROR] Failed to write to socket: Connection reset by peer.",
    "2025-11-03 10:00:09 [DEBUG] Retrying connection to service...",
    "2025-11-03 10:00:10 [INFO] Reconnected successfully.",
    "2025-11-03 10:00:11 [INFO] New user registration: user_id=1034",
    "2025-11-03 10:00:12 [DEBUG] Session token generated for user_id=1034",
    "2025-11-03 10:00:13 [INFO] File uploaded: report.pdf (2.3MB)",
    "2025-11-03 10:00:14 [WARN] Disk usage at 85%.",
    "2025-11-03 10:00:15 [INFO] Cron job executed: cleanup_temp_files",
    "2025-11-03 10:00:16 [DEBUG] Deleting temp file: tmp123.tmp",
    "2025-11-03 10:00:17 [INFO] User login: admin",
    "2025-11-03 10:00:18 [ERROR] Authentication failed for user_id=305.",
    "2025-11-03 10:00:19 [INFO] Shutting down background workers.",
    "2025-11-03 10:00:20 [INFO] Worker 1 stopped.",
    "2025-11-03 10:00:21 [INFO] Worker 2 stopped.",
    "2025-11-03 10:00:22 [DEBUG] All threads joined successfully.",
    "2025-11-03 10:00:23 [INFO] Backup started.",
    "2025-11-03 10:00:24 [INFO] Backup completed successfully in 3.4s.",
    "2025-11-03 10:00:25 [WARN] API rate limit reached for user_id=102.",
    "2025-11-03 10:00:26 [INFO] Cache cleared successfully.",
    "2025-11-03 10:00:27 [DEBUG] Releasing unused memory blocks.",
    "2025-11-03 10:00:28 [INFO] Graceful shutdown initiated.",
    "2025-11-03 10:00:29 [INFO] All services stopped.",
    "2025-11-03 10:00:30 [INFO] Application terminated cleanly."
};

volatile sig_atomic_t stop = 0;

void handle_signal(int sig) {
    (void)sig;
    stop = 1;
}

// Usage - logs <file_name>.log
int main(int argc, char** argv) {
    srand(time(NULL));

    const char* file_name = argv[1];

    FILE* log_file = fopen(file_name, "a");

    if (!log_file) {
        printf("Unable to open file");
        return -1;
    }

    signal(SIGINT, handle_signal);

    const int min = 0;
    const int max = sizeof(logs) / sizeof(logs[0]) - 1;

    for (;;) {
        if (stop) {
            printf("\nReceived Ctrl+C — cleaning up...\n");
            fflush(log_file);
            fclose(log_file);
            printf("Log file closed. Exiting cleanly.\n");
            break;
        }

        const int num = min + rand() % (max - min + 1);
        const char* data = logs[num];
        fprintf(log_file, "%s\n", data);
        fflush(log_file);
        usleep(1 * 1000);
    }


    return 0;
}