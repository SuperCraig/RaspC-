/*
compile:
gcc -o capture capture.c `pkg-config --libs --cflags gtk+-2.0`

usage:
capture [-options...]
	-o outputfilename
	-s delayseconds
*/
#include <gtk/gtk.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <gdk/gdkx.h>
#include <iostream>
#include <vector>
using namespace std;

void do_capture_screenshot (char *file, int sec){
	gint x, y;
	GdkScreen* cur_screen = NULL;
	GdkWindow* window = NULL;
	GdkPixbuf* pixbuf_screenshot = NULL;
	GdkRectangle rect;
	GdkRectangle screen_rect;
	printf("ready to capture.....\r\n");
	sleep(sec);

	if(cur_screen == NULL)
		cur_screen = gdk_screen_get_default (); //get screen

	screen_rect.x = 0;
	screen_rect.y = 0;

	screen_rect.width = gdk_screen_get_width (cur_screen); //get screen width
	screen_rect.height = gdk_screen_get_height (cur_screen); //get screen height
	printf("screen_rect: x=%d,y=%d,w=%d,h=%d\n",screen_rect.x,screen_rect.y,screen_rect.width,screen_rect.height);

	clock_t getID1 = clock();
	window = gdk_screen_get_root_window (cur_screen); //get window by screen
	gdk_window_get_origin (window, &x, &y); //get origin point
	rect.x = x;
	rect.y = y;
	gdk_drawable_get_size (GDK_DRAWABLE (window), &rect.width, &rect.height); //get drawable size
	clock_t getID2 = clock();
	printf("Get screen by ID: %d\n", getID2-getID1);

	rect.height = 256;
	rect.width = 512;

	printf("rect: x=%d,y=%d,w=%d,h=%d\n",rect.x,rect.y,rect.width,rect.height);

	if (! gdk_rectangle_intersect (&rect, &screen_rect, &rect)) { //obtain the intersect area from rect and screen_rect
		printf("Capture failed!...\r\n");
		return;
	}

	printf("capture rect: x=%d,y=%d,w=%d,h=%d\n",rect.x,rect.y,rect.width,rect.height);
	clock_t shot1 = clock();
	pixbuf_screenshot = gdk_pixbuf_get_from_drawable (NULL, window,
		NULL,rect.x - x, rect.y - y, 0, 0,
		rect.width, rect.height); //get pixbuf from drawable widget
	clock_t shot2 = clock();
	printf("Screen shot time: %d\n", shot2-shot1);

	clock_t read1 = clock();
	int n_channels = gdk_pixbuf_get_n_channels(pixbuf_screenshot);
	int rowstride = gdk_pixbuf_get_rowstride(pixbuf_screenshot);
	guchar *pixels = gdk_pixbuf_get_pixels(pixbuf_screenshot);
	//size_t length = sizeof((unsigned char*) pixels);
	vector<unsigned char> pixels_v(pixels, pixels + rect.width * rect.height * n_channels);
	clock_t read2 = clock();

	printf("Read img buffer time: %d\n", read2-read1);

	printf("total size of buffer: %d\n", rect.width * rect.height * n_channels);

	clock_t save1 = clock();
	gdk_pixbuf_save (pixbuf_screenshot, file, "jpeg", NULL, "quality","100", NULL); //save pixbuf to jpeg format file
	clock_t save2 = clock();
	printf("Save to JPEG time: %d\n", save2-save1);

	clock_t free1 = clock();
	g_object_unref (pixbuf_screenshot);  //free pixbuf
	clock_t free2 = clock();
	printf("Free buffer time: %d\n", free2-free1);
	printf("Capture saved!!!\r\n");
	return;
}


int main( int argc, char *argv[]){
	int cmd, sec;
	char *file = NULL;
	time_t tt;
	struct tm *tm_ptr = NULL;
	file = (char *)malloc(32);
	time(&tt);
	tm_ptr = localtime(&tt);
	strftime(file, 32, "%Y%m%d%H%M%S", tm_ptr);
	strcat(file, ".jpg");
	sec = 3;
	gtk_init (&argc, &argv);
	while ((cmd = getopt(argc, argv, "o:s:")) != -1) {
		switch (cmd) {
			case 'o':
				file = optarg;
			break;
			case 's':
				sec = atoi(optarg);
			break;
			default:
			break;
		}
	}

	//do_capture_screenshot(file, sec);
	clock_t cap1 =clock();
	do_capture_screenshot(file, 0);
	clock_t cap2 = clock();
	printf("Cap time: %d", cap2-cap1);
	return 0;
}
