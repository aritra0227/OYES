#include "fb.c"

unsigned int str_length(char * buf){
    unsigned int count = 0;
    while(buf[count]){
        count+=1;
    }
    return count;
}

int kmain(){
    char * fb_dummy = "FRAMEBUFFER WORKING";
    fb_write(fb_dummy, str_length(fb_dummy));
    return 145;
}