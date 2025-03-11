#!/bin/bash

# Проверяем, переданы ли аргументы (1 — путь к бинарнику, 2 — путь к тестам)
if [ $# -lt 2 ]; then
    echo "❌ Использование: $0 <путь_к_бинарнику> <путь_к_папке_с_тестами>"
    exit 1
fi

binary="$1"        # Путь к бинарнику
data_dir="$2"      # Путь к папке с тестами

if [ ! -d "$data_dir" ]; then
    echo "❌ Папка $data_dir не найдена!"
    exit 1
fi

files=("$data_dir"/*.dat)

if [ ${#files[@]} -eq 0 ] || [ "${files[0]}" == "$data_dir/*.dat" ]; then
    echo "❌ Нет файлов с расширением .dat в папке $data_dir."
    exit 1
fi

for file in "${files[@]}"; do
    base_name=$(basename "$file" .dat)        # Получаем имя файла без расширения
    expected_file="$data_dir/$base_name.ans"  # Полный путь к файлу .ans

    echo "📂 Обработка файла: $file"

    if [ ! -f "$expected_file" ]; then
        echo "❌ Файл с ожидаемым результатом ($expected_file) не найден!"
        exit 1
    fi

    result=$("$binary" < "$file")     # Запускаем программу и сохраняем результат

    read size < "$expected_file"
    expected_arr=($(awk 'NR>1 {print $0}' "$expected_file"))

    # Преобразуем вывод программы в массив
    result_arr=($result)

    # Сравнение размеров массивов
    if [ ${#expected_arr[@]} -ne $size ] || [ ${#result_arr[@]} -ne $size ]; then
        echo "❌ Размеры массивов не совпадают! Размер в expected: $size, в выводе: ${#result_arr[@]}"
        exit 1
    fi

    # Сравнение элементов массивов
    for ((i = 0; i < $size; i++)); do
        if [ "${expected_arr[$i]}" -ne "${result_arr[$i]}" ]; then
            echo "❌ Ошибка на элементе $i! Ожидалось: ${expected_arr[$i]}, получено: ${result_arr[$i]}"
            exit 1
        fi
    done

    # Если все элементы совпали
    echo "✅ Тест пройден!"
    echo "-----------------------------------"

done
