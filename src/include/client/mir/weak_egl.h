/*
 * EGL without any linkage requirements!
 * ~~~
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
 * Authored by: Daniel van Vugt <daniel.van.vugt@canonical.com>
 */

#include <EGL/egl.h>
#include <dlfcn.h>

namespace mir { namespace client {

class WeakEGL
{
public:
    WeakEGL()
    {
        egl1 = dlopen("libEGL.so.1", RTLD_NOLOAD|RTLD_LAZY);
    }

    ~WeakEGL()
    {
        if (egl1)
            dlclose(egl1);
    }

    EGLBoolean eglGetConfigAttrib(EGLDisplay dpy, EGLConfig config,
                                  EGLint attribute, EGLint *value)
    {
        if (find("eglGetConfigAttrib", (void**)&pGetConfigAttrib))
            return pGetConfigAttrib(dpy, config, attribute, value);
        else
            return EGL_FALSE;
    }

private:
    bool find(char const* name, void **func)
    {
        if (!*func)
        {
            // RTLD_DEFAULT is first choice to support wrappers like MockEGL
            *func = dlsym(RTLD_DEFAULT, name);

            // This will work more in real-world situations if the library
            // is hidden behind an RTLD_LOCAL...
            if (!*func)
                *func = dlsym(egl1, name);
        }
        return *func != nullptr;
    }

    void* egl1;
    EGLBoolean (*pGetConfigAttrib)(EGLDisplay dpy, EGLConfig config,
                                   EGLint attribute, EGLint *value) = nullptr;
};

}} // namespace mir::client
