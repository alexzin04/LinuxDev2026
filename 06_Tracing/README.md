# move / safemove

`move infile outfile` копирует содержимое infile в outfile и удаляет исходник
после успешных fsync и закрытия файлов. При ошибке исходник сохраняется,
цель по возможности удаляется. Ошибки имеют разные коды выхода.

Библиотека libprotect.so запрещает unlink файлов с PROTECT в имени.
Установленный сценарий `safemove infile outfile` запускает move с LD_PRELOAD.

Сборка и проверки (Linux, gcc, make, Python 3, strace):

```sh
make
make test
make clean
```

Тесты проверяют коды ошибок и сохранность файлов через strace error injection,
а также защиту LD_PRELOAD.

Сборка исходного RPM в hasher:

```sh
hsh --lazy-clean --no-sisyphus-check=packager,changelog,gpg ~/hasher safemove-1.0-alt1.src.rpm
```

Проверки метаданных сопровождающего и подписи отключены.
