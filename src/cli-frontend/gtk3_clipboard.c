#include <stdio.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <unistd.h>
#include <sys/wait.h>


static void __text_request_callback(GtkClipboard *clipboard, const gchar *old, gpointer data) {
  gtk_clipboard_set_text(clipboard, data, -1);
  gtk_main_quit();
}

static int __send_to_clipboard(const char *text) {
  GdkDisplay *display = NULL;
  GtkClipboard *clipboard = NULL;
  int did_init = 0;

begin:
  if(!(display = gdk_display_get_default())) {
    if(!did_init) {goto need_init;}
    fprintf(stderr, "from GTK: no display\n");
    return -1;
  }

  if(!(clipboard = gtk_clipboard_get_default(display))) {
    fprintf(stderr, "from GTK: no clipboard\n");
    return -2;
  }

  gtk_clipboard_request_text(clipboard, &__text_request_callback, (void*)text);
  gtk_main();

  return 0;

need_init:
  gtk_init(0, NULL);
  did_init = 1;
  goto begin;
}

int send_to_clipboard(const char *text) {
  pid_t child;
  if(!(child = fork())) {
    return __send_to_clipboard(text);
  } else {
    int status;
    waitpid(child, &status, 0);
    return status;
  }
}
