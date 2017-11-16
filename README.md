# 2ch-cli
Консольный (ncurses) клиент для 2ch.hk

## Что умеет
- Читать треды
- Работать с двачекапчей
- Постить с капчей (для редактирования поста форкается в `$EDITOR`)
- Искать по треду

## Что не умеет
- Показывать картинки
- Прикреплять файлы
- Другие капчи
- Пасскоды

## Скриншоты
![](https://a.safe.moe/8RRWy.png)
![](https://a.safe.moe/KyyIo.png)
![](https://a.safe.moe/GdTxU.png)

## Требования
- pkg-config

- cmake>=3.5

- libcurl

- jsoncpp

- libncursesw

- icu

- libcaca с поддержкой ncurses

- caca-utils (`convert`)

## Сборка
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

## Использование
`2ch-cli -h` - помощь

`2ch-cli -b abu -n 42375` - откроет API-тред

## Клавиши
`h` - помощь по клавишам

`C` - очистить нижнюю часть экрана

`q` - выход

Стрелки - скролл вверх/вниз

`PageUp` / `PageDown` - вверх/вниз на 20 строк

`Home` / `End` - к первому/последнему посту

`g` - переход по номеру на борде

`G` - переход по номеру в треде

`u` - обновить тред

`f` - поиск по подстроке в постах

`s` - вкл/выкл sage

`Enter` - отправить пост

## Настройки
Редактор берётся из `$EDITOR`.
Юзерагент, стандартный редактор и email задаются с помощью аргументов CMake.
По умолчанию:
- Редактор - nano
- Юзерагент - "Linux" (Linux: Неизвестно)
- email - пустой
