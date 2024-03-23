import subprocess
import sys
from termcolor import colored

# Define a global list to hold all test case functions
test_cases = []


# Modify the decorator to add each test case function to the global list
def testcase(func):
    def wrapper(tester, *args, **kwargs):
        print(colored(f"Starting test '{func.__name__}'", "yellow"))
        try:
            func(tester, *args, **kwargs)
            print(colored(f"Test '{func.__name__}': PASSED", "green"))
        except AssertionError as e:
            print(colored(f"Test '{func.__name__}': FAILED - {e}", "red"))
        except Exception as e:
            print(colored(f"Test '{func.__name__}': ERROR - {e}", "red"))
        print(colored(f"Test '{func.__name__}' finished\n", "yellow"))

    test_cases.append(wrapper)  # Register the test case

    return wrapper


class ExecutableTester:
    def __init__(self, executable_path):
        self.executable_path = executable_path

    def execute(self, input_data):
        process = subprocess.Popen(
            [self.executable_path] + ["-t", "udp", "-s", "localhost", "-p", "4567"],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
        )
        stdout, stderr = process.communicate(input=input_data)
        return stdout, stderr


### TEST CASES ###


# Your test cases decorated with @testcase
@testcase
def test_hello(tester):
    stdout, stderr = tester.execute("Hello")
    for line in stdout.split("\n"):
        print(colored("STDOUT:", "blue"), colored(line, "blue"))
    for line in stderr.split("\n"):
        print(colored("STDERR:", "magenta"), colored(line, "magenta"))
    assert "expected_output" in stdout, "Output does not match expected output."


@testcase
def test_hello2(tester):
    stdout, stderr = tester.execute("Hello2")
    for line in stdout.split("\n"):
        print(colored("STDOUT:", "blue"), colored(line, "blue"))
    for line in stderr.split("\n"):
        print(colored("STDERR:", "magenta"), colored(line, "magenta"))
    assert "expected_output" in stdout, "Output does not match expected output."


### END TEST CASES ###


def run_tests(executable_path):
    tester = ExecutableTester(executable_path)
    # Iterate over all registered test cases and run them
    for test in test_cases:
        test(tester)


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python test_executable.py <path_to_executable>")
        sys.exit(1)
    executable_path = sys.argv[1]
    run_tests(executable_path)
