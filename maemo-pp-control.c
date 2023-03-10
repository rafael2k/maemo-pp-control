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
GtkWidget *button_low_consumption;
GtkWidget *button_normal_consumption;


/* Create buttons and add it to main view */
GtkWidget *battery_display;
GtkWidget *battery_label;

GtkWidget *kbd_display;
GtkWidget *kbd_label;

GtkWidget *vbox;
GtkWidget *hbox0;
GtkWidget *hbox1;
GtkWidget *hbox2;
GtkWidget *hbox3;

void sig_handler(int sig_num)
{

    if(sig_num == SIGINT)
    {
        exit(EXIT_SUCCESS);
    }
}

void callback_button_pressed(gpointer data)
{
    system("sudo sh -c 'echo mem > /sys/power/state'");
}


gboolean battery_update(gpointer data)
{
    GtkEntry *label = (GtkEntry*)data;
    char buf[256];
    memset(&buf, 0x0, 256);
    int battery_level = -1;
    FILE *battery_fp = popen("upower -i /org/freedesktop/UPower/devices/battery_axp20x_battery | grep percentage | cut -d \"%\" -f 1 | cut -d \":\" -f 2 | xargs", "r");
    fscanf(battery_fp, "%d", &battery_level);
    pclose(battery_fp);
    if (battery_level == -1)
        sprintf(buf, "NOT FOUND");
    else
        sprintf(buf, "%d %%", battery_level);
    gtk_entry_set_text (label, buf);
    return TRUE;
}

gboolean kbd_update(gpointer data)
{
    GtkEntry *label = (GtkEntry*)data;
    char buf[256];
    memset(&buf, 0x0, 256);
    int battery_level = -1;
    FILE *battery_fp = popen("upower -i /org/freedesktop/UPower/devices/battery_ip5xxx_battery | grep percentage | cut -d \"%\" -f 1 | cut -d \":\" -f 2 | xargs", "r");
    fscanf(battery_fp, "%d", &battery_level);
    pclose(battery_fp);
    if (battery_level == -1)
        sprintf(buf, "NOT FOUND");
    else
        sprintf(buf, "%d %%", battery_level);
    gtk_entry_set_text (label, buf);
    return TRUE;
}

void enable_flashligth(gpointer data)
{
    system("echo 1 > /sys/class/leds/white:flash/brightness");
}

void disable_flashligth(gpointer data)
{
    system("echo 0 > /sys/class/leds/white:flash/brightness");
}

void sched_powersave(gpointer data)
{
    system("sudo sh -c 'echo powersave > /sys/bus/cpu/devices/cpu0/cpufreq/scaling_governor'");
    system("sudo sh -c 'echo powersave > /sys/bus/cpu/devices/cpu1/cpufreq/scaling_governor'");
    system("sudo sh -c 'echo powersave > /sys/bus/cpu/devices/cpu2/cpufreq/scaling_governor'");
    system("sudo sh -c 'echo powersave > /sys/bus/cpu/devices/cpu3/cpufreq/scaling_governor'");
}

void sched_schedutil(gpointer data)
{
    system("sudo sh -c 'echo schedutil > /sys/bus/cpu/devices/cpu0/cpufreq/scaling_governor'");
    system("sudo sh -c 'echo schedutil > /sys/bus/cpu/devices/cpu1/cpufreq/scaling_governor'");
    system("sudo sh -c 'echo schedutil > /sys/bus/cpu/devices/cpu2/cpufreq/scaling_governor'");
    system("sudo sh -c 'echo schedutil > /sys/bus/cpu/devices/cpu3/cpufreq/scaling_governor'");
}

void trigger_strobe(gpointer data)
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
    g_set_application_name("Maemo PinePhone Control");

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

    kbd_display = hildon_entry_new (HILDON_SIZE_FINGER_HEIGHT);
    gtk_entry_set_alignment (GTK_ENTRY(kbd_display), 0.5);
    gtk_editable_set_editable (GTK_EDITABLE (kbd_display), FALSE); // may be this should be false?

    kbd_label = gtk_label_new("Keyboard Battery Level:");

    button_lights_on = hildon_gtk_button_new(HILDON_SIZE_AUTO_HEIGHT);
    button_lights_off = hildon_gtk_button_new(HILDON_SIZE_AUTO_HEIGHT);
    button_low_consumption = hildon_gtk_button_new(HILDON_SIZE_AUTO_HEIGHT);
    button_normal_consumption = hildon_gtk_button_new(HILDON_SIZE_AUTO_HEIGHT);

    gtk_button_set_label (GTK_BUTTON(button_lights_on),"TORCH ON");
    gtk_button_set_label (GTK_BUTTON(button_lights_off),"TORCH OFF");

    gtk_button_set_label (GTK_BUTTON(button_low_consumption),"POWER SAVING");
    gtk_button_set_label (GTK_BUTTON(button_normal_consumption),"POWER NORMAL");

    vbox = gtk_vbox_new(TRUE, 3);
    hbox0 = gtk_hbox_new(TRUE, 5);
    hbox1 = gtk_hbox_new(TRUE, 5);
    hbox2 = gtk_hbox_new(TRUE, 5);
    hbox3 = gtk_hbox_new(TRUE, 5);

    gtk_container_add(GTK_CONTAINER(hbox0), button_lights_on);
    gtk_container_add(GTK_CONTAINER(hbox0), button_lights_off);

    gtk_container_add(GTK_CONTAINER(hbox1), button_low_consumption);
    gtk_container_add(GTK_CONTAINER(hbox1), button_normal_consumption);

    gtk_container_add(GTK_CONTAINER(hbox2), battery_label);
    gtk_container_add(GTK_CONTAINER(hbox2), battery_display);

    gtk_container_add(GTK_CONTAINER(hbox3), kbd_label);
    gtk_container_add(GTK_CONTAINER(hbox3), kbd_display);

    gtk_container_add(GTK_CONTAINER(vbox), button);
    gtk_container_add(GTK_CONTAINER(vbox), hbox0);
    gtk_container_add(GTK_CONTAINER(vbox), hbox1);
    gtk_container_add(GTK_CONTAINER(vbox), hbox2);
    gtk_container_add(GTK_CONTAINER(vbox), hbox3);

    g_signal_connect(G_OBJECT(button_lights_on), "clicked", G_CALLBACK(enable_flashligth), (void *) NULL);

    g_signal_connect(G_OBJECT(button_lights_off), "clicked", G_CALLBACK(disable_flashligth), (void *) NULL);

    g_signal_connect(G_OBJECT(button_low_consumption), "clicked", G_CALLBACK(sched_powersave), (void *) NULL);

    g_signal_connect(G_OBJECT(button_normal_consumption), "clicked", G_CALLBACK(sched_schedutil), (void *) NULL);

    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(callback_button_pressed), (void *) NULL);

    gtk_container_add(GTK_CONTAINER(window), vbox);

    /* Connect signal to X in the upper corner */
    g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(gtk_main_quit), NULL);

    /* Begin the main application */
    gtk_widget_show_all(GTK_WIDGET(window));

    battery_update((gpointer) battery_display);
    kbd_update((gpointer) kbd_display);

    g_timeout_add_seconds(3, battery_update, battery_display);

    g_timeout_add_seconds(3, kbd_update, kbd_display);

    gtk_main();

    return EXIT_SUCCESS;
}
