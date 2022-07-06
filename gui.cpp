// based on gtk simplest example: https://developer.gnome.org/gtk3/stable/gtk-getting-started.html
#include <gtk/gtk.h>

//#include <cstdlib>
//#include <algorithm> //std::min,max and etc
//#include "definitions.h"
#include "Network.h"
#include "Perceptron.cpp"
#include "Layer.cpp"

typedef struct
{
    float x;
    float y;
    int color; // 0 - blue, 1 - red
} Point;

Point points[100];
int points_count = 0;

int buf_width = 1200;
int buf_height = 800;

// draw area pixel buffer
GdkPixbuf *pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, false, 8, buf_width, buf_height);
int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
// The pixel buffer in the GdkPixbuf instance
guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);


bool thread_started = false;
GMutex mutex_interface;

Network * net;

void init_network(){
    g_mutex_lock(&mutex_interface);
    if(net){
        delete net;
    }
    net = new Network("net1");
    net->createLayer("input", 2);  //input layer. 2 perceptrons: for x and for y
    net->createLayer("hid1", 5);
    net->createLayer("hid2", 5);
    net->createLayer("output", 1); // one result - color
    g_mutex_unlock(&mutex_interface);
}


gpointer threadcompute(gpointer data)
{
    if(thread_started)
        return NULL;

    thread_started = true;

    while (true)
    {
        // for each point
        for(int i=0; i < points_count; i++){
            usleep(1);
            g_mutex_lock(&mutex_interface);
            PRINT_ON = false;
            Point p = points[i];
            // set input data
            net->setInputValue(0, p.x);
            net->setInputValue(1, p.y);
            net->forward();
            float arr [1] = {p.color ? 0.9f : 0.1f};
            net->learn(arr, 1);
            PRINT_ON = true;
            g_mutex_unlock(&mutex_interface);
        }
    }
    return NULL;
}

void on_start_button_clicked(GtkButton *button, gpointer data)
{
    g_thread_new("thread", threadcompute, data);
}

void on_clear_button_clicked(GtkButton *button, gpointer data)
{
    points_count = 0;
}


void on_reset_button_clicked(GtkButton *button, gpointer data)
{
    init_network();
}



void on_drawing_area_clicked(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    pf("area clicked btn: %d x: %f y: %f\n", event->button, event->x, event->y);
    guint width = gtk_widget_get_allocated_width(widget);
    guint height = gtk_widget_get_allocated_height(widget);

    points[points_count].x = event->x / width;
    points[points_count].y = event->y / height;
    points[points_count].color = event->button == 1 ? 0 : 1;
    points_count++;

    gtk_widget_queue_draw(widget);
    pf("added new point. Total points: %d\n", points_count);
}


void put_pixel(GdkPixbuf *pixbuf, int x, int y, guchar red, guchar green, guchar blue)
{
    int n_channels = 3;
    // Ensure that the coordinates are in a valid range
    if(x >= buf_width || y >= buf_height)
        return;


    // The pixel we wish to modify
    guchar *p = pixels + y * rowstride + x * n_channels;
    p[0] = red;
    p[1] = green;
    p[2] = blue;
    p[3] = 255;
}

gboolean draw_callback(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    //pf("draw_callback\n");
    guint width, height;
    GdkRGBA color;
    GtkStyleContext *context;
    context = gtk_widget_get_style_context(widget);
    width = (int) gtk_widget_get_allocated_width(widget);
    height = (int) gtk_widget_get_allocated_height(widget);
    width = std::min<int>(width, buf_width);
    height = std::min<int>(height, buf_height);
    // gtk_render_background(context, cr, 0, 0, width, height);
    // gtk_style_context_get_color(context, gtk_style_context_get_state(context), &color);
    color.alpha = 1;

    // draw network predictions field to the buffer
    int scale = 3;

    for (int y = 0; y < height / scale; y++)
    {
        for (int x = 0; x < width / scale; x++)
        {
            g_mutex_lock(&mutex_interface);
            float x_val = (float) (x+0.5) / width * scale;
            float y_val = (float) (y+0.5) / height * scale;
            net->setInputValue(0, x_val);
            net->setInputValue(1, y_val);
            net->forward();
            float net_result = net->outLayer()->perceptrons[0]->result;
            guchar red = 150;
            guchar blue = 200 - 200 * net_result;
            guchar green = 200 * net_result;
            for(int i = 0; i < scale*scale; i++){
                put_pixel(pixbuf, x * scale + (i/scale), y * scale + (i % scale), red, green, blue);
            }
            g_mutex_unlock(&mutex_interface);
        }
    }
    gdk_cairo_set_source_pixbuf(cr, pixbuf, 0, 0);
    cairo_paint(cr);

    // draw points
    for (int i = 0; i < points_count; i++)
    {
        Point p = points[i];
        cairo_rectangle(cr, p.x * std::min((int)width, buf_width) - 5, p.y * std::min((int)height, buf_height) - 5, 10, 10);
        //cairo_arc(cr, p.x * std::min((int)width, buf_width), p.y * std::min((int)height, buf_height), 5, 0, 360);
        color.green = 0;
        if (p.color == 0)
        {
            // blue
            color.green = 0;
            color.red = 0.1;
            color.blue = 0.7;
        }
        else
        {
            // red
            color.green = 0;
            color.red = 0.7;
            color.blue = 0.1;
        }
        gdk_cairo_set_source_rgba(cr, &color);
        cairo_fill(cr);
    }


    return FALSE;
}


gboolean timeout_redraw(GtkWidget *widget)
{
    gtk_widget_queue_draw(widget);
    return TRUE;
}

gboolean on_key_press (GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
    printf("key pressed: %d\n", event->keyval);
    if(event->keyval == 113 || event->keyval == 1738)
        gtk_widget_destroy(widget);

    return FALSE; 
}

void activate(GtkApplication *app, gpointer user_data)
{
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *button2;
    GtkWidget *button3;
    GtkWidget *layout_box;
    GtkWidget *button_box;
    GtkWidget *drawing_area;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Network Result");
    // gtk_window_set_default_size(GTK_WINDOW(window), 400, 400);

    layout_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_container_add(GTK_CONTAINER(window), layout_box);

    button_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_button_box_set_layout((GtkButtonBox *)button_box, GTK_BUTTONBOX_SPREAD);
    gtk_widget_set_margin_top(button_box, 20);
    // gtk_widget_set_vexpand(button_box, true);
    gtk_container_add(GTK_CONTAINER(layout_box), button_box);

    button = gtk_button_new_with_label("Start");
    g_signal_connect(button, "clicked", G_CALLBACK(on_start_button_clicked), NULL);
    gtk_container_add(GTK_CONTAINER(button_box), button);

    button2 = gtk_button_new_with_label("Clear");
    g_signal_connect(button2, "clicked", G_CALLBACK(on_clear_button_clicked), NULL);
    gtk_container_add(GTK_CONTAINER(button_box), button2);

    button3 = gtk_button_new_with_label("Reset");
    g_signal_connect(button3, "clicked", G_CALLBACK(on_reset_button_clicked), NULL);
    gtk_container_add(GTK_CONTAINER(button_box), button3);

    drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, 300, 300);
    gtk_widget_set_vexpand(drawing_area, true);
    gtk_widget_set_margin_start(drawing_area, 10);
    gtk_widget_set_margin_bottom(drawing_area, 10);
    gtk_widget_set_margin_end(drawing_area, 10);
    g_signal_connect(G_OBJECT(drawing_area), "draw", G_CALLBACK(draw_callback), NULL);
    gtk_widget_add_events(drawing_area, GDK_BUTTON_PRESS_MASK);
    g_signal_connect(G_OBJECT(drawing_area), "button-press-event", G_CALLBACK(on_drawing_area_clicked), (void *)123123);
    gtk_container_add(GTK_CONTAINER(layout_box), drawing_area);

    g_signal_connect (G_OBJECT (window), "key-press-event", G_CALLBACK (on_key_press), NULL);


    // redraw timer
    g_timeout_add(30, (GSourceFunc)timeout_redraw, drawing_area);

    gtk_widget_show_all(window);
}

int main(int argc, char **argv)
{
    init_network();

    GtkApplication *app;
    int status;

    app = gtk_application_new("my.neural.app", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}