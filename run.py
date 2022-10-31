import argparse
import logging
import os
import traceback


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


def test(test_case_path: str, output_path: str, answer_path: str):
    logger.info(f'Start to test case {test_case_path}')
    os.system(f'bin/splc {test_case_path}')
    answer, out = '', ''
    with open(answer_path) as f:
        answer = f.read()
    with open(output_path) as f:
        out = f.read()
    if answer == out:
        logger.info(f'Pass test case {test_case_path}')
    else:
        logger.warning(f'Fail to pass test case {test_case_path}')
        os.system(f'diff {output_path} {answer_path}')


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('-n', '--test-case-no', default=None, type=int)
    parser.add_argument('-t', '--type', default='r', type=str)
    parser.add_argument('--dir', default='test-official', type=str)
    opt = parser.parse_args()

    os.system('make')

    root, dir_names, file_names = list(os.walk(f'./{opt.dir}/'))[0]
    count = 0
    for file_name in file_names:
        if os.path.splitext(file_name)[-1][1:] == 'spl':
            count += 1

    try:
        if opt.test_case_no is None:
                for i in range(1, count + 1):
                    num = f'0{i}' if i < 10 else i
                    test(f'{opt.dir}/test_1_{opt.type}{num}.spl',
                        f'{opt.dir}/test_1_{opt.type}{num}.out',
                        f'{opt.dir}/standard-output/test_1_{opt.type}{num}.out')
        else:
            num = f'0{opt.test_case_no }' if opt.test_case_no    < 10 else opt.test_case_no
            test(f'{opt.dir}/test_1_{opt.type}{num}.spl',
                    f'{opt.dir}/test_1_{opt.type}{num}.out',
                    f'{opt.dir}/out/test_1_{opt.type}{num}.out')
    except Exception:
        traceback.print_exc()

    os.system('make clean')
