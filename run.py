import argparse
import logging
import os


logging.basicConfig(level = logging.INFO,format = '%(asctime)s - %(name)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)


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
    parser.add_argument('-t', '--test-case', default=None, type=int)
    opt = parser.parse_args()

    os.system('make')
    os.system('clear')

    if opt.test_case is None:
        for i in range(1, 13):
            num = f'0{i}' if i < 10 else i
            test(f'test/test-phase-1/test_1_r{num}.spl',
                f'test/test-phase-1/test_1_r{num}.out',
                f'test/out/test_1_r{num}.out')
    else:
        num = f'0{opt.test_case}' if opt.test_case < 10 else opt.test_case
        test(f'test/test-phase-1/test_1_r{num}.spl',
                f'test/test-phase-1/test_1_r{num}.out',
                f'test/out/test_1_r{num}.out')

    os.system('make clean')
