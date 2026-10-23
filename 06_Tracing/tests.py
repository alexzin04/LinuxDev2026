#!/usr/bin/env python3
import os
from pathlib import Path
import re
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parent
MOVE = str(ROOT / 'move')
DATA = bytes(range(256)) * 600
SOURCE = Path('input')
TARGET = Path('output')
NAMES = {
    2: 'открытие исходника', 3: 'fstat исходника',
    5: 'открытие цели', 6: 'fstat цели', 9: 'усечение цели',
    10: 'чтение', 11: 'запись', 13: 'fsync',
    14: 'закрытие цели', 15: 'закрытие исходника', 16: 'удаление исходника',
}
checks = 0


def reset():
    for path in (SOURCE, TARGET):
        if path.exists() or path.is_symlink():
            path.unlink()
    SOURCE.write_bytes(DATA)
    TARGET.write_bytes(b'old content')


def run(expected, args=None, injections=(), env=None):
    command = [MOVE, *(args if args is not None else ['input', 'output'])]
    if injections:
        options = [item for rule in injections for item in ('-e', 'inject=' + rule)]
        command = ['strace', '-qq', '-o', 'injected.trace', *options, *command]
    result = subprocess.run(command, env=env, capture_output=True)
    assert result.returncode == expected, (
        command, result.returncode, expected, result.stderr.decode(errors='replace'))
    if expected:
        assert result.stderr, 'Нет сообщения об ошибке'


def passed(name):
    global checks
    checks += 1
    print(f'{name}: OK', flush=True)


def discover_calls():
    reset()
    subprocess.run(['strace', '-qq', '-yy', '-o', 'baseline.trace',
                    MOVE, 'input', 'output'], check=True)
    assert not SOURCE.exists() and TARGET.read_bytes() == DATA
    passed('Перезапись существующего файла')
    ordinals = {}
    file_fds = {}
    calls = []
    for line in Path('baseline.trace').read_text().splitlines():
        match = re.match(r'(\w+)\(', line)
        if not match:
            continue
        call = match[1]
        ordinals[call] = ordinals.get(call, 0) + 1
        source = 'input' in line
        target = 'output' in line
        if call in ('open', 'openat') and (source or target):
            result = re.search(r'= (\d+)', line)
            if result:
                file_fds[int(result[1])] = 'source' if source else 'target'
        if call in ('fstat', 'newfstatat', 'ftruncate', 'read', 'write', 'fsync', 'close'):
            fd = re.match(r'\w+\((\d+)', line)
            role = file_fds.get(int(fd[1])) if fd else None
            source = role == 'source'
            target = role == 'target'
        if not (source or target):
            continue
        if call in ('open', 'openat'):
            code = 2 if source else 5
        elif call in ('fstat', 'newfstatat'):
            code = 3 if source else 6
        elif call == 'close':
            code = 15 if source else 14
        else:
            code = {'ftruncate': 9, 'read': 10, 'write': 11,
                    'fsync': 13, 'unlink': 16, 'unlinkat': 16}.get(call)
        if code:
            calls.append((call, ordinals[call], code))
    assert set(NAMES) <= {code for _, _, code in calls}, calls
    return calls


def test_injections(calls):
    for call, ordinal, code in calls:
        reset()
        run(code, injections=[f'{call}:error=EIO:when={ordinal}'])
        assert SOURCE.read_bytes() == DATA
        assert TARGET.exists() == (code in (2, 3, 5, 6))
        passed(f'Ошибка: {NAMES[code]} ({call}, вызов {ordinal})')
    write_number = next(number for _, number, code in calls if code == 11)
    reset()
    run(12, injections=[f'write:retval=0:when={write_number}'])
    assert SOURCE.read_bytes() == DATA and not TARGET.exists()
    passed('Нулевая запись')


def test_cleanup(calls):
    unlink_call = next(call for call, _, code in calls if code == 16)
    reset()
    run(19, injections=['fsync:error=EIO:when=1', f'{unlink_call}:error=EPERM:when=1'])
    assert SOURCE.read_bytes() == DATA and TARGET.exists()
    passed('Ошибка удаления цели при откате')
    close_number = next(number for _, number, code in calls if code == 14)
    for offset, code, name in ((0, 17, 'цели'), (1, 18, 'исходника')):
        reset()
        run(code, injections=['fsync:error=EIO:when=1',
                              f'close:error=EIO:when={close_number + offset}'])
        assert SOURCE.read_bytes() == DATA and not TARGET.exists()
        passed(f'Ошибка закрытия {name} при откате')


def test_arguments_and_types():
    reset()
    run(1, args=[])
    assert SOURCE.read_bytes() == DATA
    passed('Неверное число аргументов')
    run(7, args=['input', 'input'])
    assert SOURCE.read_bytes() == DATA
    passed('Одинаковые пути')
    os.link(SOURCE, 'alias')
    run(7, args=['input', 'alias'])
    assert SOURCE.read_bytes() == DATA and Path('alias').read_bytes() == DATA
    Path('alias').unlink()
    passed('Один inode у исходника и цели')
    for code, args, name in (
        (4, ['.', 'output'], 'Каталог вместо исходника'),
        (5, ['input', '.'], 'Каталог вместо цели'),
        (8, ['input', '/dev/null'], 'Устройство вместо цели'),
    ):
        run(code, args=args)
        assert SOURCE.read_bytes() == DATA and TARGET.read_bytes() == b'old content'
        passed(name)
    os.symlink('input', 'symlink')
    run(5, args=['input', 'symlink'])
    assert SOURCE.read_bytes() == DATA and Path('symlink').is_symlink()
    passed('Символическая ссылка вместо цели')
    os.mkfifo('fifo')
    run(4, args=['fifo', 'output'])
    assert Path('fifo').exists() and TARGET.read_bytes() == b'old content'
    passed('FIFO вместо исходника')


def test_preload():
    reset()
    protected = Path('inputPROTECT')
    SOURCE.rename(protected)
    env = dict(os.environ, LD_PRELOAD=str(ROOT / 'libprotect.so'))
    run(16, args=[str(protected), 'output'], env=env)
    assert protected.read_bytes() == DATA and not TARGET.exists()
    passed('LD_PRELOAD: защита PROTECT')
    reset()
    run(0, env=env)
    assert not SOURCE.exists() and TARGET.read_bytes() == DATA
    passed('LD_PRELOAD: обычное имя')
    Path('PROTECT-dir').mkdir()
    nested = Path('PROTECT-dir/plain')
    nested.write_bytes(DATA)
    run(0, args=[str(nested), 'output'], env=env)
    assert not nested.exists() and TARGET.read_bytes() == DATA
    passed('LD_PRELOAD: PROTECT только в имени каталога')


def test_success():
    reset()
    TARGET.unlink()
    run(0)
    assert not SOURCE.exists() and TARGET.read_bytes() == DATA
    passed('Перемещение в новый файл')
    SOURCE.write_bytes(b'')
    run(0)
    assert not SOURCE.exists() and TARGET.read_bytes() == b''
    passed('Перемещение пустого файла')


def main():
    with tempfile.TemporaryDirectory(prefix='move-test-') as directory:
        previous = Path.cwd()
        try:
            os.chdir(directory)
            calls = discover_calls()
            test_injections(calls)
            test_cleanup(calls)
            test_arguments_and_types()
            test_preload()
            test_success()
        finally:
            os.chdir(previous)
    print(f'Всего проверок: {checks}; вбросов по базовой трассе: {len(calls)}')


if __name__ == '__main__':
    main()
