import time
import signal

from types import FrameType

from pproto_py import TcpServer
from pproto_py import Connection

from logger import logger
from connection import userCommands, userCommandsImpl


class TDemo04Server:
    def __init__(self):
        self.interrupted = False
        signal.signal(signal.SIGTERM, self.signal_handler)
        signal.signal(signal.SIGINT, self.signal_handler)
        self.pproto_server = TcpServer(
            '0.0.0.0',
            33023,
            userCommands,
            userCommandsImpl
        )
        logger.info("Start 'Transport Demo 04 Python Server'")

    def new_connection_handler(self, connection: Connection):
        logger.info("Connected %s:%s", *connection.hist_peername)

    def disconect_connection_handler(self, connection: Connection):
        logger.info("Disconnected")

    def start_connection(self):
        self.pproto_server.run(
            self.new_connection_handler,
            self.disconect_connection_handler
        )

    def run(self) -> None:
        self.start_connection()
        try:
            while not self.interrupted:
                time.sleep(3)
        finally:
            self.pproto_server.stop()

    def signal_handler(self, signum: int, frame: FrameType) -> None:
        if signum == signal.SIGINT or signum == signal.SIGTERM:
            logger.info(
                "Signal %s is received. Program will be stopped", "SIGTERM" if signum == signal.SIGTERM else "SIGINT"
            )
        else:
            logger.info("Signal handler called with signal %s at %s", signum, frame)
        self.interrupted = True

    def __del__(self):
        logger.info("Stop 'Transport Demo 04 Python Server'")


if __name__ == '__main__':
    TDemo04Server().run()
