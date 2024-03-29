// based on gtk simplest example: https://docs.gtk.org/gtk3/getting_started.html
#include <sys/time.h>
#include <gtk/gtk.h>

#include "colored_output.h"
#include "Network.h"
#include "Perceptron.cpp"
#include "Layer.cpp"

typedef struct
{
    float x;
    float y;
    int color; // 0 - blue, 1 - red
} Point;


#define LEARNING_RATES_COUNT 6
#define LEARNING_RATE_DEFAULT_INDEX 3
struct {
    float value;
    const char * label;
} g_learning_rates [LEARNING_RATES_COUNT] = {
    {0.001, "LR 0.001"}, 
    {0.005, "LR 0.005"}, 
    {0.01,  "LR 0.01"}, 
    {0.05,  "LR 0.05"}, 
    {0.1,   "LR 0.1"}, 
    {0.2,   "LR 0.2"},
};  

int g_epoch_count = 0; // epoch means training the neural network with all the training data for one cycle
int g_train_count = 0; // number of singe trainings. (train_count = epoch_count * points_count)

Point g_points[100];
int g_points_count = 0;

int g_buf_width = 1200;
int g_buf_height = 800;

// draw area pixel buffer
GdkPixbuf *g_pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, false, 8, g_buf_width, g_buf_height);
// The pixel buffer in the GdkPixbuf instance
guchar *g_pixels = gdk_pixbuf_get_pixels(g_pixbuf);


bool g_thread_started = false;
bool g_thread_exit_flag = false;
bool g_thread_sleep = true;
GMutex g_mutex_interface;

Network * g_net;

void init_network(){
    g_mutex_lock(&g_mutex_interface);
    if(g_net){
        delete g_net;
    }
    g_net = new Network("net1");
    g_net->createLayer("input", 2);  //input layer. 2 perceptrons: for x and for y
    g_net->createLayer("hid1", 5);
    g_net->createLayer("hid2", 5);
    g_net->createLayer("output", 1); // one result - color
    g_mutex_unlock(&g_mutex_interface);
}


gpointer threadcompute(gpointer data)
{
    if(g_thread_started)
        return NULL;

    g_thread_started = true;

    while (true)
    {
        usleep(0);
        if(g_thread_exit_flag){
            g_thread_exit_flag = false;
            g_thread_started = false;
            return NULL;
        }
        if(!g_points_count){
            usleep(10000);
            continue;
        }
        // for each point
        for(int i=0; i < g_points_count; i++){
            if(g_thread_sleep)
                usleep(1);
            
            // teach the network with one point
            g_mutex_lock(&g_mutex_interface);
            PRINT_ON = false;
            Point p = g_points[i];
            // set input data
            g_net->setInputValue(0, p.x);
            g_net->setInputValue(1, p.y);
            g_net->forward();
            float arr [1] = {p.color ? 0.9f : 0.1f};
            g_net->learn(arr, 1);
            PRINT_ON = true;
            g_train_count++;
            g_mutex_unlock(&g_mutex_interface);
        }
        g_epoch_count ++;
    }
    return NULL;
}


void put_pixel(GdkPixbuf *pixbuf, int x, int y, guchar red, guchar green, guchar blue)
{
    static int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    static int n_channels = gdk_pixbuf_get_n_channels(pixbuf);
    // Ensure that the coordinates are in a valid range
    if(x >= g_buf_width || y >= g_buf_height)
        return;


    // The pixel we wish to modify
    guchar *p = g_pixels + y * rowstride + x * n_channels;
    p[0] = red;
    p[1] = green;
    p[2] = blue;
    //p[3] = 255;
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
    width   = MIN(width, g_buf_width);
    height  = MIN(height, g_buf_height);
    // gtk_render_background(context, cr, 0, 0, width, height);
    // gtk_style_context_get_color(context, gtk_style_context_get_state(context), &color);
    color.alpha = 1;

    // draw network predictions field to the buffer
    int scale = 3;

    for (int y = 0; y <= height / scale; y++)
    {
        for (int x = 0; x <= width / scale; x++)
        {
            g_mutex_lock(&g_mutex_interface);
            float x_val = (float) (x+0.5) / width * scale;
            float y_val = (float) (y+0.5) / height * scale;
            g_net->setInputValue(0, x_val);
            g_net->setInputValue(1, y_val);
            g_net->forward();
            float net_result = g_net->outLayer()->perceptrons[0]->result;

            float br = 100 * (1 - std::abs(0.5 - net_result));
            guchar green = 128 + br;
            guchar blue  = MIN(255, 221 - 200 * net_result + br);
            guchar red   = MIN(255, 49 + 200 * net_result + br);
            

            for(int i = 0; i < scale*scale; i++){
                put_pixel(g_pixbuf, x * scale + (i/scale), y * scale + (i % scale), red, green, blue);
            }
            g_mutex_unlock(&g_mutex_interface);
        }
    }
    gdk_cairo_set_source_pixbuf(cr, g_pixbuf, 0, 0);
    cairo_paint(cr);

    // draw points
    for (int i = 0; i < g_points_count; i++)
    {
        Point p = g_points[i];
        cairo_rectangle(cr, p.x * std::min((int)width, g_buf_width) - 5, p.y * std::min((int)height, g_buf_height) - 5, 10, 10);

        color.green = 1;
        color.red = 1;
        color.blue = 1;
        
        gdk_cairo_set_source_rgba(cr, &color);
        cairo_set_line_width(cr, 2);
        cairo_stroke_preserve(cr);



        color.green = 0;
        if (p.color == 0)
        {
            // blue
            color.green = 0.7;
            color.red = 0.5;
            color.blue = 1;
        }
        else
        {
            // red
            color.green = 0.8;
            color.red = 1;
            color.blue = 0.5;
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


gboolean timeout_label(GtkWidget *widget)
{
    if(!widget) return FALSE;
    static int last_time_ms = 0;
    static int last_epoch_count = 0;
    static int last_train_count = 0;
    g_mutex_lock(&g_mutex_interface);

    struct timeval tp;
    gettimeofday(&tp, NULL);
    int cur_time_ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    int epoch_per_second = 0;
    int train_per_second = 0;

    if(cur_time_ms - last_time_ms > 0){
        int time_elapsed = cur_time_ms - last_time_ms;
        epoch_per_second = 1000 * (g_epoch_count - last_epoch_count) / time_elapsed;
        train_per_second = 1000 * (g_train_count - last_train_count) / time_elapsed;
        epoch_per_second = MAX(0, epoch_per_second);
        train_per_second = MAX(0, train_per_second);
    }

    last_time_ms = cur_time_ms;
    last_epoch_count = g_epoch_count;
    last_train_count = g_train_count;

    
    static const char * fmt =   "<tt>"
                                "<span foreground='gray' size='small'>   epoch: </span><b>% 5d</b> k  "
                                "<span foreground='gray' size='small'>speed: </span><b>% 6.2f</b> ke/s  \n"
                                "<span foreground='gray' size='small'>   error: </span><b><span foreground='%s'>%.3f</span></b>    "
                                "<span foreground='gray' size='small'>       </span> <b>% 4.1f</b> kt/s"
                                "</tt>";
    static char buf [500];

    sprintf(buf, fmt, 
        g_epoch_count / 1000, 
        (float) epoch_per_second / 1000, 
        g_net->outLayer()->errorAbsSum() < 0.05 ? "lightgray" : "magenta",
        g_net->outLayer()->errorAbsSum(),
        (float) train_per_second / 1000);

    gtk_label_set_markup((GtkLabel*)widget, buf);
    g_mutex_unlock(&g_mutex_interface);
    return TRUE;
}

gboolean on_key_press (GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
    printf("key pressed: %d\n", event->keyval);
    if(event->keyval == 113/*q*/ || event->keyval == 1738/*й*/ || event->keyval == 65307 /*esc*/)
        gtk_widget_destroy(widget);
    return FALSE; 
}

void on_start_button_clicked(GtkButton *button, gpointer data)
{
    if(g_thread_started){
        g_thread_exit_flag = true;
        gtk_button_set_label(button, "Start");
    }
    else {
        g_epoch_count = 0;
        g_thread_new("compute-thread", threadcompute, data);
        gtk_button_set_label(button, "Stop");
    }
}

void on_clear_button_clicked(GtkButton *button, gpointer data)
{
    g_points_count = 0;
}


void on_reset_button_clicked(GtkButton *button, gpointer data)
{
    g_epoch_count = 0;
    init_network();
}

void on_rng_button_clicked(GtkButton *button, gpointer data)
{
    auto fRand = [](){
        return (float)rand() / RAND_MAX;
    };

    int POINTS_MAX = 20;
    int points_cnt = fRand() * POINTS_MAX;

    g_points_count = 0; // clear points
    for(int i = 0; i < points_cnt; i++){
        g_points[g_points_count].x = fRand();
        g_points[g_points_count].y = fRand();
        g_points[g_points_count].color = fRand() > 0.5;
        g_points_count++;
    }
}




void on_drawing_area_clicked(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    pf("area clicked btn: %d x: %f y: %f\n", event->button, event->x, event->y);
    int width = gtk_widget_get_allocated_width(widget);
    int height = gtk_widget_get_allocated_height(widget);
    width = MIN(width, g_buf_width);
    height = MIN(height, g_buf_height);
    
    if(event->x > width || event->y > height)
        return;

    g_points[g_points_count].x = event->x / width;
    g_points[g_points_count].y = event->y / height;
    g_points[g_points_count].color = event->button == 1 ? 0 : 1;
    g_points_count++;

    gtk_widget_queue_draw(widget);
    pf("added new point. Total points: %d\n", g_points_count);
}

void on_combo1_changed(GtkComboBox *combo, gpointer data){
    gint act = gtk_combo_box_get_active(combo);
    activation_function_index = act;
    pf("changed index %d\n", act);
}

void on_combo2_changed(GtkComboBox *combo, gpointer data){
    gint act = gtk_combo_box_get_active(combo);
    Perceptron::learning_rate = g_learning_rates[act].value;
    pf("LR changed to %f\n", Perceptron::learning_rate);
}

void on_switch1_changed(GtkSwitch *sw, gpointer data){
    gint act = gtk_switch_get_active(sw);
    g_thread_sleep = (bool) act;
    pf("sleep active %d\n", act);
}

std::string get_network_title(Network * network){
    std::string result = "";
    for(int l = 0; l < g_net->layersCount(); l++){
        result += (l ? "-" : "") + std::to_string(g_net->getLayer(l)->size) ;
    }
    return result;
}


void activate(GtkApplication *app, gpointer user_data)
{
    GtkWidget *window;
    GtkWidget *button1;
    GtkWidget *button2;
    GtkWidget *button3;
    GtkWidget *button4;
    GtkWidget *layout_box;
    GtkWidget *button_box;
    GtkWidget *button_box2;
    GtkWidget *label1;
    GtkWidget *drawing_area;
    GtkWidget *combo1;
    GtkWidget *combo2;
    GtkWidget *switch1;

    window = gtk_application_window_new(app);
    static std::string window_title = "Network " + get_network_title(g_net);
    gtk_window_set_title(GTK_WINDOW(window), window_title.c_str());
    // gtk_window_set_default_size(GTK_WINDOW(window), 400, 400);

    layout_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), layout_box);

    button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_margin_start(button_box, 10);    
    gtk_widget_set_margin_top(button_box, 10);
    // gtk_widget_set_vexpand(button_box, true);
    gtk_container_add(GTK_CONTAINER(layout_box), button_box);

    button1 = gtk_button_new_with_label("Start");
    g_signal_connect(button1, "clicked", G_CALLBACK(on_start_button_clicked), NULL);
    gtk_container_add(GTK_CONTAINER(button_box), button1);

    button2 = gtk_button_new_with_label("Clear");
    g_signal_connect(button2, "clicked", G_CALLBACK(on_clear_button_clicked), NULL);
    gtk_container_add(GTK_CONTAINER(button_box), button2);

    button3 = gtk_button_new_with_label("Reset");
    g_signal_connect(button3, "clicked", G_CALLBACK(on_reset_button_clicked), NULL);
    gtk_container_add(GTK_CONTAINER(button_box), button3);

    button4 = gtk_button_new_with_label("RGN");
    g_signal_connect(button4, "clicked", G_CALLBACK(on_rng_button_clicked), NULL);
    gtk_container_add(GTK_CONTAINER(button_box), button4);

    label1 = gtk_label_new("epoches: ");
    gtk_widget_set_halign(label1, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(layout_box), label1);

    button_box2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    //gtk_button_box_set_layout((GtkButtonBox *)button_box2, GTK_BUTTONBOX_START);
    gtk_widget_set_margin_start(button_box2, 10);
    gtk_container_add(GTK_CONTAINER(layout_box), button_box2);


    combo1 = gtk_combo_box_text_new();
    for(int i = 0; i < ACTIVATION_BUNDLES_COUNT; i++){
        gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo1), NULL, activation_bundles[i].name);
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo1), 0);
    g_signal_connect(combo1, "changed", G_CALLBACK(on_combo1_changed), NULL);
    gtk_container_add(GTK_CONTAINER(button_box2), combo1);

    combo2 = gtk_combo_box_text_new();
    for(int i=0; i < LEARNING_RATES_COUNT; i++){
        gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo2), NULL, g_learning_rates[i].label);
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo2), LEARNING_RATE_DEFAULT_INDEX);
    g_signal_connect(combo2, "changed", G_CALLBACK(on_combo2_changed), NULL);
    gtk_container_add(GTK_CONTAINER(button_box2), combo2);


    GtkWidget * labelSleep = gtk_label_new("slp:");
    gtk_container_add(GTK_CONTAINER(button_box2), labelSleep);

    switch1 = gtk_switch_new();
    gtk_switch_set_active((GtkSwitch*)switch1, true);
    gtk_widget_set_halign(switch1, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(switch1, GTK_ALIGN_CENTER);
    gtk_container_add(GTK_CONTAINER(button_box2), switch1);
    g_signal_connect(switch1, "notify::active", G_CALLBACK(on_switch1_changed), NULL);


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

    gtk_window_set_icon_from_file((GtkWindow*)window, "gui_icon.png", NULL);

    // redraw timer
    g_timeout_add(30, (GSourceFunc)timeout_redraw, drawing_area);

    // update label timer
    g_timeout_add(100, (GSourceFunc)timeout_label, label1);

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