/*
 * Copyright © 2015 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 2 or 3,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef MIR_TOOLKIT_MIR_BUFFER_STREAM_H_
#define MIR_TOOLKIT_MIR_BUFFER_STREAM_H_

#include <mir_toolkit/mir_native_buffer.h>
#include <mir_toolkit/client_types.h>
#include <mir_toolkit/deprecations.h>

#ifdef __cplusplus
/**
 * \addtogroup mir_toolkit
 * @{
 */
extern "C" {
#endif

/**
 * Test for a valid buffer stream
 *
 * \param [in] buffer_stream  The buffer stream
 * \return                 True if the supplied buffer_stream is valid, or
 *                         false otherwise.
 */
bool mir_buffer_stream_is_valid(MirBufferStream *buffer_stream);

/**
 * Retrieve a text description of the error. The returned string is owned by
 * the library and remains valid until the stream or the associated
 * connection has been released.
 *   \param [in] buffer_stream  The buffer stream
 *   \return              A text description of any error resulting in an
 *                        invalid stream, or the empty string "" if the
 *                        connection is valid.
 */
char const *mir_buffer_stream_get_error_message(MirBufferStream *buffer_stream);

/**
 * Create a new buffer stream unattached to a surface and wait for the result. 
 * The resulting buffer stream may be used with 
 * mir_cursor_configuration_from_buffer_stream in order to post images 
 * to the system cursor.
 *
 * \deprecated Use mir_render_surface_get_buffer_stream instead
 * \param [in] connection       A valid connection
 * \param [in] width            Requested buffer width
 * \param [in] height           Requested buffer height
 * \param [in] format         Requested pixel format
 * \param [in] buffer_usage     Requested buffer usage, use
 *                              mir_buffer_usage_software for cursor image streams
 *
 * \return                      The new buffer stream. This is guaranteed non-null, 
 *                              but may be invalid in the case of error.
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
MirBufferStream* mir_connection_create_buffer_stream_sync(MirConnection *connection,
    int width, int height,
    MirPixelFormat format,
    MirBufferUsage buffer_usage)
MIR_FOR_REMOVAL_IN_VERSION_1("Use mir_render_surface_get_buffer_stream instead");
#pragma GCC diagnostic pop
/**
 * Release the supplied stream and any associated buffer. The returned wait
 * handle remains valid until the connection to the server is released.
 *
 *   \deprecated A stream obtained through mir_render_surface_get_buffer_stream does not need to be released.
 *   \warning callback could be called from another thread. You must do any
 *            locking appropriate to protect your data accessed in the
 *            callback.
 *   \param [in] buffer_stream  The stream
 *   \param [in] callback       Callback function to be invoked when the request
 *                              completes
 *   \param [in,out] context    User data passed to the callback function
 *   \return                    A handle that can be passed to mir_wait_for
 */
MirWaitHandle *mir_buffer_stream_release(
    MirBufferStream * buffer_stream,
    MirBufferStreamCallback callback,
    void *context)
MIR_FOR_REMOVAL_IN_VERSION_1("A stream obtained through mir_render_surface_get_buffer_stream() does not need to be released.");

/**
 * Release the specified buffer stream like in mir,_buffer_stream_release(), 
 * but also wait for the operation to complete.
 *   \deprecated A stream obtained through mir_render_surface_get_buffer_stream does not need to be released.
 *   \param [in] buffer_stream  The buffer stream to be released
 */
void mir_buffer_stream_release_sync(MirBufferStream *buffer_stream)
MIR_FOR_REMOVAL_IN_VERSION_1("A stream obtained through mir_render_surface_get_buffer_stream() does not need to be released.");

/**
 * Retrieve the current buffer in "raw" representation.
 *   \deprecated Use platform specific platform extensions instead
 *   \pre                         The buffer stream is valid
 *   \param [in]  buffer_stream   The buffer stream
 *   \param [out] buffer_package  Structure to be populated
 */
void mir_buffer_stream_get_current_buffer(MirBufferStream *buffer_stream,
    MirNativeBuffer **buffer_package)
MIR_FOR_REMOVAL_IN_VERSION_1("Use platform specific buffer extensions instead");

/**
 * Advance a buffer stream's buffer. The returned handle remains valid until the
 * next call to mir_buffer_stream_swap_buffers, until the buffer stream has been 
 * released or the connection to the server has been released.
 *   \warning callback could be called from another thread. You must do any
 *            locking appropriate to protect your data accessed in the
 *            callback.
 *   \param [in] buffer_stream      The buffer stream
 *   \param [in] callback     Callback function to be invoked when the request
 *                            completes
 *   \param [in,out] context  User data passed to the callback function
 *   \return                  A handle that can be passed to mir_wait_for
 */
MirWaitHandle *mir_buffer_stream_swap_buffers(
    MirBufferStream *buffer_stream,
    MirBufferStreamCallback callback,
    void *context)
MIR_FOR_REMOVAL_IN_VERSION_1("For non-blocking swaps use mir_buffer_stream_swap_buffers_sync with an interval of zero."
                             " And a client API for manual vsync will soon be available.");

/**
 * Advance a buffer stream's buffer as in mir_buffer stream_swap_buffers(), 
 * but also wait for the operation to complete.
 *   \param [in] buffer_stream  The buffer stream whose buffer to advance
 */
void mir_buffer_stream_swap_buffers_sync(MirBufferStream *buffer_stream);

/**
 * Retrieve a buffer stream's graphics region
 *   \warning Depending on platform, this can map the graphics buffer each
 *            time its called. The region remains mapped until
 *            mir_buffer_stream_swap_buffers().
 *   \pre                          The buffer stream is valid
 *   \param [in] buffer_stream     The buffer stream
 *   \param [out] graphics_region  Structure to be populated
 *   \return                       True if the region is valid, false otherwise.
 */
bool mir_buffer_stream_get_graphics_region(
    MirBufferStream *buffer_stream,
    MirGraphicsRegion *graphics_region);

/**
 * Retrieve a window type which may be used by EGL.
 *   \deprecated Use MirRenderSurface *as the window type
 *   \param [in] buffer_stream The buffer stream
 *   \return                   An EGLNativeWindowType that the client can use
 */
MirEGLNativeWindowType mir_buffer_stream_get_egl_native_window(MirBufferStream *buffer_stream)
MIR_FOR_REMOVAL_IN_VERSION_1("Use MirRenderSurface *as the window type");

/**
 * Set the scale associated with all buffers in the stream
 * \deprecated
 * \param [in] buffer_stream The buffer stream
 * \param [in] scale         The scale
 * \return                  A handle that can be passed to mir_wait_for
 */
MirWaitHandle *mir_buffer_stream_set_scale(MirBufferStream* buffer_stream, float scale)
MIR_FOR_REMOVAL_IN_VERSION_1("Functionality replaced by the introduction of MirRenderSurface");

/**
 * Set the swapinterval for the stream.
 *   \warning EGL users should use eglSwapInterval directly.
 *   \param [in] stream   The buffer stream
 *   \param [in] interval The number of vblank signals that
 *                        mir_buffer_stream_swap_buffers will wait for
 *   \return              A wait handle that can be passed to mir_wait_for,
 *                        or NULL if the interval could not be supported
 */
MirWaitHandle* mir_buffer_stream_set_swapinterval(MirBufferStream* stream, int interval);

/**
 * Query the swapinterval that the stream is operating with.
 * The default interval is 1.
 *   \param [in] stream   The buffer stream
 *   \return              The swapinterval value that the client is operating with.
 *                        Returns -1 if stream is invalid.
 */
int mir_buffer_stream_get_swapinterval(MirBufferStream* stream);

/**
 * Query the approximate time interval in microseconds until the next vblank
 * for a given buffer stream (actually the next vblank for the monitor deemed
 * most relevant to the window using the buffer stream). The result of
 * (current_time + mir_buffer_stream_get_microseconds_till_vblank()) is the
 * precise time at which the client should start rendering the next frame (or
 * at least when it should sample its inputs/scene) so as to produce perfectly
 * smooth rendering.
 *
 * \note  This function is only needed for streams that have been configured
 *        with a swap interval of zero. Streams with non-zero swap intervals
 *        already have accurate synchronization and throttling built in to the
 *        mir_buffer_stream_swap_buffers_sync() function.
 *
 *   \param [in] stream   The buffer stream
 *   \return              Time in microseconds to the next vblank for the
 *                        given buffer stream (may be as low as zero).
 */
unsigned long mir_buffer_stream_get_microseconds_till_vblank(MirBufferStream const* stream);

/**
 * Set the physical size of the buffers provided by the buffer stream.
 *
 * \warning: This does not affect the size of the current buffer.
 *           The next buffer after calling mir_buffer_stream_swap_buffers
 *           will have the designated size.
 *
 * \param [in] stream   The buffer stream
 * \param [in] width    The desired physical width
 * \param [in] height   The desired physical height
 */
void mir_buffer_stream_set_size(MirBufferStream* stream, int width, int height);

/**
 * Get the physical size of the next buffer that will be provided by the stream.
 *
 * \param [in]  stream   The buffer stream
 * \param [out] width    The physical width of the stream
 * \param [out] height   The physical height of the stream
 */
void mir_buffer_stream_get_size(MirBufferStream* stream, int* width, int* height);

#ifdef __cplusplus
}
/**@}*/
#endif

#endif // MIR_TOOLKIT_MIR_BUFFER_STREAM_H_
