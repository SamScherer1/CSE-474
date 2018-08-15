
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#define NEW_CHAR_DIR "/dev/lcd"

#define ADD 1
#define SUBTRACT 2
#define MULTIPLY 3
#define DIVIDE 4

#define NULL_ISH -947
#define MAX_SIZE 10


double evaluate(double* numbers, int* operations, int num_numbers, int num_operations);



int main() {
    
	int size_buf, fd;
	char read_buf[100], write_buf[100];
	fd = open(NEW_CHAR_DIR, O_RDWR);
	if (fd < 0) {
		printf("File %s cannot be opened\n", NEW_CHAR_DIR);
		exit(1);
	}

    printf("----------------------------------\n");
    printf("Welcome to the calculator!\n");
    printf("This calculator can compute arithmetic expressions. To use, enter an expression such as:\n");
    printf("2+3/5-100+15\n");
    printf("Please do not enter any symbols other than '+', '-', '*', '/', and integers.\n");
    printf("As on a standard calculator, calculation can be continued by typing just '+2' or '/4'.\n");
    printf("Time to compute!\n\n");
    
    double eval = 0;
	int num_numbers = 0;

    while (1) {
        double numbers[MAX_SIZE] = {0};
        int operations[MAX_SIZE] = {0};

        int num_numbers = 0;
        int num_operations = 0;
        
        char prev_char = '0'; // keeps track of whether previous entry was number or operation
        
        //char str[10 * MAX_SIZE];
        //int str_size = 0;     

        while (1) {
            int c = getchar();
            if (c == '\n' || c == EOF) { /* at end, add terminating zero */
                break;
            }

            // if entry is an operation
            if (c == '+' || c == '-' || c == '*' || c == '/') {
                if (c == '+') {
                    operations[num_operations] = ADD;
                } else if (c == '-') {
                    operations[num_operations] = SUBTRACT;
                } else if (c == '*') {
                    operations[num_operations] = MULTIPLY;
                } else if (c == '/') {
                    operations[num_operations] = DIVIDE;
                } else {
                    printf("ERROR");
                }
                num_operations++;
                
                // update prev_char
                if (prev_char == '0') {
                    numbers[0] = eval;
                    num_numbers++;
                }
                prev_char = 'o';
            }
            
            // if entry is a digit
            else if (isdigit(c)) {
                if (prev_char == 'n') {
                    numbers[num_numbers - 1] = 10 * numbers[num_numbers - 1] + c - '0';
                } else {
                    numbers[num_numbers] = c - '0';
                    num_numbers++;
                    prev_char = 'n';
                }
            } else {
                printf("Cannot parse input: please use correct format.");
                break;
            }
            

			char char_str[10];
			char_str[0] = (char) c;
			char_str[1] = '\0';
            write(fd, char_str, sizeof(char_str));
			printf("Wrote %s\n", char_str);
            
            //str[str_size] = c;
            //str_size++;
        }
        //str[str_size] = '\0';
        
        eval = evaluate(numbers, operations, num_numbers, num_operations);
        //printf("Given: %s\n", str);
        //printf("= %.3f \n\n", eval);
        
        char result_str[10 * MAX_SIZE];
        sprintf(result_str, "=%.2f", eval);
        
        write(fd, "\n", sizeof("\n"));
		//printf("Test value: %d\n", testWrite);
		
		write(fd, result_str, sizeof(result_str));
		printf("Wrote %s\n", result_str);


		// wait and print newline
		usleep(2000000);

		char reset_str[10];
		reset_str[0] = '\t';
		reset_str[1] = '\0';
		write(fd, reset_str, sizeof(reset_str));
        
		// NEWLINE, THEN PRINT RESULT TO SCREEN
    
    }


	close(fd);
    return 0;
}


double evaluate(double* numbers, int* operations, int num_numbers, int num_operations) {
    
    double new_nums[MAX_SIZE] = {0};
    int new_opers[MAX_SIZE] = {0};
    
    int new_count = 0;
    double result = NULL_ISH;
    
    for (int j = 0; j < num_operations; j++) {
        // if addition or subtraction:
        if (operations[j] == ADD || operations[j] == SUBTRACT) {
            if (result == NULL_ISH) {
                new_nums[new_count] = numbers[j];
            } else {
                new_nums[new_count] = result;
            }
            new_opers[new_count] = operations[j];
            result = NULL_ISH;
            new_count++;
        } else if (operations[j] == MULTIPLY || operations[j] == DIVIDE) {
            if (result == NULL_ISH) {
                result = numbers[j];
            }
            if (operations[j] == MULTIPLY) {
                result = result * numbers[j+1];
            }
            if (operations[j] == DIVIDE) {
                result = result / (double) numbers[j+1];
            }
        }
    }
    
    // don't forget the last number
    if (result == NULL_ISH) {
        new_nums[new_count] = numbers[num_numbers-1];
    } else {
        new_nums[new_count] = result;
    }
    
    // actually evaluate
    double eval = new_nums[0];
    for (int j = 0; j < new_count; j++) {
        if (new_opers[j] == ADD) {
            eval = eval + new_nums[j + 1];
        } else if (new_opers[j] == SUBTRACT) {
            eval = eval - new_nums[j + 1];
        }
    }
    //printf("Final result: %.3f \n\n", eval);
    
    //printf("New numbers: %.3f, %.3f, %.3f, %.3f, %.3f...\n", new_nums[0], new_nums[1], new_nums[2], new_nums[3], new_nums[4]);
    //printf("New operations: %d, %d, %d, %d, %d...\n", new_opers[0], new_opers[1], new_opers[2], new_opers[3], new_opers[4]);
    
    return eval;
}



