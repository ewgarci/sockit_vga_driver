#ifndef VGA_CTL_H
#define VGA_CTL_H
#include <linux/ioctl.h>

#define VGA_SIZE_LENGTH    640
#define VGA_SIZE_WIDTH     480

typedef struct{

    unsigned int center_x;
    unsigned int center_y;
    //unsigned int length;
    //unsigned int width;
    //unsigned int radius;
    unsigned int move_up;
    unsigned int move_down;
    unsigned int move_left;
    unsigned int move_right;
    unsigned int bouncing; 

} vga_arg_t;

#define VGA_MAGIC 'l'
#define SHOW_FIGURE _IOW(VGA_MAGIC, 1, vga_arg_t *)

#endif
