#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int p[2]; // Pipe để giao tiếp giữa các tiến trình
    pipe(p);

    int pid = fork();
    if (pid < 0) {
        printf("Fork failed\n");
        exit(1);
    }

    if (pid == 0) {
        // Tiến trình con - xử lý số nguyên tố
        close(p[1]); // Tiến trình con không ghi vào pipe

        int prime;
        while (read(p[0], &prime, sizeof(prime)) > 0) {
            printf("prime %d\n", prime);

            int next_pipe[2];
            pipe(next_pipe);

            int pid_next = fork();
            if (pid_next < 0) {
                printf("Fork failed\n");
                exit(1);
            }

            if (pid_next == 0) {
                // Tiến trình con tiếp theo
                close(next_pipe[1]);
                p[0] = next_pipe[0]; // Chuyển pipe cho tiến trình tiếp theo
                continue; // Tiến trình tiếp tục xử lý
            } else {
                // Tiến trình hiện tại lọc các số
                close(next_pipe[0]);

                int num;
                while (read(p[0], &num, sizeof(num)) > 0) {
                    if (num % prime != 0) {
                        write(next_pipe[1], &num, sizeof(num));
                    }
                }
                close(next_pipe[1]); // Đóng pipe sau khi xử lý xong
                wait(0); // Chờ tiến trình con kết thúc
                exit(0);
            }
        }

        close(p[0]); // Đóng pipe khi kết thúc
        exit(0);
    } else {
        // Tiến trình cha - gửi các số từ 2 đến 280 vào pipe
        close(p[0]); // Tiến trình cha không đọc từ pipe

        for (int i = 2; i <= 280; i++) {
            write(p[1], &i, sizeof(i));
        }

        close(p[1]); // Đóng pipe sau khi gửi xong
        wait(0); // Chờ tiến trình con kết thúc
        exit(0);
    }
}
