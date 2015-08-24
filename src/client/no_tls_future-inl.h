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

//Hybris and libc can have TLS collisions in the thread using OpenGL
//between std::promise and the GL dispatch.
//https://github.com/libhybris/libhybris/issues/212
//If this bug is resolved, switch to std::future and rm this file

#ifndef MIR_CLIENT_NO_TLS_FUTURE_INL_H_
#define MIR_CLIENT_NO_TLS_FUTURE_INL_H_

#include <memory>
#include <mutex>
#include <condition_variable>
#include <future>

namespace mir
{
namespace client
{
template<typename T>
class State
{
public:
    template<class Rep, class Period>
    std::future_status wait_for(std::chrono::duration<Rep, Period> const& timeout_duration) const
    {
        std::unique_lock<std::mutex> lk(mutex);
        if (cv.wait_for(lk, timeout_duration, [this]{ return set || broken; }))
            return std::future_status::ready;
        return std::future_status::timeout;
    }

    void set_value(T val)
    {
        std::lock_guard<std::mutex> lk(mutex);
        set = true;
        value = std::move(val);
        cv.notify_all();
    }

    T get_value()
    {
        std::unique_lock<std::mutex> lk(mutex);
        cv.wait(lk, [this]{ return set || broken; });
        if (broken)
            throw std::future_error(std::future_errc::broken_promise);
        return value; 
    }

    void break_promise()
    {
        std::lock_guard<std::mutex> lk(mutex);
        if (!set)
            broken = true;
        cv.notify_all();
    }

    State() = default;
    State(State const&) = delete;
    State(State &&) = delete;
    State& operator=(State const&) = delete;
    State& operator=(State &&) = delete;

private:
    std::mutex mutable mutex;
    std::condition_variable mutable cv;
    bool set{false};
    bool broken{false};
    T value;
};

template<typename T>
struct NoTLSFuture
{
    NoTLSFuture() :
        state(nullptr) 
    {
    }

    NoTLSFuture(std::shared_ptr<State<T>> const& state) :
        state(state)
    {
    }

    NoTLSFuture(NoTLSFuture&& other) :
        state(std::move(other.state))
    {
    }

    NoTLSFuture& operator=(NoTLSFuture&& other)
    {
        state = std::move(other.state);
        return *this;
    }

    NoTLSFuture(NoTLSFuture const&) = delete;
    NoTLSFuture& operator=(NoTLSFuture const&) = delete;

    T get()
    {
        return state->get_value();
    }

    template<class Rep, class Period>
    std::future_status wait_for(std::chrono::duration<Rep, Period> const& timeout_duration) const
    {
        return state->wait_for(timeout_duration);
    }

    bool valid() const
    {
        return state != nullptr;
    }

private:
    std::shared_ptr<State<T>> state;
};

template<typename T>
class NoTLSPromise
{
public:
    NoTLSPromise():
        state(std::make_shared<State<T>>())
    {
    }

    ~NoTLSPromise()
    {
        if (state && !state.unique())
            state->break_promise();
    }

    NoTLSPromise(NoTLSPromise&& other) :
        state(std::move(other.state))
    {
    }

    NoTLSPromise& operator=(NoTLSPromise&& other)
    {
        state = std::move(other.state);
    }

    NoTLSPromise(NoTLSPromise const&) = delete;
    NoTLSPromise operator=(NoTLSPromise const&) = delete;

    void set_value(T value)
    {
        state->set_value(value);
    }

    NoTLSFuture<T> get_future()
    {
        return NoTLSFuture<T>(state);
    }

private:
    std::shared_ptr<State<T>> state; 
};
}
}
#endif
