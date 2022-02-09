from pproto_py import Message
from pproto_py import Connection
from pproto_py import BaseCommand

import logging

from .userCommands import TDEMO04_01
from .userCommands import TDEMO04_02


class TDemo04_01(BaseCommand):
    COMMAND_UUID = TDEMO04_01.uuid

    @staticmethod
    def initial(connection: Connection, *args, **kwargs) -> None:
        ...

    @staticmethod
    def handler(msg: Message) -> None:
        ans = msg.get_answer_copy()
        ans.set_content({
            "value1": 10,
            "value2": "Response"
        })
        ans.send_answer()


class TDemo04_02(BaseCommand):
    COMMAND_UUID = TDEMO04_02.uuid

    @staticmethod
    def initial(connection: Connection, *args, **kwargs) -> None:
        ...

    @staticmethod
    def handler(msg: Message) -> None:
        ans = msg.get_answer_copy()
        content = msg.get_content()
        if content["beginTest"]:
            logging.getLogger("pproto_py").disabled = True
        if content["endTest"]:
            logging.getLogger("pproto_py").disabled = False
        ans.set_content({})
        ans.send_answer()
