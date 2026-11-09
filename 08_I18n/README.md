# 08_I18n — угадывание числа и GNU gettext

Задание выполнено по [лекции](https://uneex.org/LecturesCMC/LinuxApplicationDevelopment2026/08_I18n)
и последней версии [simple-gettext](https://git.sr.ht/~frbrgeorge/simple-gettext/log).

Результат: `guess-number-1.0-alt1.src.rpm`.
Сборка и проверка выполнены в ALT Regular Sisyphus (aarch64),
в виртуальной машине из `qemu-alt/alt-aarch64.qcow2`.

Программа просит загадать число от 1 до 100, задаёт не более семи
вопросов «число больше середины?» и выводит результат.
В английской локали принимаются только `Yes` / `No`, в русской — `Да` / `Нет`.
Регистр и пробелы значимы. Неверная строка вызывает диагностику и повтор
того же вопроса; EOF и ошибка чтения завершают программу с ненулевым кодом.

## Autotools и перевод

Для ALT нужны `gcc autoconf automake gettext gettext-tools python3 rpm-build`.

```sh
autoreconf -fisv
./configure --prefix="$HOME/.local"
make
make check
make install
LC_ALL=ru_RU.UTF-8 "$HOME/.local/bin/guess-number"
```

Как в примере, используются `AM_GNU_GETTEXT([external])`, `po/LINGUAS`,
`po/POTFILES.in` и `po/Makevars`. `autoreconf` автоматически вызывает
`autopoint`. Путь к переводам берётся из `$(localedir)` при сборке;
для проверки перевода программу нужно установить в выбранный prefix.

`po/ru.po` — исходник перевода; `.pot`, `.gmo`, `.mo` и инфраструктура,
созданная autotools/gettext, генерируются и исключены из Git.
Обновить перевод после изменения строк:

```sh
make -C po update-po
# отредактировать po/ru.po
make
```

## Повторная сборка RPM в ALT

Из готового исходного пакета:

```sh
rpmbuild --rebuild guess-number-1.0-alt1.src.rpm
```

Из исходников этого каталога (выполнять из `08_I18n`):

```sh
mkdir -p "$HOME/RPM/SOURCES" "$HOME/RPM/SPECS"
tar -czf "$HOME/RPM/SOURCES/guess-number-1.0.tar.gz" \
    --transform='s,^,guess-number-1.0/,' \
    configure.ac Makefile.am src/guess-number.c src/Makefile.am \
    po/LINGUAS po/POTFILES.in po/Makevars po/ru.po \
    tests/check.py LICENSE guess-number.spec
cp guess-number.spec "$HOME/RPM/SPECS/"
rpmbuild -ba "$HOME/RPM/SPECS/guess-number.spec"
```

Spec запускает `autoreconf`, `configure`, сборку и `make check`.
`%find_lang` включает в бинарный пакет только скомпилированный каталог перевода.

## Проверка установленного пакета

В ALT выполнено:

```sh
sudo rpm -Uvh ~/RPM/RPMS/aarch64/guess-number-1.0-alt1.aarch64.rpm
CHECK_INSTALLED=1 GUESS_PROGRAM=/usr/bin/guess-number python3 tests/check.py
rpm -ql guess-number
rpm -V guess-number
```

Тесты прошли для всех чисел 1…100 в английской и русской локалях,
для пустого и неверного ответа, лишних пробелов, длинной строки,
встроенного NUL, EOF сразу и посреди диалога, последней строки без `\n`.
`rpm -V` не обнаружил изменений.

Пример после установки:

```text
Загадайте число от 1 до 100.
Ваше число больше 50? (Да/Нет): Да
Ваше число больше 75? (Да/Нет): Да
Ваше число больше 88? (Да/Нет): Да
Ваше число больше 94? (Да/Нет): Да
Ваше число больше 97? (Да/Нет): Да
Ваше число больше 99? (Да/Нет): Да
Ваше число — 100!
```

Содержимое бинарного пакета (`rpm -ql guess-number`):

```text
/usr/bin/guess-number
/usr/share/doc/guess-number-1.0
/usr/share/doc/guess-number-1.0/LICENSE
/usr/share/locale/ru/LC_MESSAGES/guess-number.mo
```

Исходных `.po`, временных `.pot` и прочих файлов сборки в бинарном пакете нет.
Исходный RPM содержит архив исходников и spec (в нём `.po` необходим).

SHA-256 исходного RPM:

```text
df0ab3e2fbf74b5881d6d0245dbf9cc8ca6b0200ae9284fe330669d74d3acf3c
```
