/*
 * GStreamer
 * Copyright (C) 2005 Thomas Vander Stichele <thomas@apestaart.org>
 * Copyright (C) 2005 Ronald S. Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) 2023 nitansh <<user@hostname.org>>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Alternatively, the contents of this file may be used under the
 * GNU Lesser General Public License Version 2.1 (the "LGPL"), in
 * which case the following provisions apply instead of the ones
 * mentioned above:
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/**
 * SECTION:element-myfilter
 *
 * FIXME:Describe myfilter here.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! myfilter ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>

#include "gstmyfilter.h"

GST_DEBUG_CATEGORY_STATIC (gst_my_filter_debug);
#define GST_CAT_DEFAULT gst_my_filter_debug

/* Filter signals and args */
enum
{
  /* FILL ME */
  LAST_SIGNAL
};

enum
{
  PROP_0,
  PROP_SILENT
};

/* FIR Low pass filter cofficients */
double h[] = {
  2.168404344971009e-19,  -3.4149908541622475e-06,  5.0289162501205464e-05,
  8.818047498384203e-05,  -0.0001338323053923082,  -0.0003684296467048151,  5.778822583373967e-05,
  0.0008350486376658562,  0.00044584352884337563,  -0.001255768324672679,  -0.0015862577698064191, 
  0.0011308744867137997,  0.0032933394905967917,  0.0002064797027041942,  -0.004990454152616308, 
  -0.0032906359901750553,  0.005508824877680051,  0.008075337361342395,  -0.0032830033228053996,
  -0.013504396513306,  -0.003142321029233854,  0.017309078204101977,  0.014433272663436384, 
  -0.016129167278047637,  -0.029894917720004292,  0.005718108676486315,  0.04730753284730599, 
   0.019938686748843915,  -0.06333889770811145,  -0.07691360525051384,  0.07445903601495721,  
   0.3085797859752966,  0.30857978597529667,  0.30607165932120345,  0.07325182900790751,  
  -0.07504626542630366,  -0.061290071056135174,  0.019132326828822002,  0.045009024904287175,  
  0.005393293662249214,  -0.027948121517539302,  -0.014942656727975609,  0.013247458887885694,  
  0.015735014935198608,  -0.00282828046093806,  -0.012029712602719338,  -0.0028930801011914343,  
  0.007035981980381442,  0.004742632611074658,  -0.002797054549370543,  -0.004184168427760433,  
  0.00017055978747486662,  0.0026760842115849475,  0.0009021118708581258,  -0.0012387982740560465,  
 -0.0009563627193121765,  0.00032923268372137823,  0.0005927278571015568,  3.887915462853265e-05,  
 -0.00022932471015630075,  -7.357446505070843e-05,  3.852641877642543e-05,  1.2440028221224353e-05,  
  5.275146923934764e-20
  };
  

/* the capabilities of the inputs and outputs.
 *
 * describe the real formats here.
 */
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (
    "audio/x-raw, "
      "format = (string)F64LE,"
      "channels = (int) { 1, 2 }, "
      "rate = (int)48000"
      )
    );

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (
    "audio/x-raw, "
      "format = (string)F64LE,"
      "channels = (int) { 1, 2 }, "
      "rate = (int)48000"
      )
    );

#define gst_my_filter_parent_class parent_class
G_DEFINE_TYPE (GstMyFilter, gst_my_filter, GST_TYPE_ELEMENT);

GST_ELEMENT_REGISTER_DEFINE (my_filter, "my_filter", GST_RANK_NONE,
    GST_TYPE_MYFILTER);

static void gst_my_filter_set_property (GObject * object,
    guint prop_id, const GValue * value, GParamSpec * pspec);
static void gst_my_filter_get_property (GObject * object,
    guint prop_id, GValue * value, GParamSpec * pspec);

static gboolean gst_my_filter_sink_event (GstPad * pad,
    GstObject * parent, GstEvent * event);
static GstFlowReturn gst_my_filter_chain (GstPad * pad,
    GstObject * parent, GstBuffer * buf);

/* GObject vmethod implementations */

/* initialize the myfilter's class */
static void
gst_my_filter_class_init (GstMyFilterClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;

  gobject_class->set_property = gst_my_filter_set_property;
  gobject_class->get_property = gst_my_filter_get_property;

  g_object_class_install_property (gobject_class, PROP_SILENT,
      g_param_spec_boolean ("silent", "Silent", "Produce verbose output ?",
          FALSE, G_PARAM_READWRITE));

  gst_element_class_set_details_simple (gstelement_class,
      "MyFilter",
      "FIXME:Generic",
      "FIXME:Generic Template Element", "nitansh <<user@hostname.org>>");

  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&src_factory));
  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&sink_factory));
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad callback functions
 * initialize instance structure
 */
static void
gst_my_filter_init (GstMyFilter * filter)
{
  filter->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");
  gst_pad_set_event_function (filter->sinkpad,
      GST_DEBUG_FUNCPTR (gst_my_filter_sink_event));
  gst_pad_set_chain_function (filter->sinkpad,
      GST_DEBUG_FUNCPTR (gst_my_filter_chain));
  GST_PAD_SET_PROXY_CAPS (filter->sinkpad);
  gst_element_add_pad (GST_ELEMENT (filter), filter->sinkpad);

  filter->srcpad = gst_pad_new_from_static_template (&src_factory, "src");
  GST_PAD_SET_PROXY_CAPS (filter->srcpad);
  gst_element_add_pad (GST_ELEMENT (filter), filter->srcpad);

  filter->silent = FALSE;
}

static void
gst_my_filter_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstMyFilter *filter = GST_MYFILTER (object);

  switch (prop_id) {
    case PROP_SILENT:
      filter->silent = g_value_get_boolean (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_my_filter_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstMyFilter *filter = GST_MYFILTER (object);

  switch (prop_id) {
    case PROP_SILENT:
      g_value_set_boolean (value, filter->silent);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

/* GstElement vmethod implementations */

/* this function handles sink events */
static gboolean
gst_my_filter_sink_event (GstPad * pad, GstObject * parent,
    GstEvent * event)
{
  GstMyFilter *filter;
  gboolean ret;

  filter = GST_MYFILTER (parent);

  GST_LOG_OBJECT (filter, "Received %s event: %" GST_PTR_FORMAT,
      GST_EVENT_TYPE_NAME (event), event);

  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_CAPS:
    {
      GstCaps *caps;

      gst_event_parse_caps (event, &caps);
      /* do something with the caps */

      /* and forward */
      ret = gst_pad_event_default (pad, parent, event);
      break;
    }
    default:
      ret = gst_pad_event_default (pad, parent, event);
      break;
  }
  return ret;
}

/* chain function
 * this function does the actual processing
 */
static GstFlowReturn
gst_my_filter_chain (GstPad * pad, GstObject * parent, GstBuffer * buf)
{
  GstMyFilter *filter;

  filter = GST_MYFILTER (parent);

  GstBuffer *outbuf;
  GstBuffer *filtered_buf;

  GstCaps *caps = gst_pad_get_current_caps(pad);
    const gchar *caps_string = gst_caps_to_string(caps);
    gint num_channels = -1;
    gboolean is_stereo = FALSE;

    if (gst_structure_has_name(gst_caps_get_structure(caps, 0), "audio/x-raw")) {
        gst_structure_get_int(gst_caps_get_structure(caps, 0), "channels", &num_channels);
        is_stereo = (num_channels == 2);
    }

    g_print("New pad with caps %s, num_channels %d, is_stereo %s\n", caps_string, num_channels, is_stereo ? "TRUE" : "FALSE");

    gst_caps_unref(caps);

  outbuf = gst_buffer_copy_deep (buf);
  gst_buffer_unref (buf);
  if (!outbuf) {
    /* something went wrong - signal an error */
    GST_ELEMENT_ERROR (GST_ELEMENT (filter), STREAM, FAILED, (NULL), (NULL));
    return GST_FLOW_ERROR;
  }

  GstMapInfo map;
  if (gst_buffer_map(outbuf, &map, GST_MAP_READ)) {
      gsize dest_size;
      gpointer dest;
      int outsize = 0;
      gst_buffer_extract_dup(outbuf, 0, map.size, &dest, &dest_size);
      // use the extracted data
      double *y = convolve((double*)dest, dest_size/(8*num_channels), num_channels, &outsize);
      filtered_buf = gst_buffer_new_wrapped((gpointer)y, outsize*8*num_channels);
      g_free(dest);
      gst_buffer_unmap(outbuf, &map);
  }

  return gst_pad_push (filter->srcpad, filtered_buf);
}

double* convolve(double x[], int lenX, gint channels, int *outsize)
{
  int lenH = 64;
  int nconv = lenH+lenX-1;
  *outsize = nconv;
  int i,j,h_start,x_start,x_end,ch=0;

  double *y = (double*) calloc(channels*nconv, sizeof(double));
  
  if(channels == 2)
  {
    for (i=0; i<nconv; i++)
    {
      x_start = MAX(0,i-lenH+1);
      x_end   = MIN(i+1,lenX);
      h_start = MIN(i,lenH-1);
      for(j=x_start; j<x_end; j++)
      {
        y[2*i] += h[h_start--]*x[2*j];
      }
    }

    for (i=0; i<nconv; i++)
    {
      x_start = MAX(0,i-lenH+1);
      x_end   = MIN(i+1,lenX);
      h_start = MIN(i,lenH-1);
      for(j=x_start; j<x_end; j++)
      {
        y[2*i+1] += h[h_start--]*x[2*j+1];
      }
    }
  }else{
    for (i=0; i<nconv; i++)
    {
      x_start = MAX(0,i-lenH+1);
      x_end   = MIN(i+1,lenX);
      h_start = MIN(i,lenH-1);
      for(j=x_start; j<x_end; j++)
      {
        y[i] += h[h_start--]*x[j];
      }
    }
  }

  return y;
}


/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean
myfilter_init (GstPlugin * myfilter)
{
  /* debug category for filtering log messages
   *
   * exchange the string 'Template myfilter' with your description
   */
  GST_DEBUG_CATEGORY_INIT (gst_my_filter_debug, "myfilter",
      0, "Template myfilter");

  return GST_ELEMENT_REGISTER (my_filter, myfilter);
}

/* PACKAGE: this is usually set by meson depending on some _INIT macro
 * in meson.build and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use meson to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "myfirstmyfilter"
#endif

/* gstreamer looks for this structure to register myfilters
 *
 * exchange the string 'Template myfilter' with your myfilter description
 */
GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    myfilter,
    "my_filter",
    myfilter_init,
    PACKAGE_VERSION, GST_LICENSE, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)
