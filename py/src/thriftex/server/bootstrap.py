import logging
import signal
import time

from thriftex.server.service import ServiceEx
from thriftex.base.common import get_class


class BootStrap(object):
    def __init__(self):
        self._processor = None
        self._handler = None
        self._service_ex = None
        self._port = 9090

        self.logger = logging.getLogger('thriftex')

    def set_port(self, port):
        self._port = port

    def set_handler_name(self, handler_name):
        """ Set the handler class name, inherited from service_class.Iface
        """
        handler_class = get_class(handler_name)
        self.set_handler(handler_class())
        self.logger.info("Create handler: %s" % (handler_name,))

    def set_handler(self, handler):
        self._handler = handler

    def set_service_name(self, service_class):
        """ Set the service class name, according to thrift generator rule,
        processor name should be `service_class.Processor'
        """
        processor_class = get_class(service_class + ".Processor")
        self._processor = processor_class(self._handler)
        self.logger.info("Create processor: %s.Processor" % (service_class,))

    def start(self):
        self._service_ex = ServiceEx(port=self._port)
        self._service_ex.set_processor(self._processor)
        self._service_ex.start()

        # the signal handler
        signal.signal(signal.SIGINT, self.stop)

    def stop(self, signum=None, frame=None):
        self._service_ex.stop()

    def wait_for_stop(self):
        while self._service_ex.get_running_status():
            time.sleep(1)
