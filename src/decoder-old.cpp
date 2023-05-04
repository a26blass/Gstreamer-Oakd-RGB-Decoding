#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <iostream>
#include <filesystem>
#include <csignal>
#include <typeinfo>
#include "depthai/depthai.hpp"

typedef std::shared_ptr<dai::DataOutputQueue> DaiOutQueue;
typedef struct pipeline_frame pframe_t;
struct pipeline_frame {
    DaiOutQueue queue;
    GstElement *source;
};


static void on_eos(GstBus *bus, GstMessage *msg, gpointer data) {
    GMainLoop *loop = (GMainLoop *) data;
    g_print("Reached end of stream.\n");
    g_main_loop_quit(loop);
}

static gboolean on_error(GstBus *bus, GstMessage *message, gpointer data) {
    GError *err;
    gchar *debug_info;

    gst_message_parse_error(message, &err, &debug_info);
    g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME (message->src), err->message);
    g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
    g_error_free(err);
    g_free(debug_info);
    g_main_loop_quit((GMainLoop*)data);

    return TRUE;
}

static void info (GstBus *bus, GstMessage *msg, gpointer data) {
    g_print("DEBUG INFO: %s\n", GST_OBJECT_NAME (msg->src));
}

static gboolean decode(gpointer data) {
    pframe_t *frame = (pframe_t *)(data);
    DaiOutQueue q = frame->queue;
    GstElement *source = frame->source;

    // Get the next H.265 frame from the output queue
    auto packet_ptr = q->tryGet<dai::ImgFrame>();
    if (!packet_ptr) {
        return TRUE;
    }
    
    auto packet = *packet_ptr;

    // Construct a GstBuffer from the frame data
    GstBuffer *buffer = gst_buffer_new_wrapped((char*)packet.getData().data(), packet.getData().size());

    // Push the buffer to the appsrc element
    GstFlowReturn ret = gst_app_src_push_buffer(GST_APP_SRC(source), buffer);

    if (ret != GST_FLOW_OK) {
        g_printerr("Error pushing buffer to appsrc: %s\n", gst_flow_get_name(ret));
        return FALSE;
    }

    // Return TRUE to keep the loop running
    return TRUE;
}





// Keyboard interrupt (Ctrl + C) detected
static std::atomic<bool> alive{true};
static void sigintHandler(int signum) {
    alive = false;
}


int main(int argc, char** argv) {
    using namespace std;
    std::signal(SIGINT, &sigintHandler);

    // Create pipeline
    dai::Pipeline dai_pipeline;

    // Define sources and outputs
    auto camRgb = dai_pipeline.create<dai::node::ColorCamera>();
    auto videoEnc = dai_pipeline.create<dai::node::VideoEncoder>();
    auto xout = dai_pipeline.create<dai::node::XLinkOut>();

    xout->setStreamName("h265");

    camRgb->setBoardSocket(dai::CameraBoardSocket::RGB);
    camRgb->setResolution(dai::ColorCameraProperties::SensorResolution::THE_4_K);
    videoEnc->setDefaultProfilePreset(30, dai::VideoEncoderProperties::Profile::H265_MAIN);

    // Linking
    camRgb->video.link(videoEnc->input);
    videoEnc->bitstream.link(xout->input);

    // Connect to device and start pipeline
    dai::Device device(dai_pipeline);

    // Output queue will be used to get the encoded data from the output defined above
    auto q = device.getOutputQueue("h265", 30, true);

    cout << "Press Ctrl+C to exit..." << endl;

    //Create GStreamer Elements
    GstElement *pipeline, *source, *parser, *queue, *decoder, *sink;
    GstBus *bus;
    GMainLoop *loop;
    GstStateChangeReturn ret;
    GstBuffer* buffer;

    /* Initialize GStreamer */
    gst_init(NULL, NULL);
    loop = g_main_loop_new(NULL, FALSE);

    /* Create elements */
    pipeline = gst_pipeline_new("out");
    source = gst_element_factory_make("appsrc", "source");
    parser = gst_element_factory_make("h265parse", "parser");
    decoder = gst_element_factory_make("avdec_h265", "decoder");
    queue = gst_element_factory_make("queue", "queue");
    sink = gst_element_factory_make("autovideosink", "video-output");

    if (!pipeline || !source || !decoder || !queue || !sink) {
        g_printerr("One or more elements could not be created. Exiting.\n");
        return -1;
    }

    /* Set pipeline to NULL state */
    ret = gst_element_set_state(pipeline, GST_STATE_NULL);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Failed to set pipeline to NULL state. Exiting.\n");
        return -1;
    }


    /* Set appsrc properties */
    pframe_t *frame = (pframe_t *)calloc(1, sizeof(pframe_t));
    frame->queue = q;
    frame->source = source;


    /* Link elements */
    if (!gst_element_link_many(source, parser, decoder, queue, sink, NULL)) {
        g_printerr("Failed to link elements...\n");
        return -1;
    }

    /* Add elements to the pipeline */
    gst_bin_add_many (GST_BIN (pipeline), source, parser, decoder, queue, sink, NULL);

    /* Set pipeline to the paused state */
    ret = gst_element_set_state(pipeline, GST_STATE_PAUSED);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Unable to set the pipeline to the paused state.\n");
        return -1;
    }

    /* Start playing */
    ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr ("Unable to set the pipeline to the playing state.\n");
        gst_object_unref (pipeline);
        return -1;
    }


    /* Add callback function to retrieve & decode OAK-D frames synchronously */
    guint timeout_id = g_timeout_add(33, &decode, frame);


    /* Set up bus for handling messages */
    bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
    gst_bus_add_signal_watch (bus);
    g_signal_connect (bus, "message::eos", G_CALLBACK (on_eos), loop);
    g_signal_connect (bus, "message::error", (GCallback)on_error, loop);
    g_signal_connect (bus, "message::info", (GCallback)info, loop);

    /* Main Loop */   
    g_main_loop_run(loop);
    /* Free resources */
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    free(frame);

    return 0;
}

