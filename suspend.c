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
 * A single button app to suspend the system.
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

#include <gtk/gtk.h>

#include <hildon/hildon-banner.h>
#include <hildon/hildon-program.h>
#include <hildon/hildon.h>



/* Hildon/GTK stuff */
HildonProgram *program;
HildonWindow *window;
GtkWidget *button;
GtkWidget *button_lights_on;
GtkWidget *button_lights_off;


/* Create buttons and add it to main view */
GtkWidget *battery_display;
GtkWidget *battery_label;

GtkWidget *vbox;
GtkWidget *hbox0;
GtkWidget *hbox1;

void sig_handler(int sig_num)
{

    if(sig_num == SIGINT)
    {
        exit(EXIT_SUCCESS);
    }
}

// upower -i /org/freedesktop/UPower/devices/battery_axp20x_battery | grep percentage | cut -d "%" -f 1 | cut -d ":" -f 2 | xargs

void callback_button_pressed(GtkWidget * widget, char nothing)
{
    system("sudo sh -c 'echo mem > /sys/power/state'");
}


gboolean battery_update(gpointer data)
{
    GtkEntry *label = (GtkEntry*)data;
    char buf[256];
    memset(&buf, 0x0, 256);
    int battery_level = 0;
    FILE *battery_fp = popen("upower -i /org/freedesktop/UPower/devices/battery_axp20x_battery | grep percentage | cut -d \"%\" -f 1 | cut -d \":\" -f 2 | xargs", "r");
    fscanf(battery_fp, "%d", &battery_level);
    pclose(battery_fp);
    sprintf(buf, "%d %%", battery_level);
    gtk_entry_set_text (label, buf);
    return TRUE;
}

gboolean enable_flashligth(gpointer data)
{
    system("echo 1 > /sys/class/leds/white:flash/brightness");
}

gboolean disable_flashligth(gpointer data)
{
    system("echo 0 > /sys/class/leds/white:flash/brightness");
}

gboolean trigger_strobe(gpointer data)
{
    system("echo 1 > /sys/class/leds/white:flash/flash_strobe");
}


int main(int argc, char *argv[])
{
    if (argc > 1){
        fprintf(stderr, "Usage: %s\n", argv[0]);
        fprintf(stderr, "Usage example: %s\n", argv[0]);
        return EXIT_SUCCESS;
    }

    if (!gtk_init_check (&argc, &argv)) {
        fprintf(stderr, "Display cannot be initialized, wait for the curses interface :-P\n");
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

    button = hildon_gtk_button_new(HILDON_SIZE_AUTO_HEIGHT);
    gtk_button_set_label (GTK_BUTTON(button),"SUSPEND");

    // set powersave / set schedutil (normal)
    // start camera focus
    // stop camera focus
    // battery level & kbd battery level
    battery_display = hildon_entry_new (HILDON_SIZE_FINGER_HEIGHT);
    gtk_entry_set_alignment (GTK_ENTRY(battery_display), 0.5);
    gtk_editable_set_editable (GTK_EDITABLE (battery_display), FALSE); // may be this should be false?

    battery_label = gtk_label_new("Battery Level:");

    button_lights_on = hildon_gtk_button_new(HILDON_SIZE_FINGER_HEIGHT);
    button_lights_off = hildon_gtk_button_new(HILDON_SIZE_FINGER_HEIGHT);

    gtk_button_set_label (GTK_BUTTON(button),"TORCH ON");
    gtk_button_set_label (GTK_BUTTON(button),"TORCH OFF");


    vbox = gtk_vbox_new(TRUE, 3);
    hbox0 = gtk_hbox_new(TRUE, 5);
    hbox1 = gtk_hbox_new(TRUE, 5);

    gtk_container_add(GTK_CONTAINER(hbox0), button_lights_on);
    gtk_container_add(GTK_CONTAINER(hbox0), button_lights_off);

    gtk_container_add(GTK_CONTAINER(hbox1), battery_label);
    gtk_container_add(GTK_CONTAINER(hbox1), battery_display);

    gtk_container_add(GTK_CONTAINER(vbox), button);
    gtk_container_add(GTK_CONTAINER(vbox), hbox0);
    gtk_container_add(GTK_CONTAINER(vbox), hbox1);

    g_signal_connect(G_OBJECT(button_lights_on), "clicked", G_CALLBACK(enable_flashligth), (void *) NULL);

    g_signal_connect(G_OBJECT(button_lights_off), "clicked", G_CALLBACK(disable_flashligth), (void *) NULL);

    gtk_container_add(GTK_CONTAINER(window), vbox);

    /* Connect signal to X in the upper corner */
    g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(gtk_main_quit), NULL);

    /* Begin the main application */
    gtk_widget_show_all(GTK_WIDGET(window));

    g_timeout_add_seconds(3, battery_update, battery_display);

    gtk_main();

    return EXIT_SUCCESS;
}
