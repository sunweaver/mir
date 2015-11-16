/*
 * Copyright © 2015 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 3,
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
 * Authored by: Kevin DuBois <kevin.dubois@canonical.com>
 */

#ifndef MIR_GRAPHICS_EGL_SYNC_FENCE_H_
#define MIR_GRAPHICS_EGL_SYNC_FENCE_H_

#include "egl_extensions.h"
#include <memory>
#include <chrono>
#include <mutex>

namespace mir
{
namespace graphics
{
class CommandStreamSync
{
public:
    //insert a sync object into the GL command stream of the current context.
    // \warning the calling thread should have a current egl context and display
    virtual void raise() = 0;
    // remove fence without waiting.
    virtual void reset() = 0;
    //wait for fence.
    // \ param [in] ns  The amount of time to wait for the fence to become signalled
    // \ returns        true if the fence was signalled, false if timeout
    virtual bool clear_or_timeout_after(std::chrono::nanoseconds ns) = 0;

    virtual ~CommandStreamSync() = default;
    CommandStreamSync() = default; 
    CommandStreamSync(CommandStreamSync const&) = delete;
    CommandStreamSync& operator=(CommandStreamSync const&) = delete;
};

class NullCommandSync : public CommandStreamSync
{
    void raise() override;
    void reset() override;
    bool clear_or_timeout_after(std::chrono::nanoseconds ns) override;
};

class EGLSyncFence : public CommandStreamSync
{
public:
    EGLSyncFence(std::shared_ptr<EGLSyncExtensions> const&);
    ~EGLSyncFence();

    void raise() override;
    void reset() override;
    bool clear_or_timeout_after(std::chrono::nanoseconds ns) override;
private:
    void reset(std::unique_lock<std::mutex> const&);
    bool clear_or_timeout_after(std::unique_lock<std::mutex> const&, std::chrono::nanoseconds ns);

    std::shared_ptr<EGLSyncExtensions> const egl;
    std::chrono::nanoseconds const default_timeout{
        std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::milliseconds(1))};

    std::mutex mutex;
    EGLDisplay fence_display;
    EGLSyncKHR sync_point;
};

}
}

#endif /* MIR_GRAPHICS_EGL_SYNC_FENCE_H_ */
