#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>

#include "../include/tray.h"
#include "../include/conf.h"
#include "../include/csn_api.h"
#include "../include/icon.h"

// you shouldn't spend too much time in this file
// neither did I

gboolean update_tray(GtkWidget *widget)
{
	const char *api;
	char urlformat[] = "https://www.csn.tu-chemnitz.de/api/%s";
	char url[256];
	char tooltipformat[] = "Total traffic (%s): %i MiB (%.1f%%) (%s)";
	char tooltipformat_hh[] = "Total traffic (%s): %i MiB (%s)";
	char tooltip[128];
	char timestring[32];
	struct csn_api_value myval;
	time_t timer;
	struct tm* tm_info;

	double p;

	int totaltraffic;
	int happyhour;

	api = conf.key;

	sprintf(url, urlformat, api);

	myval = csn_api(url);
	if (!myval.updated) {
		tray_set_icon(thetray, csn_icon_gray);
		return TRUE;
	}

	GtkStatusIcon *tray_icon = (GtkStatusIcon*)widget;

	timer = myval.updated;
	tm_info = localtime(&timer);
	strftime(timestring, 32, "%H:%M", tm_info);

	happyhour = (myval.updated+3600)%86400	< 28800;

//	happyhour = (myval.updated % 86400) < 28800;
	totaltraffic = happyhour ? myval.h_total : myval.n_total;

	p = 100*myval.n_total / QUOTA;

	if (happyhour)
		sprintf(tooltip, tooltipformat_hh, "happy hour",
			totaltraffic, timestring);
	else
		sprintf(tooltip, tooltipformat, "normal",
			totaltraffic, p, timestring);

	if (p>=conf.warnlevel && !happyhour) {
		tray_set_icon(thetray, csn_icon_alert);
		GtkWidget *dialog;
		dialog = gtk_message_dialog_new (NULL,
			GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_WARNING,
			GTK_BUTTONS_CLOSE,
			"OMG! %.1f%% of quota reached!", p);
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
	} else if (happyhour) {
		tray_set_icon(thetray, csn_icon_nyan);
	} else {
		tray_set_icon(thetray, csn_icon);
	}

	gtk_status_icon_set_tooltip(tray_icon, tooltip);

	return TRUE;
}

GtkWidget* tray_bakeconfwindow()
{
	GtkWidget *confbox;
	GtkWidget *apikeybox, *api_testbutton;
	GtkWidget *refresh_box, *refresh_label;
	GtkWidget *closebox, *closebutton;

	conf_window.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(conf_window.window),
			"CSN Tray Settings");
	g_signal_connect (conf_window.window, "delete_event",
		G_CALLBACK(gtk_widget_hide_on_delete), NULL);
	gtk_widget_set_size_request(GTK_WIDGET(conf_window.window), 400, -1);
	gtk_container_set_border_width(GTK_CONTAINER(conf_window.window),5);

	confbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(conf_window.window), confbox);
	gtk_widget_show(confbox);

	apikeybox = gtk_hbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(confbox), apikeybox);
	gtk_widget_show(apikeybox);

	conf_window.api_inputfield = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(conf_window.api_inputfield), 32);
	gtk_entry_set_text(GTK_ENTRY(conf_window.api_inputfield), conf.key);
	gtk_container_add(GTK_CONTAINER(apikeybox), conf_window.api_inputfield);
	gtk_widget_show(conf_window.api_inputfield);

	api_testbutton = gtk_button_new_with_label("Test API key");
	gtk_widget_set_size_request(api_testbutton, 10, -1);
	gtk_widget_show(api_testbutton);
	gtk_container_add(GTK_CONTAINER(apikeybox), api_testbutton);
	gtk_signal_connect(GTK_OBJECT(api_testbutton), "clicked",
		GTK_SIGNAL_FUNC(tray_test_api),
		(gpointer)conf_window.api_inputfield);

	refresh_box = gtk_hbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(confbox), refresh_box);
	gtk_container_set_border_width(GTK_CONTAINER(refresh_box), 3);
	gtk_widget_show(refresh_box);

	refresh_label = gtk_label_new("Refreshing interval (min):");
	gtk_container_add(GTK_CONTAINER(refresh_box), refresh_label);
	gtk_widget_show(refresh_label);

	conf_window.refresh_inputfield = gtk_spin_button_new(GTK_ADJUSTMENT
			(gtk_adjustment_new(conf.refreshrate,15,1440,15,1,0)),
			15,0
		);
	gtk_container_add(GTK_CONTAINER(refresh_box),
		conf_window.refresh_inputfield);
	gtk_entry_set_width_chars(GTK_ENTRY(conf_window.refresh_inputfield), 3);
	gtk_widget_show(conf_window.refresh_inputfield);
	/* ***** API key frame ***** */

	/* ----- interface frame: warn options ----- */
	GtkWidget *warncheckbox_box;

	warncheckbox_box = gtk_hbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(confbox), warncheckbox_box);
	gtk_widget_show(warncheckbox_box);

	conf_window.warn_box = gtk_check_button_new_with_label(
				"Warn if traffic reaches threshold (%):");
	gtk_container_add(GTK_CONTAINER(warncheckbox_box), conf_window.warn_box);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(conf_window.warn_box),
				conf.warn);
	gtk_widget_show(conf_window.warn_box);

	conf_window.warn_inputfield = gtk_spin_button_new(GTK_ADJUSTMENT
			(gtk_adjustment_new(conf.warnlevel,0,100,5,1,0)), 15,0);
	gtk_container_add(GTK_CONTAINER(warncheckbox_box),
			conf_window.warn_inputfield);
	gtk_entry_set_width_chars(GTK_ENTRY(conf_window.warn_inputfield), 3);
	gtk_widget_show(conf_window.warn_inputfield);
	/* ***** interface frame: warn options ***** */

	closebox = gtk_alignment_new(1,0,0,0);
	gtk_container_add(GTK_CONTAINER(confbox), closebox);
	gtk_widget_show(closebox);

	closebutton = gtk_button_new_with_label("Apply & Close");
	gtk_container_add(GTK_CONTAINER(closebox), closebutton);
	gtk_widget_show(closebutton);
	gtk_signal_connect(GTK_OBJECT(closebutton), "clicked",
		GTK_SIGNAL_FUNC(tray_hide_conf_window), (gpointer)&conf_window);

	gtk_widget_hide(GTK_WIDGET(conf_window.window));

	return conf_window.window;
}

void tray_test_api(GtkWidget *widget, gpointer data)
{
	GtkWidget *e;
	const char *api;
	char urlformat[] = "https://www.csn.tu-chemnitz.de/api/%s";
	char url[256];

	e = (GtkWidget*)data;
	api = gtk_entry_get_text(GTK_ENTRY(e));

	sprintf(url, urlformat, api);

	struct csn_api_value val;
	val = csn_api(url);

	GtkWidget *dialog;

	if (val.error) {
		char* err;
		switch(val.error) {
		case CAE_NOCURL: {
			err = "CURL doesn't work";
			break;
		}
		case CAE_NOTOK: {
			err = "Cannot establish connection";
			break;
		}
		case CAE_NOAPI: {
			err = "API key not valid";
			break;
		}
		default: {
			err = "Unknown error";
		}
		}
		dialog = gtk_message_dialog_new(GTK_WINDOW(conf_window.window),
			GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_ERROR,
			GTK_BUTTONS_OK,
			"Error: %s", err);
		tray_set_icon(thetray, csn_icon_gray);
	} else {
		dialog = gtk_message_dialog_new(GTK_WINDOW(conf_window.window),
			GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_INFO,
			GTK_BUTTONS_CLOSE,
			"Works");
		update_tray((GtkWidget*)thetray);
	}
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy (dialog);
}

void tray_hide_conf_window(GtkWidget *widget, gpointer data)
{
	GtkWidget *w, *e_api, *e_refr, *e_warnlevel, *e_warnbox;
	w = conf_window.window;

	e_api = conf_window.api_inputfield;
	e_refr = conf_window.refresh_inputfield;
	e_warnlevel = conf_window.warn_inputfield;
	e_warnbox = conf_window.warn_box;

	gtk_widget_hide(GTK_WIDGET(w));

	conf.key = gtk_entry_get_text(GTK_ENTRY(e_api));
	conf.refreshrate = atoi(gtk_entry_get_text(GTK_ENTRY(e_refr)));
	conf.warnlevel = atoi(gtk_entry_get_text(GTK_ENTRY(e_warnlevel)));
	conf.warn = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(e_warnbox));

	update_tray((GtkWidget*)thetray);
}

void tray_icon_on_menu(GtkStatusIcon *tray_icon, guint button,
		guint activation_time, gpointer themenu)
{
	gtk_menu_popup(GTK_MENU(themenu), NULL, NULL,
			gtk_status_icon_position_menu, tray_icon,
			button, activation_time);
}

void tray_show(GtkMenuItem *item, gpointer window)
{
	gtk_widget_show(GTK_WIDGET(window));
	gtk_window_deiconify(GTK_WINDOW(window));
}

void tray_exit(GtkMenuItem *item, gpointer user_data)
{
	gtk_main_quit();
}

void tray_set_icon(GtkStatusIcon *tray_icon, char* csn_icon[])
{
	GdkPixbuf *icon;
	icon = gdk_pixbuf_new_from_xpm_data((const char**)csn_icon);
		gtk_status_icon_set_from_pixbuf(tray_icon, icon);
}

GtkStatusIcon *create_tray_icon()
{
	GtkStatusIcon *tray_icon;
//	GdkPixbuf *icon;
	tray_icon = gtk_status_icon_new();
/*	g_signal_connect(G_OBJECT(tray_icon), "activate",
			G_CALLBACK(tray_icon_on_click), NULL);*/

	GtkWidget *confwindow;
	confwindow = tray_bakeconfwindow();

	GtkWidget *traymenu, *traymenu_exit, *traymenu_conf;
	traymenu = gtk_menu_new();
	traymenu_conf = gtk_menu_item_new_with_label("Settings");
	traymenu_exit = gtk_menu_item_new_with_label("Quit");

	g_signal_connect(G_OBJECT(traymenu_conf), "activate",
			G_CALLBACK(tray_show), confwindow);
	g_signal_connect(G_OBJECT(traymenu_exit), "activate",
			G_CALLBACK(tray_exit), NULL);

	gtk_menu_shell_append(GTK_MENU_SHELL(traymenu), traymenu_conf);
	gtk_menu_shell_append(GTK_MENU_SHELL(traymenu), traymenu_exit);

	gtk_widget_show_all(traymenu);

	g_signal_connect(G_OBJECT(tray_icon), "popup-menu",
			G_CALLBACK(tray_icon_on_menu), traymenu);

	tray_set_icon(tray_icon, csn_icon);

	gtk_status_icon_set_tooltip(tray_icon, "");
	gtk_status_icon_set_visible(tray_icon, TRUE);

	return tray_icon;
}
