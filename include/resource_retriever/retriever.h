/*
 * Copyright (C) 2009, Willow Garage, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the names of Stanford University or Willow Garage, Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *     this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef RESOURCE_RETRIEVER_RETRIEVER_H
#define RESOURCE_RETRIEVER_RETRIEVER_H

#include <stdint.h>
#include <string>
#include <boost/shared_array.hpp>
#include <stdexcept>
#include <curl/curl.h>
#include <memory>

typedef void CURL;

namespace resource_retriever
{

struct Progress
{
    curl_off_t dl_total;
    curl_off_t dl_now;
    double last_in_secs;
    CURL *curl;
};

class Exception : public std::runtime_error
{
public:
    Exception(const std::string &file, const std::string &error_msg)
        : std::runtime_error("Error retrieving file [" + file + "]: " + error_msg)
    {}
};

/**
 * \brief A combination of a pointer to data in memory along with the data's size.
 */
struct MemoryResource
{
    MemoryResource()
        : size(0)
    {}

    boost::shared_array<uint8_t> data;
    uint32_t size;
};

/**
 * \brief Retrieves files from from a url.  Caches a CURL handle so multiple accesses to a single url
 * will keep connections open.
 */
class Retriever
{
private:
    static struct Progress prog_;
public:
    Retriever();
    ~Retriever();

    /**
     * \brief Get a file and store it in memory
     * \param url The url to retrieve.  package://package/file will be turned into the correct file:// invocation
     * \return The file, loaded into memory
     * \throws resource_retriever::Exception if anything goes wrong.
     */
    MemoryResource get(const std::string &url);

    static void setProgress(Progress &prog);
    static struct Progress getProgress();

private:
    Retriever(const Retriever &ret) = delete;

    static int xferinfo(const void *p,
                        const curl_off_t dltotal, const curl_off_t dlnow,
                        const curl_off_t ultotal, const curl_off_t ulnow);

    static int older_progress(void *p,
                              double dltotal, double dlnow,
                              double ultotal, double ulnow);
    CURL *curl_handle_;
};

} // namespace resource_retriever

#endif // RESOURCE_RETRIEVER_RETRIEVER_H
