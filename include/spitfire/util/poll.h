#pragma once

#include <poll.h>

namespace spitfire
{
  namespace util
  {
    enum class POLL_READ_RESULT {
      ERROR,
      DATA_READY,
      TIMED_OUT
    };

    class poll_read {
    public:
      explicit poll_read(int fd);

      POLL_READ_RESULT poll(int timeout_ms);

    private:
      struct pollfd fds;
    };
  }
}
