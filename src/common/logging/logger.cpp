/*
 * Copyright © 2014-2015 Canonical Ltd.
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
 * Authored by: Cemil Azizoglu <cemil.azizoglu@canonical.com>
 */

#include "mir/logging/dumb_console_logger.h"
#include "mir/logging/logger.h"

#include <mutex>
#include <cstdarg>
#include <cstdio>

namespace ml = mir::logging;

void ml::Logger::log(char const* component, Severity severity, char const* format, ...)
{
    auto const bufsize = 4096;
    va_list va;
    va_start(va, format);
    char message[bufsize];
    vsnprintf(message, bufsize, format, va);
    va_end(va);

    // Inefficient, but maintains API: Constructing a std::string for message/component.
    log(severity, std::string{message}, std::string{component});
}

namespace
{
std::mutex log_mutex;
std::shared_ptr<ml::Logger> the_logger;

std::shared_ptr<ml::Logger> get_logger()
{
    std::lock_guard<decltype(log_mutex)> lock{log_mutex};

    if (!the_logger)
        the_logger = std::make_shared<ml::DumbConsoleLogger>();

    return the_logger;
}
}

void ml::log(ml::Severity severity, const std::string& message, const std::string& component)
{
    auto const logger = get_logger();

    logger->log(severity, message, component);
}

void ml::set_logger(std::shared_ptr<Logger> const& new_logger)
{
    if (new_logger)
    {
        std::lock_guard<decltype(log_mutex)> lock{log_mutex};
        the_logger = new_logger;
    }
}

namespace mir
{
namespace logging
{
// For backwards compatibility (avoid breaking ABI)
void log(ml::Severity severity, std::string const& message)
{
    ml::log(severity, message, "");
}
}
}

