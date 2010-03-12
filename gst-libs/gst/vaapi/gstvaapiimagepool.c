/*
 *  gstvaapiimagepool.c - Gst VA image pool
 *
 *  gstreamer-vaapi (C) 2010 Splitted-Desktop Systems
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#include "config.h"
#include "gstvaapiimagepool.h"

#define DEBUG 1
#include "vaapi_debug.h"

G_DEFINE_TYPE(
    GstVaapiImagePool,
    gst_vaapi_image_pool,
    GST_VAAPI_TYPE_VIDEO_POOL);

#define GST_VAAPI_IMAGE_POOL_GET_PRIVATE(obj)                   \
    (G_TYPE_INSTANCE_GET_PRIVATE((obj),                         \
                                 GST_VAAPI_TYPE_IMAGE_POOL,	\
                                 GstVaapiImagePoolPrivate))

struct _GstVaapiImagePoolPrivate {
    GstVaapiImageFormat format;
    guint               width;
    guint               height;
};

static void
gst_vaapi_image_pool_set_caps(GstVaapiVideoPool *pool, GstCaps *caps)
{
    GstVaapiImagePoolPrivate * const priv = GST_VAAPI_IMAGE_POOL(pool)->priv;
    GstStructure *structure;
    gint width, height;

    structure = gst_caps_get_structure(caps, 0);
    gst_structure_get_int(structure, "width", &width);
    gst_structure_get_int(structure, "height", &height);

    priv->format        = gst_vaapi_image_format_from_caps(caps);
    priv->width         = width;
    priv->height        = height;
}

gpointer
gst_vaapi_image_pool_alloc_object(
    GstVaapiVideoPool *pool,
    GstVaapiDisplay   *display
)
{
    GstVaapiImagePoolPrivate * const priv = GST_VAAPI_IMAGE_POOL(pool)->priv;

    return gst_vaapi_image_new(display,
                               priv->format,
                               priv->width,
                               priv->height);
}

static void
gst_vaapi_image_pool_finalize(GObject *object)
{
    G_OBJECT_CLASS(gst_vaapi_image_pool_parent_class)->finalize(object);
}

static void
gst_vaapi_image_pool_class_init(GstVaapiImagePoolClass *klass)
{
    GObjectClass * const object_class = G_OBJECT_CLASS(klass);
    GstVaapiVideoPoolClass * const pool_class = GST_VAAPI_VIDEO_POOL_CLASS(klass);

    g_type_class_add_private(klass, sizeof(GstVaapiImagePoolPrivate));

    object_class->finalize      = gst_vaapi_image_pool_finalize;

    pool_class->set_caps        = gst_vaapi_image_pool_set_caps;
    pool_class->alloc_object    = gst_vaapi_image_pool_alloc_object;
}

static void
gst_vaapi_image_pool_init(GstVaapiImagePool *pool)
{
    GstVaapiImagePoolPrivate *priv = GST_VAAPI_IMAGE_POOL_GET_PRIVATE(pool);

    pool->priv          = priv;
    priv->format        = 0;
    priv->width         = 0;
    priv->height        = 0;
}

GstVaapiVideoPool *
gst_vaapi_image_pool_new(GstVaapiDisplay *display, GstCaps *caps)
{
    return g_object_new(GST_VAAPI_TYPE_IMAGE_POOL,
                        "display", display,
                        "caps",    caps,
                        NULL);
}