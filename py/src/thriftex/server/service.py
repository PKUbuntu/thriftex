import sys
import logging

from thrift.transport import TSocket
from thrift.protocol import TBinaryProtocol
from thrift.server.TNonblockingServer import TNonblockingServer
import threading


class ServiceEx(object):
    """ Xoa core service
    """
    def __init__(self, port=9090):
        """ Initialize port
        """
        self._port = port
        self._processor = None
        self._server_transport = None
        self._transport_factory = None
        self._protocol_factory = None
        self._server = None
        self._thread_number = 16
        self._service_thread = None

        self.logger = logging.getLogger('thriftex')

    def get_port(self):
        return self._port

    def set_processor(self, processor):
        self._processor = processor

    def set_thread_number(self, number):
        self._thread_number = number

    def run(self):
        if not self._processor:
            self.logger.error("Do not initialize handler/processor !")
            sys.exit(-1)

        self._server_transport = TSocket.TServerSocket(port=self._port)
        self._protocol_factory = TBinaryProtocol.TBinaryProtocolFactory()

        self._server = TNonblockingServer(self._processor,
                                          self._server_transport,
                                          self._protocol_factory)

        self._server.setNumThreads(self._thread_number)

        self.logger.info("service begin to run on port [%d]" % (self._port, ))
        self._server.serve()

    def get_running_status(self):
        return self._service_thread.is_alive()

    def start(self):
        self._service_thread = threading.Thread(target=self.run)
        self._service_thread.start()

    def stop(self):
        if self._service_thread is not None:
            self.logger.info("stop service")
            self._server.stop()
            self._service_thread.join()
            self._server.close()
