# 2ch-cli
Консольный (ncurses) клиент для 2ch.hk

##### Юзабелен в данной версии, пытайся.

## Требования
- pkg-config

- cmake>=3.7

- libcurl

- jsoncpp

- libncursesw

- [libcaca и caca-utils](https://github.com/cacalabs/libcaca) с поддержкой ncurses

## Установка
```
git clone https://github.com/bcskda/2ch-cli
cd 2ch-cli
mkdir build && cd build
cmake .. # Опционально, аргументы
make -j4
sudo make install
```

### Возможные аргументы CMake
```
-DCURL_USERAGENT='Ваш юзерагент (Linux)'
-DDEFAULT_EDITOR='Ваш редактор (nano)'
-DDEFAULT_EMAIL='Ваш email (пусто)'
-DCMAKE_INSTALL_PREFIX='Ваш префикс (/usr/local)'
```


## Запуск
```
./2ch-cli -h
```

## Настройки
Редактор берётся из переменной окружения ```EDITOR```.
Юзерагент, стандартный редактор и email задаются с помощью аргументов CMake.
По умолчанию:
- Редактор - nano
- Юзерагент - "Linux" (Linux: Неизвестно)
- email - пустой
