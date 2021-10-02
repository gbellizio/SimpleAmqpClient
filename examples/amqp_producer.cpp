/*
 * ***** BEGIN LICENSE BLOCK *****
 * Version: MIT
 *
 * Portions created by Gennaro Bellizio are Copyright (c) 2021
 * Gennaro Bellizio. All Rights Reserved.
 *
 * Portions created by Alan Antonuk are Copyright (c) 2012-2013
 * Alan Antonuk. All Rights Reserved.
 *
 * Portions created by VMware are Copyright (c) 2007-2012 VMware, Inc.
 * All Rights Reserved.
 *
 * Portions created by Tony Garnock-Jones are Copyright (c) 2009-2010
 * VMware, Inc. and Tony Garnock-Jones. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * ***** END LICENSE BLOCK *****
 */

#include <SimpleAmqpClient/Bytes.h>
#include <SimpleAmqpClient/SimpleAmqpClient.h>

#include <chrono>
#include <iostream>
#include <numeric>
#include <thread>

#include "utils.h"

inline auto constexpr summary_every = std::chrono::seconds{1};
inline auto constexpr body_size = std::size_t{256};

inline void send_batch(AmqpClient::Channel::ptr_t channel,
                       const std::string &exchange,
                       const std::string &routing_key, int rate_limit,
                       int message_count) {
  auto body = std::string{};
  body.resize(body_size);

  std::iota(std::begin(body), std::end(body), 0x00);
  auto message = AmqpClient::BasicMessage::Create(body);

  int i;
  int sent = 0;
  int previous_sent = 0;

  using namespace std::chrono;
  using clock = high_resolution_clock;
  using milliseconds = duration<double, std::milli>;
  using seconds = duration<double>;

  auto start_time = clock::now(), previous_report_time = start_time;
  auto next_summary_time = start_time + summary_every;

  for (i = 0; i < message_count; i++) {
    auto now = clock::now();

    try {
      channel->BasicPublish(exchange, routing_key, message);

      sent++;
      if (now > next_summary_time) {
        int countOverInterval = sent - previous_sent;

        double intervalRate =
            countOverInterval /
            duration_cast<seconds>(now - previous_report_time).count();

        printf("%d ms: Sent %d - %d since last report (%d Hz)\n",
               (int)duration_cast<milliseconds>(now - start_time).count(), sent,
               countOverInterval, (int)intervalRate);

        previous_sent = sent;
        previous_report_time = now;
        next_summary_time += summary_every;
      }

      while ((i / duration_cast<seconds>(now - start_time).count()) >
             rate_limit) {
        std::this_thread::sleep_for(std::chrono::microseconds{500});
        now = clock::now();
      }

    } catch (const AmqpClient::AmqpException &ex) {
      std::cerr << ex << std::endl;
      exit(ex.reply_code());
    }
  }

  {
    auto total_delta = duration_cast<milliseconds>(clock::now() - start_time);

    printf("PRODUCER - Message count: %d\n", message_count);
    printf("Total time, milliseconds: %d\n",
           static_cast<int>(total_delta.count()));
    printf("Overall messages-per-second: %g\n",
           (message_count / (duration_cast<seconds>(total_delta).count())));
  }
}

int main(int argc, char **argv) {
  using namespace AmqpClient;

  if (argc < 9) {
    std::cerr
        << "Usage: amqp_producer host port exchange routing_key rate_limit "
           "message_count\n";
    return 1;
  }

  auto open_opts = Channel::OpenOpts{};
  open_opts.host = argv[1];
  open_opts.port = std::stoi(argv[2]);
  open_opts.auth = Channel::OpenOpts::BasicAuth(argv[3], argv[4]);

  Channel::ptr_t channel = Channel::Open(open_opts);

  auto rate_limit = std::stoi(argv[7]);
  auto message_count = std::stoi(argv[8]);

  send_batch(channel, argv[5], argv[6], rate_limit, message_count);

  return EXIT_SUCCESS;
}