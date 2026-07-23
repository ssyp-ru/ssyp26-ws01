# Мастерская №1 "Нани"

Мастерская по разработке компилятора языка. 

Пример кода на "нани".
```
fn fib(n) {
    let cur = 1;
    let prev = 1;
    for (let i = 3; i < n; i = i + 1) {
        let next = prev + cur;
        prev = cur;
        cur = next;
    }
    return cur;
}

print "fibonacci number 20:";
print fib(20);
```

Больше примеров в папке nani/examples и в [файле тестов](nani/core/tests/test.nani)

# Сборка и запуск

Сборка тестировалась на линуксе в debian based дистрибутиве.

Для сборки проекта нужны gcc и дополнительные библиотеки для сборки raylib, на момент работы мастерской это
```
sudo apt-get install -y build-essential libasound2-dev libgl1-mesa-dev libglu1-mesa-dev libx11-dev libxcursor-dev libxi-dev libxinerama-dev libxrandr-dev
```

Далее

```
cd nani
make
```

и можно запускать

```
./core/nani examples/snake.nani
```

# Состав мастерской
## Ученики
- Глазачев Семён Юрьевич
- Чемисов Глеб Евгеньевич
- Непраш Глеб Евгеньевич
- Калошин Никита Антонович
- Самышкин Константин Сергеевич
- Личман Вера Ильинична


## Руководитель
Климов Николай Сергеевич
