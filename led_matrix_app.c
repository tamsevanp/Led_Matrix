#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define LED_MATRIX_DEVICE "/dev/led_matrix"

// 8x8 LED matrix patterns
unsigned char smiley_face[8] = {
    0b00111100, //   ****  
    0b01000010, // *      * 
    0b10100101, // * *  * *
    0b10000001, // *      *
    0b10100101, // * *  * *
    0b10011001, // *  **  *
    0b01000010, //  *    * 
    0b00111100  //   ****  
};

unsigned char heart_shape[8] = {
    0b00000000, //         
    0b01100110, //  **  ** 
    0b11111111, // ********
    0b11111111, // ********
    0b11111111, // ********
    0b01111110, //  ******
    0b00111100, //   ****  
    0b00011000  //    **   
};

// Function to write data to the LED matrix device
void write_pattern_to_led_matrix(unsigned char *pattern)
{
    int fd = open(LED_MATRIX_DEVICE, O_WRONLY);
    if (fd == -1) {
        perror("Failed to open LED matrix device");
        exit(EXIT_FAILURE);
    }

    // Write the 8-byte pattern to the device
    ssize_t result = write(fd, pattern, 8);
    if (result != 8) {
        perror("Failed to write pattern to LED matrix");
    } else {
        printf("Pattern successfully written to LED matrix.\n");
    }

    close(fd);
}

int main()
{
    int choice;

    printf("LED Matrix Controller\n");
    printf("Choose a pattern to display:\n");
    printf("1. Smiley Face\n");
    printf("2. Heart Shape\n");
    printf("3. Exit\n");

    while (1) {
        printf("\nEnter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
        case 1:
            write_pattern_to_led_matrix(smiley_face);
            break;
        case 2:
            write_pattern_to_led_matrix(heart_shape);
            break;
        case 3:
            printf("Exiting...\n");
            exit(EXIT_SUCCESS);
        default:
            printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}


