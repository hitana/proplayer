#include <QDebug>
#include "gstpipeline.h"

// vika
extern gboolean            gst_structure_get_fourcc            (const GstStructure *structure,
                                                                const gchar *fieldname,
                                                                guint32 *value);

GStreamerPipeline::GStreamerPipeline(int vidIx,
                   const QString &videoLocation,
                   const char *renderer_slot,
                   QObject *parent)
  : Pipeline(vidIx, videoLocation, renderer_slot, parent),
    m_source(NULL),
    m_decodebin(NULL),
    m_videosink(NULL),
    m_audiosink(NULL),
    m_audioconvert(NULL),
    m_audioqueue(NULL),
    m_loop(NULL),
    m_bus(NULL),
    m_pipeline(NULL)
{
    qDebug() << "constructor entered";

    m_incomingBufThread = new GstIncomingBufThread(this, this);
    m_outgoingBufThread = new GstOutgoingBufThread(this, this);

    QObject::connect(m_incomingBufThread, SIGNAL(finished()), this, SLOT(cleanUp()));

}

GStreamerPipeline::~GStreamerPipeline()
{
}

void GStreamerPipeline::Configure()
{
    qDebug() <<  "Configure entered";

    gst_init (NULL, NULL);

#ifdef Q_OS_WIN
    m_loop = g_main_loop_new (NULL, FALSE);
#endif

#if 0
    /* Create the elements */
    this->m_pipeline = gst_pipeline_new (NULL);
    if(this->m_videoLocation.isEmpty())
    {
        qDebug() << "No video file specified. Using video test source.";
        this->m_source = gst_element_factory_make ("videotestsrc", "testsrc");
    }
    else
    {
        this->m_source = gst_element_factory_make ("filesrc", "filesrc");
        g_object_set (G_OBJECT (this->m_source), "location", /*"video.avi"*/ m_videoLocation.toUtf8().constData(), NULL);
    }
    this->m_decodebin = gst_element_factory_make ("decodebin", "decodebin");
    this->m_videosink = gst_element_factory_make ("fakesink", "videosink");
    //this->m_audiosink = gst_element_factory_make ("alsasink", "audiosink");
    //this->m_audiosink = gst_element_factory_make ("autoaudiosink", "audiosink");  // vika
    //this->m_audioconvert = gst_element_factory_make ("audioconvert", "audioconvert");
    //this->m_audioqueue = gst_element_factory_make ("queue", "audioqueue");
    //this->m_playbin = gst_element_factory_make("playbin", "playbin"); // vika

    if (this->m_pipeline == NULL || this->m_source == NULL || this->m_decodebin == NULL ||
        this->m_videosink == NULL || this->m_audiosink == NULL || this->m_audioconvert == NULL || this->m_audioqueue == NULL)
        g_critical ("One of the GStreamer decoding elements is missing");

    /* Setup the pipeline */
    gst_bin_add_many (GST_BIN (this->m_pipeline), this->m_source, this->m_decodebin, this->m_videosink,
                      this->m_audiosink, this->m_audioconvert, this->m_audioqueue, /*videoqueue,*/ NULL);
    g_signal_connect (this->m_decodebin, "pad-added", G_CALLBACK (on_new_pad), this);

    /* Link the elements */
    gst_element_link (this->m_source, this->m_decodebin);
    gst_element_link (this->m_audioqueue, this->m_audioconvert);
    gst_element_link (this->m_audioconvert, this->m_audiosink);

    m_bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
    gst_bus_add_watch(m_bus, (GstBusFunc) bus_call, this);
    gst_object_unref(m_bus);

    gst_element_set_state (this->m_pipeline, GST_STATE_PAUSED);
#else

    //----------------------------
    // vika test
    // works in console:
    // gst-launch-1.0 playbin uri=file:///home/vq/atomic.ts
    // gst-launch-1.0 filesrc location=/home/vq/atomic.ts ! decodebin ! autovideosink

    // gst-launch-1.0 filesrc location=/Users/qa/Downloads/Paradise.mp4 ! decodebin ! autovideosink
/*
    if (this->m_videoLocation.isEmpty()) {
        LOG(LOG_VIDPIPELINE, Logger::Info, "No video file specified. Using video test source.");
        this->m_source = gst_element_factory_make ("videotestsrc", "testsrc");
    }
    else {
        this->m_source = gst_element_factory_make ("filesrc", "filesrc");
        g_object_set (G_OBJECT (this->m_source), "location", m_videoLocation.toUtf8().constData(), NULL);
    }
*/
    // works good, but plays in separate window
    //this->m_pipeline = gst_parse_launch ("playbin uri=file:///home/vq/atomic.ts", NULL);
#ifdef Q_OS_UNIX
    this->m_pipeline = gst_parse_launch ("filesrc location=/home/vq/atomic.ts ! decodebin ! autovideosink", NULL);
#endif
#ifdef Q_OS_MAC
    this->m_pipeline = gst_parse_launch ("filesrc location=/Users/qa/Desktop/media/atomic.ts ! decodebin ! autovideosink", NULL);
#endif
    // todo : windows ?

    // it works fine
    // no it hangs all system unlit reboot!!!!
/*
    this->m_source = gst_element_factory_make ("filesrc", "filesrc");
    this->m_pipeline = gst_pipeline_new ("pipeline");
    this->m_decodebin = gst_element_factory_make ("decodebin", "decodebin");
    this->m_videosink = gst_element_factory_make ("autovideosink", "videosink");
    //this->m_videosink = gst_element_factory_make ("fakesink", "videosink");
    //this->m_audioconvert = gst_element_factory_make ("audioconvert", "audioconvert");
    //this->m_audioqueue = gst_element_factory_make ("queue", "audioqueue");
    //this->m_audiosink = gst_element_factory_make ("autoaudiosink", "audiosink");

    if (this->m_pipeline == NULL)     g_critical ("One of the GStreamer decoding elements is missing - m_pipeline");
    if (this->m_source == NULL)       g_critical ("One of the GStreamer decoding elements is missing - m_source");
    if (this->m_decodebin == NULL)    g_critical ("One of the GStreamer decoding elements is missing - m_decodebin");
    if (this->m_videosink == NULL)    g_critical ("One of the GStreamer decoding elements is missing - m_videosink");
    //if (this->m_audioconvert == NULL) g_critical ("One of the GStreamer decoding elements is missing - m_audioconvert");
    //if (this->m_audioqueue == NULL)   g_critical ("One of the GStreamer decoding elements is missing - m_audioqueue");
    //if (this->m_audiosink == NULL)    g_critical ("One of the GStreamer decoding elements is missing - m_audiosink");

    //gst_bin_add_many (GST_BIN (this->m_pipeline), this->m_source, this->m_decodebin, this->m_videosink,
    //                  this->m_audiosink, this->m_audioconvert, this->m_audioqueue, NULL);

    gst_bin_add_many (GST_BIN (this->m_pipeline), this->m_source, this->m_decodebin, this->m_videosink, NULL);

    gst_element_link_many(this->m_source, this->m_decodebin, this->m_videosink);

    //g_signal_connect (this->m_decodebin, "pad-added", G_CALLBACK (on_new_pad), this);

    //gst_element_link (this->m_source, this->m_decodebin);
    //gst_element_link (this->m_audioqueue, this->m_audioconvert);
    //gst_element_link (this->m_audioconvert, this->m_audiosink);
*/
    m_bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
    gst_bus_add_watch(m_bus, (GstBusFunc) bus_call, this);
    gst_object_unref(m_bus);
/*
    g_object_set (G_OBJECT (this->m_source), "location", "/home/vq/atomic.ts", NULL);
*/
    //gst_element_set_state (this->m_pipeline, GST_STATE_PAUSED);
    gst_element_set_state (this->m_pipeline, GST_STATE_PLAYING);

    //----------------------------
/*
    // vika: decode with elecard avcdecoder
    // GST_PLUGIN_PATH=`pwd` gst-launch-1.0 filesrc location=~/Desktop/media/airshowD1_bp_1_5.264 ! h264parse ! eavcdec ! autovideosink
    this->m_source = gst_element_factory_make ("filesrc", "filesrc");
    this->m_pipeline = gst_pipeline_new (NULL);
    //this->m_decodebin = gst_element_factory_make ("eavcdec", "decoder");
    this->m_decodebin = gst_element_factory_make ("avdec_h264", "decoder");
    this->m_parser = gst_element_factory_make ("h264parse", "h264parse");
    //this->m_videosink = gst_element_factory_make ("fakesink", "videosink");
    this->m_videosink = gst_element_factory_make ("autovideosink", "videosink");

    if (this->m_pipeline == NULL)     g_critical ("One of the GStreamer decoding elements is missing - m_pipeline");
    if (this->m_source == NULL)       g_critical ("One of the GStreamer decoding elements is missing - m_source");
    if (this->m_decodebin == NULL)    g_critical ("One of the GStreamer decoding elements is missing - m_decodebin");
    if (this->m_videosink == NULL)    g_critical ("One of the GStreamer decoding elements is missing - m_videosink");
    if (this->m_parser == NULL)       g_critical ("One of the GStreamer decoding elements is missing - m_parser");


    gst_bin_add_many (GST_BIN (this->m_pipeline), this->m_source, this->m_parser, this->m_decodebin, this->m_videosink,
                      NULL);
    //g_signal_connect (this->m_decodebin, "pad-added", G_CALLBACK (on_new_pad), this);

    gst_element_link (this->m_source, this->m_parser);
    gst_element_link (this->m_parser, this->m_decodebin);
    gst_element_link (this->m_decodebin, this->m_videosink);

    m_bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
    gst_bus_add_watch(m_bus, (GstBusFunc) bus_call, this);
    gst_object_unref(m_bus);Paradise.mp4

    //g_object_set (G_OBJECT (this->m_source), "location", "/Users/qa/Desktop/media/airshowD1_bp_1_5.264", NULL);
    //g_object_set (G_OBJECT (this->m_source), "location", "/home/vq/Paradise.mp4", NULL);
    g_object_set (G_OBJECT (this->m_source), "location", "/home/vq/atomic.ts", NULL);
    gst_element_set_state (this->m_pipeline, GST_STATE_PAUSED);
    */
#endif // if 0
}

void GStreamerPipeline::Start()
{
    GstStateChangeReturn ret = gst_element_set_state(GST_ELEMENT(this->m_pipeline), GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE)
    {
        qDebug() <<  "Failed to start up pipeline!";

        /* check if there is an error message with details on the bus */
        GstMessage* msg = gst_bus_poll(this->m_bus, GST_MESSAGE_ERROR, 0);
        if (msg)
        {
            GError *err = NULL;
            gst_message_parse_error (msg, &err, NULL);
            qDebug() << "ERROR: %s", err->message;
            g_error_free (err);
            gst_message_unref (msg);
        }
        return;
    }

    // Start the threads:
    m_incomingBufThread->start();
    m_outgoingBufThread->start();
}

void GStreamerPipeline::Stop()
{
#ifdef Q_OS_WIN
    g_main_loop_quit(m_loop);
#else
    emit stopRequested();
#endif
}

void GStreamerPipeline::cleanUp()
{
    gst_element_set_state(GST_ELEMENT(this->m_pipeline), GST_STATE_NULL);

    // Wait for both threads to finish up
    m_incomingBufThread->wait(QUEUE_CLEANUP_WAITTIME_MS);
    m_outgoingBufThread->wait(QUEUE_CLEANUP_WAITTIME_MS);

    GstBuffer *buf;
    while(this->m_incomingBufQueue.size())
    {
        this->m_incomingBufQueue.get((void**)(&buf));
        gst_buffer_unref(buf);
    }
    while(this->m_outgoingBufQueue.size())
    {
        this->m_outgoingBufQueue.get((void**)(&buf));
        gst_buffer_unref(buf);
    }

    gst_object_unref(m_pipeline);

    // Done
    m_finished = true;
    emit finished(m_vidIx);
}

unsigned char* GStreamerPipeline::bufToVidDataStart(void *buf)
{
    //return GST_BUFFER_DATA(buf);   //vika : deprecated in gstreamer-1.0

    // todo: correctly allocate buf
    // todo : unmap and unref buf and map
    /*GstBuffer *buffer;
    GstMemory *mem;
    GstMapInfo info;
    gst_buffer_map (buf, &info, GST_MAP_READ | GST_MAP_WRITE);
      memset (info.data, 0xff, info.size);
      gst_buffer_unmap (buf, &info);
    gst_buffer_unref (buf);
*/

    GstMapInfo gstMapInfo;
    GstBuffer * gstBuf;
    qDebug() << "gst_buffer_map...";
    if (!gst_buffer_map ((GstBuffer*)buf, &gstMapInfo, (GstMapFlags)( GST_MAP_READ | GST_MAP_WRITE ))) {
        gst_buffer_unmap ((GstBuffer*)buf, &gstMapInfo);
        gst_buffer_unref ((GstBuffer*)buf);
        return NULL;
    }
    gst_buffer_unmap ((GstBuffer*)buf, &gstMapInfo);
    gst_buffer_unref ((GstBuffer*)buf);
    return (unsigned char *) gstMapInfo.data; // ?? will it be valid after unmap?

   // return NULL; // vika: todo : just stub
}

void GStreamerPipeline::on_new_pad(GstElement *element,
                     GstPad *pad,
                     GStreamerPipeline* p)
{
    GstPad *sinkpad;
    GstCaps *caps;
    GstStructure *str;

    Q_UNUSED(element);

    qDebug() << "IN\n";
    // vika
    caps = gst_pad_get_current_caps (pad);
    //caps = gst_pad_get_caps (pad);
    str = gst_caps_get_structure (caps, 0);

    if (g_strrstr (gst_structure_get_name (str), "video"))
    {
        // vika
        //sinkpad = gst_element_get_pad (p->m_videosink, "sink");
        sinkpad = gst_element_get_static_pad (p->m_videosink, "sink");
        qDebug() << "gst_element_get_static_pad m_videosink sink = " << sinkpad;

        g_object_set (G_OBJECT (p->m_videosink),
                      "sync", TRUE,
                      "signal-handoffs", TRUE,  // vika: autovideosink has no such property
                      NULL);
        g_signal_connect (p->m_videosink,
                          "preroll-handoff",
                          G_CALLBACK(on_gst_buffer),
                          p);
        g_signal_connect (p->m_videosink,
                          "handoff",
                          G_CALLBACK(on_gst_buffer),
                          p);
    }
    else {
        // vika
        //sinkpad = gst_element_get_pad (p->m_audioqueue, "sink");
        sinkpad = gst_element_get_static_pad (p->m_audioqueue, "sink");
        qDebug() <<  "gst_element_get_static_pad m_audiosink sink = " << sinkpad;
    }

  /*  // test for eavcdec
    sinkpad = gst_element_get_static_pad (p->m_decodebin, "sink");
    LOG(LOG_VIDPIPELINE, Logger::Debug2, "gst_element_get_static_pad sinkpad = %p\n", sinkpad);
    // end test
*/
    // vika
    if (!gst_pad_is_linked (sinkpad)) {
        if (gst_pad_link (pad, sinkpad) != GST_PAD_LINK_OK)
          g_error ("Failed to link pads!");
      }
    gst_object_unref (sinkpad);
    gst_caps_unref (caps);
}

/* fakesink handoff callback */
void GStreamerPipeline::on_gst_buffer(GstElement * element,
                        GstBuffer * buf,
                        GstPad * pad,
                        GStreamerPipeline* p)
{
    //LOG(LOG_VIDPIPELINE, Logger::Debug2, "vid %d, element=%p, buf=%p, pad=%p, p=%p, bufdata=%p\n",
    //               p->getVidIx(), element, buf, pad, p, GST_BUFFER_DATA(buf));

    //LOG(LOG_VIDPIPELINE, Logger::Debug2, "vid %d, element=%p, buf=%p, pad=%p, p=%p\n",
      //                 p->getVidIx(), element, buf, pad, p);

    Q_UNUSED(pad)
    Q_UNUSED(element)

    if(p->m_vidInfoValid == false)
    {
        qDebug() << "Received first frame of vid" << p->getVidIx();

        // vika
        GstCaps *caps = gst_pad_get_current_caps (pad);
        //GstCaps *caps = gst_pad_get_negotiated_caps (pad);

        if (caps)
        {
            qDebug() << "gst_pad_get_current_caps OK";

            GstStructure *structure = gst_caps_get_structure (caps, 0);
            gst_structure_get_int (structure, "width", &(p->m_width));
            gst_structure_get_int (structure, "height", &(p->m_height));

            // vika
            guint32 uiFourCC;
            //gst_structure_get_fourcc (structure, "format", &uiFourCC);
            gst_structure_get_uint(structure, "format", &uiFourCC);

            //LOG(LOG_VIDPIPELINE, Logger::Error, "name = %s, uiFourCC = 0x%X, w = %d, h = %d", gst_structure_get_name (structure), uiFourCC, p->m_width, p->m_height);
        }
        else
        {
            qDebug() <<  "on_gst_buffer() - Could not get caps for pad!";
        }

        p->m_colFormat = discoverColFormat(buf, pad); // returns invalid value
        //p->m_colFormat = discoverColFormat(buf);  // deprecated
        p->m_vidInfoValid = true;
    }

    // show the buffer
    // try this
   /* flag_str = gst_buffer_get_flags_string (buf);

        sink->last_message =
            g_strdup_printf ("chain   ******* (%s:%s) (%u bytes, dts: %s, pts: %s"
            ", duration: %s, offset: %" G_GINT64_FORMAT ", offset_end: %"
            G_GINT64_FORMAT ", flags: %08x %s) %p",
            GST_DEBUG_PAD_NAME (GST_BASE_SINK_CAST (sink)->sinkpad),
            (guint) gst_buffer_get_size (buf), dts_str, pts_str,
            dur_str, GST_BUFFER_OFFSET (buf), GST_BUFFER_OFFSET_END (buf),
            GST_MINI_OBJECT_CAST (buf)->flags, flag_str, buf);
        g_free (flag_str);
        GST_OBJECT_UNLOCK (sink);

        gst_fake_sink_notify_last_message (sink);
      }
*/

    // end
/*
    gchar dts_str[64], pts_str[64], dur_str[64];
    if (GST_BUFFER_DTS (buf) != GST_CLOCK_TIME_NONE) {
          g_snprintf (dts_str, sizeof (dts_str), "%" GST_TIME_FORMAT,
              GST_TIME_ARGS (GST_BUFFER_DTS (buf)));
        } else {
          g_strlcpy (dts_str, "none", sizeof (dts_str));
        }

        if (GST_BUFFER_PTS (buf) != GST_CLOCK_TIME_NONE) {
          g_snprintf (pts_str, sizeof (pts_str), "%" GST_TIME_FORMAT,
              GST_TIME_ARGS (GST_BUFFER_PTS (buf)));
        } else {
          g_strlcpy (pts_str, "none", sizeof (pts_str));
        }

        if (GST_BUFFER_DURATION (buf) != GST_CLOCK_TIME_NONE) {
          g_snprintf (dur_str, sizeof (dur_str), "%" GST_TIME_FORMAT,
              GST_TIME_ARGS (GST_BUFFER_DURATION (buf)));
        } else {
          g_strlcpy (dur_str, "none", sizeof (dur_str));
        }

    //gchar * flag_str = gst_buffer_get_flags_string (buf);

    LOG(LOG_VIDPIPELINE, Logger::Debug2,
        "chain   ******* (%u bytes, dts: %s, pts: %s, duration: %s, flags: %08x ) %p",
        (guint) gst_buffer_get_size (buf), dts_str, pts_str, dur_str,
        GST_MINI_OBJECT_CAST (buf)->flags,  buf);
   // g_free (flag_str);
    // offset: %" G_GINT64_FORMAT ", offset_end: %"G_GINT64_FORMAT "
    // GST_BUFFER_OFFSET (buf), GST_BUFFER_OFFSET_END (buf),

    GstMapInfo info;
    int i;
    gst_buffer_map (buf, &info, (GstMapFlags)( GST_MAP_READ ));
    //gst_util_dump_mem (info.data, info.size);

    LOG(LOG_VIDPIPELINE, Logger::Debug2, "Got buffer of size = %d bytes, info->size = %d.", gst_buffer_get_size(buf), info.size);
    if (info.size > 256){
        for (i=0; i<256; i+=8){
            LOG(LOG_VIDPIPELINE, Logger::Debug2, "%02X %02X %02X %02X %02X %02X %02X %02X",
                info.data[i], info.data[i+1], info.data[i+2], info.data[i+3], info.data[i+4], info.data[i+5], info.data[i+6], info.data[i+7]);
        }
    }
    gst_buffer_unmap(buf, &info);
    //gst_buffer_unref(buf);
    // end
*/
    /* ref then push buffer to use it in qt */
    gst_buffer_ref(buf);
    p->m_incomingBufQueue.put(buf);
    //LOG(LOG_VIDPIPELINE, Logger::Debug2, "vid %d pushed buffer %p to incoming queue", p->getVidIx(), buf);

    p->NotifyNewFrame();
}

gboolean GStreamerPipeline::bus_call(GstBus *bus, GstMessage *msg, GStreamerPipeline* p)
{
  Q_UNUSED(bus)

    switch(GST_MESSAGE_TYPE(msg))
    {
        case GST_MESSAGE_EOS:
            qDebug() <<"End-of-stream received. Stopping.";
            p->Stop();
        break;

        case GST_MESSAGE_ERROR:
        {
            gchar *debug = NULL;
            GError *err = NULL;
            gst_message_parse_error(msg, &err, &debug);
            qDebug() << "Error: %s", err->message;
            g_error_free (err);
            if(debug)
            {
                qDebug() <<  "Debug details: " << debug;
                g_free(debug);
            }
            p->Stop();
            break;
        }

        default:
            break;
    }

    return TRUE;
}

ColFormat GStreamerPipeline::discoverColFormat(GstBuffer * buf, GstPad * pad)
{
    // Edit for consistent style later
    //gchar *pTmp	 = NULL;
    GstCaps *pCaps	 = NULL;
    GstStructure *pStructure = NULL;
    gint iDepth;
    gint iBitsPerPixel;
    gint iRedMask;
    gint iGreenMask;
    gint iBlueMask;
    gint iAlphaMask;
    ColFormat ret = ColFmt_Unknown;

    //pTmp = gst_caps_to_string (GST_BUFFER_CAPS(buf));
    //LOG(LOG_VIDPIPELINE, Logger::Info, "%s", pTmp);
    //g_free (pTmp);

    qDebug() <<  "buffer-size in bytes: " << gst_buffer_get_size (buf);

    // vika
    pCaps = gst_pad_get_current_caps(pad);
    //pCaps = gst_buffer_get_caps (buf);

    pStructure = gst_caps_get_structure (pCaps, 0);

    if (gst_structure_has_name (pStructure, "video/x-raw-rgb")) // vika: todo : deprecated
    {
        gst_structure_get_int (pStructure, "bpp", &iBitsPerPixel);
        gst_structure_get_int (pStructure, "depth", &iDepth);
        gst_structure_get_int (pStructure, "red_mask", &iRedMask);
        gst_structure_get_int (pStructure, "green_mask", &iGreenMask);
        gst_structure_get_int (pStructure, "blue_mask", &iBlueMask);

        switch (iDepth)
        {
        case 24:
            if (iRedMask   == 0x00ff0000 &&
                iGreenMask == 0x0000ff00 &&
                iBlueMask  == 0x000000ff)
            {
                qDebug() << "format is RGB";
                ret = ColFmt_RGB888;
            }
            else if (iRedMask   == 0x000000ff &&
                     iGreenMask == 0x0000ff00 &&
                     iBlueMask  == 0x00ff0000)
            {
                qDebug() << "format is BGR";
                ret = ColFmt_BGR888;
            }
            else
            {
                qDebug() << "Unhandled 24 bit RGB-format";
            }
            break;

        case 32:
            gst_structure_get_int (pStructure, "alpha_mask", &iAlphaMask);
            if (iRedMask   == 0xff000000 &&
                iGreenMask == 0x00ff0000 &&
                iBlueMask  == 0x0000ff00)
            {
                qDebug() << "format is RGBA";
                ret = ColFmt_ARGB8888;
            }
            else if (iRedMask   == 0x00ff0000 &&
                     iGreenMask == 0x0000ff00 &&
                     iBlueMask  == 0x000000ff)
            {
                qDebug() << "format is BGRA";
                ret = ColFmt_BGRA8888;
            }
            else
            {
                qDebug() << "Unhandled 32 bit RGB-format";
            }
            break;

        default :
            qDebug() << "Unhandled RGB-format of depth" << iDepth;
            break;
        }
    }
    else //if (gst_structure_has_name (pStructure, "video/x-raw-yuv") / vika: deprecated
           if (gst_structure_has_name (pStructure, "video/x-raw"))
    {
        guint32 uiFourCC;

        // vika : removed deprecated
        //gst_structure_get_fourcc (pStructure, "format", &uiFourCC);
        //gst_structure_get_uint(pStructure, "format", &uiFourCC);

        // vika: todo : gst_structure_get_uint returns incorrect fourcc
        // and gst_structure_get_fourcc is somehow absent in MacOS X gstreamer-1.0 libs
        // so set it manually for first app version
        uiFourCC = GST_MAKE_FOURCC ('I', '4', '2', '0');

        switch (uiFourCC)
        {
        case GST_MAKE_FOURCC ('I', '4', '2', '0'):
            qDebug() <<  "I420" << uiFourCC;
            ret = ColFmt_I420;
            break;

        case GST_MAKE_FOURCC ('I', 'Y', 'U', 'V'):
            qDebug() <<  "IYUV (0x%X)" << uiFourCC;
            ret = ColFmt_IYUV;
            break;

        case GST_MAKE_FOURCC ('Y', 'V', '1', '2'):
            qDebug() << "YV12 (0x%X)" << uiFourCC;
            ret = ColFmt_YV12;
            break;

        case GST_MAKE_FOURCC ('Y', 'U', 'Y', 'V'):
            qDebug() <<  "YUYV (0x%X)" <<  uiFourCC;
            ret = ColFmt_YUYV;
            break;

        case GST_MAKE_FOURCC ('Y', 'U', 'Y', '2'):
            qDebug() <<  "YUY2 (0x%X)" << uiFourCC;
            ret = ColFmt_YUY2;
            break;

        case GST_MAKE_FOURCC ('V', '4', '2', '2'):
            qDebug() << "V422 (0x%X)" << uiFourCC;
            ret = ColFmt_V422;
            break;

        case GST_MAKE_FOURCC ('Y', 'U', 'N', 'V'):
            qDebug() <<  "YUNV (0x%X)" << uiFourCC;
            ret = ColFmt_YUNV;
            break;

        case GST_MAKE_FOURCC ('U', 'Y', 'V', 'Y'):
            qDebug() << "UYVY (0x%X)" << uiFourCC;
            ret = ColFmt_UYVY;
            break;

        case GST_MAKE_FOURCC ('Y', '4', '2', '2'):
            qDebug() <<  "Y422 (0x%X)" << uiFourCC;
            ret = ColFmt_Y422;
            break;

        case GST_MAKE_FOURCC ('U', 'Y', 'N', 'V'):
            qDebug() << "UYNV (0x%X)" << uiFourCC;
            ret = ColFmt_YUNV;
            break;

        default :
            qDebug() << "Unhandled YUV-format 0x%X" << uiFourCC;
            break;
        }
    }
    else
    {
        qDebug() << "Unsupported caps name " << gst_structure_get_name (pStructure);
    }

    gst_caps_unref (pCaps);
    pCaps = NULL;

    return ret;
}
/*
quint32 GStreamerPipeline::discoverFourCC(GstBuffer * buf)
{
    guint32       uiFourCC = 0;
    GstCaps*      pCaps	 = NULL;
    GstStructure* pStructure = NULL;

    pCaps = gst_buffer_get_caps (buf);
    pStructure = gst_caps_get_structure (pCaps, 0);

    if (gst_structure_has_name (pStructure, "video/x-raw-yuv"))
    {
        gst_structure_get_fourcc (pStructure, "format", &uiFourCC);
    }

    return (quint32)uiFourCC;
}
*/
void GstIncomingBufThread::run()
{
    qDebug() << "GStreamerPipeline: vid %d incoming buf thread started" <<
        m_pipelinePtr->getVidIx();

#ifndef Q_OS_WIN
    //works like the gmainloop on linux (GstEvent are handled)
    QObject::connect(m_pipelinePtr, SIGNAL(stopRequested()), this, SLOT(quit()));
    exec();
#else
    g_main_loop_run(m_loop);
#endif

    // Incoming handling is all done in the static on_gst_buffer callback

    qDebug() << "GStreamerPipeline: vid %d incoming buf thread finished" << m_pipelinePtr->getVidIx();
}


void GstOutgoingBufThread::run()
{
    qDebug() <<  "GStreamerPipeline: vid %d outgoing buf thread started", m_pipelinePtr->getVidIx();

    QObject::connect(m_pipelinePtr, SIGNAL(stopRequested()), this, SLOT(quit()));

    while(m_keepRunningOutgoingThread)
    {
        /* Pop then unref buffer we have finished using in qt,
           block here if queue is empty */
        GstBuffer *buf_old = NULL;
        if(m_pipelinePtr->m_outgoingBufQueue.get((void**)(&buf_old), QUEUE_THREADBLOCK_WAITTIME_MS))
        {
            if (buf_old)
            {
                gst_buffer_unref(buf_old);
                //LOG(LOG_VIDPIPELINE, Logger::Debug2, "GStreamerPipeline: vid %d popped buffer %p from outgoing queue",
                 //   m_pipelinePtr->getVidIx(), buf_old);
                //LOG(LOG_VIDPIPELINE, Logger::Debug2, "GStreamerPipeline: vid %d m_outgoingBufQueue size is = %d",
                 //   m_pipelinePtr->getVidIx(), m_pipelinePtr->m_outgoingBufQueue.size());
            }
        } 
    }

    qDebug() << "GStreamerPipeline: vid %d outgoing buf thread finished" << m_pipelinePtr->getVidIx();
}


