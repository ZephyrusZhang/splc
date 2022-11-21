import argparse
import logging
import os

from os.path import isfile, join, splitext, basename


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
    logger.info(f'Start to test case {test_case_path}, write output to {output_path}')
    cmd = f'bin/splc {test_case_path} 2>{output_path[:-4]} && mv {output_path[:-4]} {output_path}'
    logger.info(f'cmd: {cmd}')
    os.system(cmd)
    answer, out = '', ''
    with open(answer_path) as f:
        answer = f.read()
    with open(output_path) as f:
        out = f.read()
    
    print(f'Content of {answer_path}')
    print(CustomFormatter.yellow + answer + CustomFormatter.reset)
    print(f'Content of {output_path}')
    print(CustomFormatter.yellow + out + CustomFormatter.reset)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('-f', '--file-dir', type=str, required=True, help='spl file directory')
    opt = parser.parse_args()

    os.system('make -j 8')

    files_path = [join(opt.file_dir, filr_or_dir) for filr_or_dir in os.listdir(opt.file_dir) if isfile(join(opt.file_dir, filr_or_dir))]
    standard_output_dir = join(opt.file_dir, 'standard-out')

    for file_path in files_path:
        file_name, ext = splitext(basename(file_path))
        if ext != '.spl':
            continue
        print(file_name)
        test(test_case_path=file_path,
             output_path=join(opt.file_dir, f'{file_name}.out'),
             answer_path=join(standard_output_dir, f'{file_name}.out'))
