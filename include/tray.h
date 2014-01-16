#ifndef TRAY_H_INCLUDED
#define TRAY_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>

struct gtk_conf_window {
	GtkWidget *window;
	GtkWidget *api_inputfield;
	GtkWidget *refresh_inputfield;
	GtkWidget *warn_box;
	GtkWidget *warn_inputfield;
} conf_window;

gboolean update_tray(GtkWidget *widget);

void tray_test_api(GtkWidget *widget, gpointer data);
void tray_hide_conf_window(GtkWidget *widget, gpointer data);
void tray_icon_on_menu(GtkStatusIcon *tray_icon, guint button,
		guint activation_time, gpointer user_data);
void tray_show(GtkMenuItem *item, gpointer window);
void tray_exit(GtkMenuItem *item, gpointer user_data);

void tray_set_icon(GtkStatusIcon *tray_icon, char* csn_icon[]);

GtkStatusIcon *create_tray_icon();

GtkStatusIcon *thetray;

#endif // TRAY_H_INCLUDED
