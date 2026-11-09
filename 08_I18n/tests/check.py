#!/usr/bin/env python3
"""Exercise all possible answers, invalid lines, EOF and installed locales."""
import os
import subprocess

program = os.environ.get("GUESS_PROGRAM", "./src/guess-number")
locales = [("C", "Yes", "No", "Your number is {}!", "Error:")]
if os.environ.get("CHECK_INSTALLED"):
    locales.append(("ru_RU.UTF-8", "Да", "Нет", "Ваше число — {}!", "Ошибка:"))

for locale, yes, no, result, error in locales:
    env = dict(os.environ, LC_ALL=locale, LANGUAGE="")

    def run(data):
        return subprocess.run([program], input=data, stdout=subprocess.PIPE,
                              stderr=subprocess.PIPE, env=env, timeout=5)

    for number in range(1, 101):
        low, high, answers = 1, 100, []
        while low < high:
            middle = (low + high) // 2
            answers.append(yes if number > middle else no)
            if number > middle:
                low = middle + 1
            else:
                high = middle
        assert len(answers) <= 7
        valid = ("\n".join(answers) + "\n").encode()
        completed = run(valid)
        assert completed.returncode == 0 and not completed.stderr
        assert completed.stdout.decode().rstrip().endswith(result.format(number))
        assert completed.stdout.decode().count("? (") == len(answers)
        # An invalid line must be consumed completely and leave the interval intact.
        for invalid in [b"\n", b"yes\n", b"Yes \n", b"x" * 10000 + b"\n", b"Yes\x00junk\n"]:
            completed = run(invalid + valid)
            assert completed.returncode == 0
            assert completed.stderr.decode().count(error) == 1
            assert completed.stdout.decode().rstrip().endswith(result.format(number))
    for data in [b"", (yes + "\n").encode(), b"wrong"]:
        completed = run(data)
        assert completed.returncode != 0 and error in completed.stderr.decode()
    # A final answer does not require a trailing newline.
    completed = run(((no + "\n") * 6 + no).encode())
    assert completed.returncode == 0
    assert completed.stdout.decode().rstrip().endswith(result.format(1))
print("All numbers 1..100, invalid input and EOF: OK")
