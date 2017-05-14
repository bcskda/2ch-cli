# 2ch-cli
Консольный (ncurses) клиент для 2ch.hk

##### Юзабелен в данной версии, пытайся.

## Требования
- libcurl

- jsoncpp

- libncursesw

- [libcaca и caca-utils](https://github.com/cacalabs/libcaca) с поддержкой ncurses

## Установка
```
cd /path/to/repo
git clone https://github.com/bcskda/2ch-cli
cd 2ch-cli
make
```

## Запуск
```
./2ch-cli -h
```

## Настройки
Редактор берётся из переменной окружения ```EDITOR```.
Юзерагент, стандартный редактор и email расположены в первых строках makefile,
имеют префикс ```CONFIG_```. Можно аккуратно поменять.
По умолчанию:
- Редактор - nano
- Юзерагент - "Linux" (Linux: Неизвестно)
- email - пустой
