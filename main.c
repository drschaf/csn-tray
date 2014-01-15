#include <gtk/gtk.h>
#include <time.h>
#include <stdio.h>
#include <sys/stat.h>

#include "csn_api.h"
#include "tray.h"
#include "conf.h"

int main (int argc, char **argv)
{
	struct csn_api_config myconf;
	char configfile[256];
	char configdir[256];

//#ifdef OS_UNIX || __linux__
//#define _OS
	sprintf(configdir, "%s/%s", getenv("HOME"), ".csntray");
	sprintf(configfile, "%s/%s", configdir, "config.shit");
//#endif
//#define _OS
//#ifdef OS_WINDOWS
	// windows related fuck
//#endif
//#ifndef _OS
//#error wtf is your OS
//#endif

	myconf = readconfig(configfile);

	conf.key = myconf.key;
	conf.refreshrate = myconf.refreshrate;
	conf.warnlevel = myconf.warnlevel;
	conf.warn = myconf.warn;

	int updateinterval = 60000*conf.refreshrate;
		// FYI: API updates every fifteen minutes (a minute more than
		// you need to exceed your limit at 100Mibit/s)

	gtk_init(&argc, &argv);
	thetray = create_tray_icon(conf);

	update_tray((GtkWidget*)thetray);
	g_timeout_add(updateinterval, (GSourceFunc)update_tray, thetray);

	gtk_main();

	writeconfig(configfile, conf);

	return 0;
}
