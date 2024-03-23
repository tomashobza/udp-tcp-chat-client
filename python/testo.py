import subprocess
import sys
import threading
import queue
from termcolor import colored, cprint
from time import sleep

# Define a global list to hold all test case functions
test_cases = []


def testcase(func):
    def wrapper(tester, *args, **kwargs):
        passed = False

        print("==" * 10)
        print(colored(f"⏳ Starting test '{func.__name__}'", "yellow"))
        try:
            func(tester, *args, **kwargs)
            print(colored(f"✅ Test '{func.__name__}': PASSED", "green"))
            passed = True
        except AssertionError as e:
            print(colored(f"❌ Test '{func.__name__}': FAILED - {e}", "red"))
        except Exception as e:
            print(colored(f"❌ Test '{func.__name__}': ERROR - {e}", "red"))
        print(colored(f"Test '{func.__name__}' finished\n", "yellow"))
        tester.teardown()  # Clean up after test

        return passed

    test_cases.append(wrapper)  # Register the test case
    return wrapper


class ExecutableTester:
    def __init__(self, executable_path):
        self.executable_path = executable_path
        self.process = None
        self.stdout_queue = queue.Queue()
        self.stderr_queue = queue.Queue()
        self.return_code = None

    def setup(self, args=["-t", "udp", "-s", "localhost", "-p", "4567"]):
        if self.process:
            self.teardown()
        self.process = subprocess.Popen(
            [self.executable_path] + args,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            bufsize=1,  # Line buffered
        )
        self._start_thread(self.read_stdout, self.stdout_queue)
        self._start_thread(self.read_stderr, self.stderr_queue)

        sleep(0.2)  # Give some time for the process to start

    def _start_thread(self, target, queue):
        thread = threading.Thread(target=target, args=(queue,))
        thread.daemon = True  # Thread dies with the program
        thread.start()

    def read_stdout(self, queue):
        for line in iter(self.process.stdout.readline, ""):
            print(colored("STDOUT:", "blue"), colored(line, "blue"), end="")
            queue.put(line)

    def read_stderr(self, queue):
        for line in iter(self.process.stderr.readline, ""):
            print(colored("STDERR:", "magenta"), colored(line, "magenta"), end="")
            queue.put(line)

    def execute(self, input_data):
        self.process.stdin.write(input_data + "\n")
        self.process.stdin.flush()

        sleep(0.2)

    def get_stdout(self):
        output = []
        while not self.stdout_queue.empty():
            output.append(self.stdout_queue.get())
        return "".join(output)

    def get_stderr(self):
        output = []
        while not self.stderr_queue.empty():
            output.append(self.stderr_queue.get())
        return "".join(output)

    def teardown(self):
        self.process.terminate()
        self.process.wait()
        self.return_code = self.process.returncode
        self.process = None

    def get_return_code(self):
        return self.return_code


### TEST CASES ###


# PART 1 - Testing command-line aguments
cprint(f"Testing command-line arguments", "white", "on_black")


@testcase
def test_no_args(tester):
    tester.setup(args=[])
    assert tester.get_return_code() != 0, "Expected non-zero exit code."


# PART 2 - Testing basic commands
cprint(f"Testing basic commands", "white", "on_black")


@testcase
def test_hello(tester):
    tester.setup(args=["-t", "udp", "-s", "localhost", "-p", "4567"])
    tester.execute("Hello")
    stdout = tester.get_stdout()
    stderr = tester.get_stderr()
    assert "ERR:" in stderr, "Output does not match expected output."


### END TEST CASES ###


def run_tests(executable_path):
    test_cases_passed = 0
    tester = ExecutableTester(executable_path)
    for test in test_cases:
        test_cases_passed += 1 if test(tester) else 0

    cprint(
        f"\n{'✅' if test_cases_passed == len(test_cases) else '❌'} {test_cases_passed}/{len(test_cases)} test cases passed",
        "green" if test_cases_passed == len(test_cases) else "red",
    )


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python test_executable.py <path_to_executable>")
        sys.exit(1)
    executable_path = sys.argv[1]
    run_tests(executable_path)
