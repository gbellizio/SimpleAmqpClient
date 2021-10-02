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

#include <SimpleAmqpClient/SimpleAmqpClient.h>

#include <iostream>

#include "utils.h"

int main(int argc, char **argv) {
  using namespace AmqpClient;

  if (argc < 6) {
    std::cerr << "Usage: amqp_listenq host port user password queue_name\n";
    return 1;
  }

  auto open_opts = Channel::OpenOpts{};
  open_opts.host = argv[1];
  open_opts.port = std::stoi(argv[2]);
  open_opts.auth = Channel::OpenOpts::BasicAuth(argv[3], argv[4]);

  Channel::ptr_t channel = Channel::Open(open_opts);

  auto consumer_tag = channel->BasicConsume(argv[5]);

  while (true) {
    try {
      if (auto envelope = channel->BasicConsumeMessage(consumer_tag);
          envelope) {
        utils::print_envelop(envelope, std::cout);
      }
    } catch (const AmqpException &ex) {
      std::cerr << ex;
      return EXIT_FAILURE;
    } catch (const std::exception &ex) {
      std::cerr << ex.what() << std::endl;
    }
  }

  return EXIT_SUCCESS;
}