#include<gtk/gtk.h>
#include<stdlib.h>

#include"main.h"
GtkWidget *window_main;
GtkWidget *window_progress;

int main(int argc,char *argv[])
{
	gtk_init(&argc,&argv);
	create_main_window();
	gtk_main();
	return 0;
	
}
