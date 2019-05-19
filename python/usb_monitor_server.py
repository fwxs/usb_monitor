#!/usr/bin/env python3
import json
import logging
import queue
import socketserver
import sys
import threading

logging.basicConfig(format="[%(asctime)s] %(levelname)s: %(message)s",
                    datefmt="%m/%d/%Y %H:%M:%S",
                    level=logging.INFO)

queue_ = queue.Queue()

class UsbMonitorServer(socketserver.BaseRequestHandler):
    def handle_json_data(self, json_str):
        queue_.put(json.loads(json_str))

    def handle(self):
        data = str(self.request.recv(1024), "utf-8")
        cur_thread = threading.current_thread()

        try:
            logging.info("Thread {0} received {1} bytes from {2}".format(cur_thread.getName(),
                                                                         len(data),
                                                                         self.client_address))
            self.handle_json_data(data)

        except json.decoder.JSONDecodeError as json_err:
            logging.error("On thread {0}. {1:s} sent an invalid JSON packet: {2}".format(cur_thread.getName(),
                                                                                        self.client_address[0],
                                                                                        json_err.msg))

        except Exception as ex:
            logging.error("On thread {0}. {1}".format(cur_thread.getName(), ex))


class ThreadedTCPServer(socketserver.ThreadingMixIn, socketserver.TCPServer):
    pass


if __name__ == "__main__":
    host, port = "127.0.0.1", 8080

    with ThreadedTCPServer((host, port), UsbMonitorServer) as server:
        server_thread = threading.Thread(target=server.serve_forever, daemon=True)
        server_thread.start()

        print("[*] Server running in thread:", server_thread.getName())
        s = ""
        while(s != "q"):
            s = input("Enter q to stop: ")
        server.shutdown()
        