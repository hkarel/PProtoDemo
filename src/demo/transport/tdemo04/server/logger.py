import sys
import logging
import tempfile


logger = logging.getLogger("tdemo04")
logger.setLevel(logging.DEBUG)


fd, fn = tempfile.mkstemp()


logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s %(levelname)s %(name)s [%(filename)s:%(lineno)d] %(message)s",
    datefmt="%d.%m.%Y %H:%M:%S",
    handlers=(
        logging.FileHandler(fn, mode="a"),
        logging.StreamHandler(sys.stdout),
    )
)

# Установка значения уровня логирования для proto_py
logging.getLogger("pproto_py").setLevel(logging.DEBUG)


# Установка значения уровня логирования для connection
logging.getLogger("connection").setLevel(logging.DEBUG)
