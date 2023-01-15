/*
 * Copyright (C) 2023 Rafael Diniz <rafael@riseup.net>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 *
 * Rhizo-dialer is an experimental dialer for Maemo.
 *
 */

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdbool.h>
#include <signal.h>
#include <threads.h>

#include <hildon/hildon-banner.h>
#include <hildon/hildon-program.h>
#include <hildon/hildon.h>
#include <gtk/gtk.h>


/* Hildon/GTK stuff */
HildonProgram *program;
HildonWindow *window;
GtkWidget *button;

void sig_handler(int sig_num)
{

    if(sig_num == SIGINT)
    {
        exit(EXIT_SUCCESS);
    }
}

void callback_button_pressed(GtkWidget * widget, char nothing)
{

    system("echo mem > /sys/power/state");

}

int main(int argc, char *argv[])
{
    if (argc > 1){
        fprintf(stderr, "Usage: %s\n", argv[0]);
        fprintf(stderr, "Usage example: %s\n");
        return EXIT_SUCCESS;
    }

    if (!gtk_init_check (&argc, &argv)) {
        log_message(LOG_FILE,"Display cannot be initialized, wait for the curses interface :-P\n");
        exit (EXIT_FAILURE);
    }

    signal(SIGINT, sig_handler);
    signal(SIGUSR1, sig_handler);

    /* Create the hildon program and setup the title */
    program = HILDON_PROGRAM(hildon_program_get_instance());
    g_set_application_name("Maemo Suspend");

    /* Create HildonWindow and set it to HildonProgram */
    window = HILDON_WINDOW(hildon_window_new());
    hildon_program_add_window(program, window);

    hildon_gtk_window_set_portrait_flags(GTK_WINDOW(window), HILDON_PORTRAIT_MODE_SUPPORT); // or HILDON_PORTRAIT_MODE_SUPPORT  ?

    // TODO: Use Hildon widgets!
    // http://maemo.org/api_refs/5.0/5.0-final/hildon/
    /* Create buttons and add it to main view */
    vbox = gtk_vbox_new(TRUE, 5);
    hbox0 = gtk_table_new(1, 3, FALSE);
    hbox1 = gtk_hbox_new(TRUE, 5);
    hbox2 = gtk_hbox_new(TRUE, 5);
    hbox3 = gtk_hbox_new(TRUE, 5);
    hbox4 = gtk_hbox_new(TRUE, 5);
    hbox5 = gtk_hbox_new(TRUE, 5);

    display = hildon_entry_new (HILDON_SIZE_AUTO);
    gtk_entry_set_alignment (GTK_ENTRY(display), 0.5);
    gtk_editable_set_editable (GTK_EDITABLE (display), TRUE); // may be this should be false?

    button9 = gtk_button_new_with_label("9");
//    button9 = hildon_gtk_button_new(HILDON_SIZE_AUTO);
// hildon_gtk_button_new (HILDON_SIZE_FINGER_HEIGHT | HILDON_SIZE_AUTO_WIDTH);
// gtk_button_set_label (GTK_BUTTON (button9), "9"); // ?
    button = gtk_button_new_with_label("SUSPEND");

    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(callback_button_pressed), (void *) NULL);

    gtk_container_add(GTK_CONTAINER(window), button);

    /* Connect signal to X in the upper corner */
    g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(gtk_main_quit), NULL);

    /* Begin the main application */
    gtk_widget_show_all(GTK_WIDGET(window));

    gtk_main();

    return EXIT_SUCCESS;
}
