import sys
import pathlib
import subprocess as sp

CWD = pathlib.Path().cwd()
PROJECT_ROOT = CWD.parent
INPUT_FILE = str(CWD) + '/input.txt'
FTEXT_EXEC = str(PROJECT_ROOT) + '/ftext'
EXIT_SUCCESS = 0
EXIT_FAILURE = 1

input_text = ''
with open(INPUT_FILE, mode='r') as f:
    input_text = f.read()


def main() -> int:
    sp.run(["make", "-C", PROJECT_ROOT], check=True)

    for w in range(1, 101):
        for l in range(1, 101):
            for g in range(0, 101):
                for c in range(1, 101):
                    cp = sp.run(
                        [
                            FTEXT_EXEC,
                            "--columns", str(c),
                            "--gap", str(g),
                            "--lines", str(l),
                            "--width", str(w)
                        ],
                        stdout=sp.PIPE,  # Remove this comment and you'll see.
                        stderr=sp.PIPE,
                        input=INPUT_FILE.encode()
                    )
                    if cp.returncode:
                        print("--- TEST FAILED: ")
                        print(f":: columns {c} ")
                        print(f":: gap     {g} ")
                        print(f":: lines   {l} ")
                        print(f":: width   {w} ")
                        print("--- REASON:")
                        print(cp.stderr.decode())
                        return EXIT_FAILURE;

    return EXIT_SUCCESS


if __name__ == "__main__":
    sys.exit(main())
