#include <linux/fb.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

uint32_t pixel_color(uint8_t r, uint8_t g, uint8_t b, struct fb_var_screeninfo *vinfo)
{
    return (r<<vinfo->red.offset) | (g<<vinfo->green.offset) | (b<<vinfo->blue.offset);
}

void draw_line(int x1, int y1, int x2, int y2, uint32_t pixel)
{
    int i,dx,dy,sdx,sdy,dxabs,dyabs,x,y,px,py;

    dx=x2-x1;			//Delta x
    dy=y2-y1;			//Delta y
    dxabs=abs(dx);		//Absolute delta
    dyabs=abs(dy);		//Absolute delta
    sdx=(dx>0)?1:-1; //signum function
    sdy=(dy>0)?1:-1; //signum function
    x=dyabs>>1;
    y=dxabs>>1;
    px=x1;
    py=y1;

    if (dxabs>=dyabs)
    {
        for(i=0;i<dxabs;i++)
        {
            y+=dyabs;
            if (y>=dxabs)
            {
                y-=dxabs;
                py+=sdy;
            }
            px+=sdx;
            draw(px,py,pixel);
        }
    }
    else
    {
        for(i=0;i<dyabs;i++)
        {
            x+=dxabs;
            if (x>=dyabs)
            {
                x-=dyabs;
                px+=sdx;
            }
            py+=sdy;
            draw(px,py,pixel);
        }
    }
}


//Draw a circle at (cx,cy)
void draw_circle(double cx, double cy, int radius, uint32_t pixel)
{
    inline void plot4points(double cx, double cy, double x, double y, uint32_t pixel)
    {
        draw(cx + x, cy + y,pixel);
        draw(cx - x, cy + y,pixel);
        draw(cx + x, cy - y,pixel);
        draw(cx - x, cy - y,pixel);
    }

    inline void plot8points(double cx, double cy, double x, double y, uint32_t pixel)
    {
        plot4points(cx, cy, x, y,pixel);
        plot4points(cx, cy, y, x,pixel);
    }

    int error = -radius;
    double x = radius;
    double y = 0;

    while (x >= y)
    {
        plot8points(cx, cy, x, y, pixel);

        error += y;
        y++;
        error += y;

        if (error >= 0)
        {
            error += -x;
            x--;
            error += -x;
        }
    }
}


int main()
{
    struct fb_fix_screeninfo finfo;
    struct fb_var_screeninfo vinfo;

    int fb_fd = open("/dev/fb0",O_RDWR);

    //Get variable screen information
    ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo);
    vinfo.grayscale=0;
    vinfo.bits_per_pixel=32;
    ioctl(fb_fd, FBIOPUT_VSCREENINFO, &vinfo);

    ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo);

    ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo);

    long screensize = vinfo.yres_virtual * finfo.line_length;

    uint8_t *fbp = mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, (off_t)0);

    int x,y;

    printf("vinfo.xres = %d\n", vinfo.xres);
    printf("vinfo.yres = %d\n", vinfo.yres);


    for (x=0;x<vinfo.xres;x++)
    {

        for (y=0;y<vinfo.yres;y++)
        {
          
            long location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+vinfo.yoffset) * finfo.line_length;
            *((uint32_t*)(fbp + location)) = pixel_color(0xFF,0x00,0xFF, &vinfo);
          
        }

    }

    return 0;
}
