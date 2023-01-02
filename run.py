import argparse
import logging
import os


class CustomFormatter(logging.Formatter):

    grey = "\x1b[38;20m"
    yellow = "\x1b[33;20m"
    red = "\x1b[31;20m"
    bold_red = "\x1b[31;1m"
    reset = "\x1b[0m"
    green = "\x1b[32;20m"
    format = "%(asctime)s - %(name)s - %(levelname)s - %(message)s (%(filename)s:%(lineno)d)"

    FORMATS = {
        logging.DEBUG: grey + format + reset,
        logging.INFO: green + format + reset,
        logging.WARNING: yellow + format + reset,
        logging.ERROR: red + format + reset,
        logging.CRITICAL: bold_red + format + reset
    }

    def format(self, record):
        log_fmt = self.FORMATS.get(record.levelno)
        formatter = logging.Formatter(log_fmt)
        return formatter.format(record) 


# create logger with 'spam_application'
logger = logging.getLogger(__name__)
logger.setLevel(logging.DEBUG)

# create console handler with a higher log level
ch = logging.StreamHandler()
ch.setLevel(logging.DEBUG)

ch.setFormatter(CustomFormatter())

logger.addHandler(ch)
# logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
# logger = logging.getLogger(__name__)

map = {
    (1, ''): '175\n36\n103',
    (2, '12,2011'): '31',
    (3, ''): '2\n3\n5\n7\n11\n13\n17\n19\n23\n29',
    (4, '6'): '6\n36\n216',
    (5, '7'): '13',
    (6, ''): '1000003\n1000002\n3000002\n1000003\n2000001\n2000003\n1000003',
    (7, '15,9'): '3\n3',
    (8, '16'): '7',
    (9, ''): '370\n371\n407\n3',
    (10, '-114'): '-1'
}


if __name__ == "__main__":
    parser = argparse.ArgumentParser()

    os.system('make -j 8 && bash run.sh')

    for case, answer in map.items():
        id, input = case
        logger.info(f'test_3_r{id:02}.ir. Input: {input}')
        print('------Answer-----')
        print(answer)
        print('-----Output------')
        if input != '':
            os.system(f'python3 ./irsim/irsim.py --non-interactive test/test_3_r{id:02}.ir -O /dev/stdout -i {input}')
        else:
            os.system(f'python3 ./irsim/irsim.py --non-interactive test/test_3_r{id:02}.ir -O /dev/stdout')
