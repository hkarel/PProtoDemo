from pproto_py import Message
from pproto_py import Connection
from pproto_py import BaseCommand

import uuid

from .userCommands import TDEMO03_01
from .userCommands import TDEMO03_02


class TDemo03_01(BaseCommand):
    COMMAND_UUID = TDEMO03_01.uuid

    @staticmethod
    def initial(connection: Connection, *args, **kwargs) -> Message:
        msg = connection.create_command(TDemo03_01)
        msg.set_content({
            "value1": 5,
            "value2": "Request"
        })
        msg.set_max_time_life(30)
        return msg


class TDemo03_02(BaseCommand):
    COMMAND_UUID = TDEMO03_02.uuid

    @staticmethod
    def initial(connection, *args, **kwargs) -> Message:
        msg = connection.create_command(TDemo03_02)
        msg.set_content({
            "value1": 10,
            "value2": str(uuid.uuid4())
        })
        msg.set_max_time_life(30)
        return msg
