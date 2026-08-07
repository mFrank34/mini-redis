/*
 * File: Broadcast
 * Author: Michael Franks 
 * Description:
 */

#include "Broadcast.h"

#include <unistd.h>
#include <vector>


std::shared_ptr<std::mutex> Broadcast::write_mutex(int fd)
{
    auto it = write_mutexes_.find(fd);
    if (it == write_mutexes_.end())
    {
        auto m = std::make_shared<std::mutex>();
        write_mutexes_[fd] = m;
        return m;
    }
    return it->second;
}

void Broadcast::subscribe(const std::string& channel, int fd)
{
    std::lock_guard<std::mutex> lock(mutex_);
    channels_[channel].insert(fd);
    write_mutex(fd);
}

void Broadcast::unsubscribe(int fd)
{
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& [channel, fds] : channels_)
    {
        fds.erase(fd);
    }
    write_mutexes_.erase(fd);
}

int Broadcast::publish(const std::string& channel, const std::string& message)
{
    std::vector<int> targets;
    std::vector<std::shared_ptr<std::mutex>> locks;

    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = channels_.find(channel);
        if (it == channels_.end()) return 0;
        for (int fd : it->second)
        {
            targets.push_back(fd);
            locks.push_back(write_mutex(fd));
        }
    }

    std::string payload = "MESSAGE " + channel + " " + message + "\n";
    int delivered = 0;
    for (size_t i = 0; i < targets.size(); i++)
    {
        std::lock_guard<std::mutex> wlock(*locks[i]);
        ssize_t n = write(targets[i], payload.c_str(), payload.size());
        if (n > 0) delivered++;
        // if write fails, the client is probably dead — cleanup happens
        // naturally when that client's handle_client thread notices and
        // calls unsubscribe_all
    }
    return delivered;
}
