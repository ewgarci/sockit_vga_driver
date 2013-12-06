#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdint.h>

 
#include "vga_ctl.h"
 
void show_figure(int fd)
{
    unsigned int c; 
    vga_arg_t v;
     
    printf("Enter center_x: ");
    scanf("%d", &c);
 //   getchar();

    v.center_x = c;

    printf("Enter center_y: ");
    scanf("%d", &c);

    v.center_y = c;


    if (ioctl(fd, SHOW_FIGURE, &v) == -1)
    {
        perror("show figure ioctl wrong");
    }
    else
    {
        printf("show figure");
    }
}

 
int main(int argc, char *argv[])
{
    char *file_name = "/dev/mvga";
    int fd;
    enum
    {
        e_showfigure,
        
    } option;
 
    if (argc == 1)
    {
        printf("wrong enter\n");
    }
    else if (argc == 2)
    {
        if (strcmp(argv[1], "-showfigure") == 0)
        {
            option = e_showfigure;
            printf("choice of show the figure\n");
        }
       
    }
    else
    {
        fprintf(stderr, "Usage: %s [-on | -off | -read | -blink]\n", argv[0]);
        return 1;
    }

    fd = open(file_name, O_RDWR);
    if (fd == -1)
    {
        perror("vga open got wrong");
        return 2;
    }
 
    switch (option)
    {
        case e_showfigure:
            show_figure(fd);
            break;
    
        default:
            break;
    }
 
    close (fd);
 
    return 0;
}
