import os
import numpy as np
import matplotlib.pyplot as plt
import re
import sys
import subprocess
from time import time
from typing import Dict, List, Tuple

PROGRAM_PATH = "./convolution.out"
INPUT_PATH = "images/mypersonalphoto.bmp"
OUTPUT_IMAGE = "images/output.bmp"
NUM_RUNS = 100
THREADS = [1, 2, 4, 8]
BLOCK_SIZES = [16, 32, 64, 128]

FILTERS = ["blur", "soft_blur", "gaus_small_blur", "sharpen", "motion",
           "gaus_blur", "negative",  "random"]

def  run_benchmark(filter: str, mode: str, threads: int = 1, block_size: int = 32):
    timestamps=[]
    cmd = [
            PROGRAM_PATH,
            f"--filter={filter}",
            f"--mode={mode}",
            f"--threads={threads}",
            INPUT_PATH,
            OUTPUT_IMAGE
        ]
    str = ""
    for item in cmd:
        str += " " + item

    if mode == "by_grid":
        cmd.insert(3, f"--block={block_size}")
    return str


def main():
    some = run_benchmark("blur","seq",3,4)
    execution_times = []
    output = os.popen(some).read().strip()

    match = re.search(r"convo time (\d+\.\d+)", output)
    if match:
        time = float(match.group(1))
        execution_times.append(time)
    else:
        print(f"Failed to parse output: {output}")


if __name__ == "__main__":
    main()