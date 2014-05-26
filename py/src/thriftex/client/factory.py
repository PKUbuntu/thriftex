# client factory
from thriftex.base.common import get_class
from thrift.transport import TSocket, TTransport
from thrift.protocol import TBinaryProtocol

class ClientFactory(object):
    def __init__(self, service_name):
        self._client_class = get_class(service_name + ".Client")
        self._host = "localhost"
        self._port = 9090

    def set_host(self, host):
        self._host = host

    def set_port(self, port):
        self._port = port

    def get_client(self):
        # Make socket
        transport = TSocket.TSocket(self._host, self._port)
        # Buffering is critical. Raw sockets are very slow
        transport = TTransport.TFramedTransport(transport)
        # Wrap in a protocol
        protocol = TBinaryProtocol.TBinaryProtocol(transport)

        client = self._client_class(protocol)
        transport.open()

        return client

if __name__ == '__main__':
    pass
