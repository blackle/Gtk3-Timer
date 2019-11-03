#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <glib.h>
#include <gio/gio.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <webkit2/webkit2.h>

gboolean on_load_changed(WebKitWebView *web_view, WebKitLoadEvent load_event, GtkWindow* window)
{
	(void) web_view;
	if (load_event == WEBKIT_LOAD_FINISHED) {
		gtk_widget_show_all (GTK_WIDGET(window));
		gtk_window_fullscreen (window);
	}
	return TRUE;
}

static void
gresource_uri_scheme_request_cb (WebKitURISchemeRequest *request, gpointer user_data)
{
	(void) user_data;

	const gchar *path = webkit_uri_scheme_request_get_path (request);
	printf("%s\n", path);

	GError *resource_error = NULL;
	GInputStream *stream = g_resources_open_stream (path, G_RESOURCE_LOOKUP_FLAGS_NONE, &resource_error);
	if (stream == NULL) {
		webkit_uri_scheme_request_finish_error(request, resource_error);
		return;
	}

	webkit_uri_scheme_request_finish (request, stream, -1, "text/html");
	g_object_unref (stream);
}

int main(int argc, char** argv)
{ 
	gtk_init(&argc, &argv);

	GtkWidget *main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size (GTK_WINDOW(main_window), 300, 300);
	gtk_window_set_resizable (GTK_WINDOW(main_window), FALSE);

	WebKitWebView *web_view = WEBKIT_WEB_VIEW(webkit_web_view_new ());
	webkit_web_context_register_uri_scheme (webkit_web_context_get_default (), "gresource", gresource_uri_scheme_request_cb, NULL, NULL);

	gtk_container_add (GTK_CONTAINER(main_window), GTK_WIDGET(web_view));

	g_signal_connect (main_window, "destroy", gtk_main_quit, NULL);
	g_signal_connect (web_view, "load-changed", G_CALLBACK(on_load_changed), (GtkWindow*)(main_window));

	webkit_web_view_load_uri (web_view, "resource:///com/blackle/timer/index.html");

	gtk_main();
}