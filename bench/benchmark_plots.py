import os
import re
import numpy as np
import matplotlib.pyplot as plt

PROGRAM_PATH = "./convolution.out"
IMAGE_NAME = "5120x2880.bmp"
IMAGE_PATH = f"images/{IMAGE_NAME}"
IMAGE_OUTPUT_PATH = f"images/res.bmp"
NUM_RUNS = 25
THREAD_NUM = 4

ALL_MODES = ["seq", "by_row", "by_column", "by_grid", "by_pixel"]
PARALLEL_MODES = [mode for mode in ALL_MODES if mode != "seq"]
FILTERS = ["blur", "soft_blur", "gaus_small_blur", "sharpen", "motion",
                               "gaus_blur", "negative"]

FILTERS_DICT = {
    "soft_blur": "soft blur(3x3)",
    "blur": "Blur (5x5)",
    "gaus_blur": "Gaussian Blur (5x5)",
    "motion": "Motion Blur (9x9)",
    "sharpen": "Sharpen (3x3)",
    "negative": "Negative (3[3])",
    "gaus_small_blur":"Gaussian small blur (3х3)"
}

OUTPUT_DIR = f"results/{IMAGE_NAME.split('.')[0]}"
os.makedirs(OUTPUT_DIR, exist_ok=True)


def run_program(filter_name, mode):
    times = []
    cmd = f"{PROGRAM_PATH} --filter={filter_name} --mode={mode} {IMAGE_PATH} {IMAGE_OUTPUT_PATH}"
    if mode != "seq":
        cmd += f" --thread={THREAD_NUM}"
    for _ in range(NUM_RUNS):
        output = os.popen(cmd).read().strip()
        match = re.search(r"convo time (\d+\.\d+)", output)
        if match:
            times.append(float(match.group(1)))

    return np.array(times)


def mean_confidence_interval(data, confidence=0.95):
    n = len(data)
    m = np.mean(data)
    se = np.std(data, ddof=1) / np.sqrt(n)
    h = 1.96 * se 
    return m, h


def plot_individual(filter_name, means, errors):
    x = np.arange(len(PARALLEL_MODES))
    plt.bar(
        x, means, yerr=errors, capsize=10,
        color=["#efa94a", "#47a76a", "#db5856", "#9966cc"],
        alpha=0.75
    )
    plt.xticks(x, PARALLEL_MODES)
    plt.ylabel("Time (s)")
    plt.title(f"{FILTERS_DICT[filter_name]} Execution Times (Threads: {THREAD_NUM})")
    plt.tight_layout()
    plt.savefig(f"{OUTPUT_DIR}/hist_{filter_name}.png")
    plt.clf()


def plot_grouped(all_results):
    bar_width = 0.15
    x = np.arange(len(FILTERS))
    fig, ax = plt.subplots(figsize=(12, 6))

    for i, mode in enumerate(ALL_MODES):
        means = [all_results[f]["means"][i] for f in FILTERS]
        errors = [all_results[f]["errors"][i] for f in FILTERS]
        ax.bar(
            x + i * bar_width, means, bar_width,
            yerr=errors, capsize=5, label=mode, alpha=0.7
        )

    ax.set_xticks(x + bar_width * (len(ALL_MODES) - 1) / 2)
    ax.set_xticklabels([FILTERS_DICT[f] for f in FILTERS], rotation=45)
    ax.set_ylabel("Time (s)")
    ax.set_title(f"Filter Performance Comparison (Threads: {THREAD_NUM})")
    ax.legend(title="Mode")
    plt.tight_layout()
    plt.savefig(f"{OUTPUT_DIR}/grouped_chart.png")

BLOCK_SIZES =[16, 32, 64, 128]

def benchmark_block_sizes(filters=["blur", "motion"]):
    for filter_name in filters:
        results = {}
        for block_size in BLOCK_SIZES:
            cmd = f"{PROGRAM_PATH} --filter={filter_name} --mode=by_grid {IMAGE_PATH} --block={block_size} {IMAGE_OUTPUT_PATH} --thread={THREAD_NUM}"
            times = []
            for _ in range(NUM_RUNS):
                output = os.popen(f"{cmd}").read().strip()
                match = re.search(r"convo time (\d+\.\d+)", output)
                if match:
                    times.append(float(match.group(1)))
            mean, error = mean_confidence_interval(np.array(times))
            results[block_size] = (mean, error)
        
        means = [results[bs][0] for bs in BLOCK_SIZES]
        errors = [results[bs][1] for bs in BLOCK_SIZES]
        plt.bar([str(bs) for bs in BLOCK_SIZES], means, yerr=errors, capsize=10, color="lightgreen")
        plt.xlabel("Block Size")
        plt.ylabel("Time (s)")
        plt.title(f"{FILTERS_DICT[filter_name]} - by_grid Block Size Scaling")
        plt.tight_layout()
        plt.savefig(f"{OUTPUT_DIR}/scaling_blocksize_{filter_name}.png")
        plt.clf()

def plots_for_filters():
    all_results = {f: {"means": [], "errors": []} for f in FILTERS}
    with open(f"{OUTPUT_DIR}/benchmark.txt", "w") as f:
        for filter_name in FILTERS:
            f.write(f"{filter_name}\n")
            means = []
            errors = []
            for mode in ALL_MODES:
                print(f"{filter_name} - {mode}")
                times = run_program(filter_name, mode)
                mean, error = mean_confidence_interval(times)
                means.append(mean)
                errors.append(error)
                f.write(f"  {mode}: {mean:.4f} ± {error:.4f} s\n")
            all_results[filter_name]["means"] = means
            all_results[filter_name]["errors"] = errors
            plot_individual(filter_name, means[1:], errors[1:])
    plot_grouped(all_results)

def main():

    plots_for_filters()

    benchmark_block_sizes()

    print(f"Готово. Результаты и графики сохранены в '{OUTPUT_DIR}'.")


if __name__ == "__main__":
    main()
