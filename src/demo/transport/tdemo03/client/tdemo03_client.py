import time
import signal

from types import FrameType

from pproto_py import TcpSocket
from pproto_py import Connection

from logger import logger
from connection import userCommands, userCommandsImpl


class TDemo03Client:
    def __init__(self):
        self.interrupted = False
        signal.signal(signal.SIGTERM, self.signal_handler)
        signal.signal(signal.SIGINT, self.signal_handler)
        self.pproto_client = TcpSocket(
            '0.0.0.0',
            33022,
            userCommands,
            userCommandsImpl
        )
        self.conn = None
        logger.info("Start 'Transport Demo 03 Python Client'")

    def connect_handler(self, connection: Connection):
        logger.info("Connected to %s:%s", *connection.hist_peername)

    def restore_handler(self, connection: Connection):
        ...

    def disconnect_handler(self, connection: Connection):
        logger.info("Disconnected")

    def start_connection(self):
        self.pproto_client.start(
            connect_handler=self.connect_handler,
            restore_handler=self.restore_handler,
            disconnect_handler=self.disconnect_handler
        )
        self.conn: Connection = self.pproto_client.get_current_connection()
        while not self.conn or not self.conn.is_connected():
            time.sleep(3)
            self.conn = self.pproto_client.get_current_connection()
            if self.interrupted:
                break

    def run(self) -> None:
        self.start_connection()
        try:
            self.conn.TDemo03_01_sync()
            self.conn.TDemo03_02_sync()
        finally:
            self.pproto_client.disconnect()

    def signal_handler(self, signum: int, frame: FrameType) -> None:
        if signum == signal.SIGINT or signum == signal.SIGTERM:
            logger.info(
                "Signal %s is received. Program will be stopped", "SIGTERM" if signum == signal.SIGTERM else "SIGINT"
            )
        else:
            logger.info("Signal handler called with signal %s at %s", signum, frame)
        self.interrupted = True

    def __del__(self):
        logger.info("Stop 'Transport Demo 03 Python Client'")


if __name__ == '__main__':
    TDemo03Client().run()
