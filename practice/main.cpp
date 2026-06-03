/***************************************************************************
*Project Type: Command-Line Tool                                      *
*Project Name: practice                                                    *
*File Name: main.cpp                                                       *
*Language: C++, Clion 2025.2.1                                   		   *                                       	   *
*Programmer: Баева Алёна Артёмовна                                         *
*Modified by:           									               *
*Created: 05.05.2026                                                       *
*Last Revision: 03.06.26                                    			   *
*Comment:                                                                  *
*Тема: «Структуры данных»                                                  *
*В процессе функционирования АСУ ВД  в  файле  фиксируются данные          *
*о самолетах,выполняющих полет в зоне действия АСУ.                        *
*  1) подготовить программу, сортирующую записи с использованием индексной *
*  сортировки методом выбора в порядке убывания  номеров рейсов - для      *
*  самолетов,  находящихся  в  воздухе  и  в  порядке возрастания времен   *
*  посадки  -  для  самолетов,  совершающих  посадку;  результаты печатать *
*  в виде таблицы;                                                         *
*  2) обеспечить входной контроль бортового номера, цифровой части         *
*  бортового номера и времени посадки, выполнить отладку и тестирование.   *
***************************************************************************/


#include <iostream>
#include <fstream>
using namespace std;

const int max_len = 100;
const char filename[] = "tests/test3.txt";

typedef struct {
    char brand[max_len]; // Марка ЛА
    char tail_number[max_len]; // Бортовой номер
    int flight_number; // Номер рейса
    unsigned int boarding_hour; // Час посадки
    unsigned int boarding_min; // Время посадки
    char original_line[max_len]; // Исходная строка из файла
    bool is_valid; // Флаг корректности всей строки
} Aircraft;

// Коды ошибок
enum ErrCode {
    OK = 0, ERR_FILE_NOT_FOUND, ERR_FILE_EMPTY, ERR_NO_VALID_DATA,
    ERR_INVALID_TAIL_NUMBER, ERR_INVALID_FLIGHT_NUMBER, ERR_INVALID_TIME_FORMAT,
    ERR_INCOMPLETE_DATA, ERR_MISSING_TIME, ERR_EXTRA_CHARS, ERR_DUPLICATE_TAIL_TIME,
    ERR_DUPLICATE_FLIGHT_TIME, ERR_TAIL_BRAND_MISMATCH
};

// Функция читает файл, контролируя, что он существует и не пуст. Подсчитывает
//количество ЛА в файле (всех и корректных), далее - начинает построчную обработку данных и запись в массив ЛА.
ErrCode Read_data(const char* file_name, Aircraft *&all_arr,
    int &total_lines, Aircraft *&valid_arr ,int &valid_count, int*&B);
// Функция обрабатывает строку, контролируя, что она не пустая и данные корректны.
int Parse_line(char* line, Aircraft &arr);

// Проверка дубликатов
int CheckDuplicates(Aircraft *arr, int valid_count, Aircraft &new_aircraft, char* break_str);
// Функция проверяет коректность бортового номера.
int Check_tail_number(char* num);
// Функция проверять корректность номера рейса
int Check_flight_number(char* flight_str);
//Функция обрабатывает цифровую часть бортового номера.
int Number(char *extra_s);
//Функция обрабатывает время и контролирует корректность времени посадки.
int Check_time(char* extra_s, unsigned int &hour, unsigned int &min);
//Функция выводит текст ошибки
void Print_message(ErrCode code, char* brand);
//Функция выводит отсортированный массив ЛА
void Print(Aircraft *&arr, int N, int *&B);
//Функция выводит исходный массив ЛА
void PrintOriginal(Aircraft *arr, int N);
//Индексная сортировка
void IndexSort(Aircraft *&arr, int N, int *&B);
//Функция считает длину строки
int Length_of(char* str);
//Функция считает длину числа
int Length_of_number(int num);

int main() { // Начало main
    Aircraft* all_arr = nullptr; // Массив всез ЛА
    int total_lines = 0; // Количество всех ЛА
    Aircraft* valid_arr = nullptr; // Массив корректных ЛА
    int valid_count = 0; // Количество коррректных ЛА
    int* B = nullptr; // Массив для индексной сортировки
    char* break_str = new char[max_len]; // Строка для вывода ошибки
    break_str[0] = '\0';

    cout << "Запуск файла " << filename << endl;

    // Читаем файл
    ErrCode err = Read_data(filename, all_arr, total_lines, valid_arr, valid_count, B);
    if (err == ERR_FILE_NOT_FOUND || err == ERR_FILE_EMPTY)
    { // Если критическая ошибка - нет файла или он пуст
        Print_message(err, break_str); // Сообщение об ошибке
        delete[] break_str; // Очистка памяти
        return 1; // Конец работы программы
    }

    cout << "Содержимое файла:" << endl;
    PrintOriginal(all_arr, total_lines);   // Выводим исходные строки


    if (valid_count > 0)
    {// Если есть корректные строки
        IndexSort(valid_arr, valid_count, B); // Сортируем их
        cout << "\nОтсортированные корректные записи:" << endl;
        Print(valid_arr, valid_count, B); // Вывод таблицы
    } else
    {
        cout << "\nНет корректных записей для сортировки." << endl;
    }
    // Очистка памяти
    delete[] all_arr;
    delete[] valid_arr;
    delete[] B;
    delete[] break_str;
    return 0;
} // Конец main

ErrCode Read_data(const char* file_name, Aircraft *&all_arr,
    int &total_lines, Aircraft *&valid_arr, int &valid_count, int*&B)
{
    // Подсчет непустых строк в файле
    ifstream file_0(file_name);
    if (!file_0.is_open()) return ERR_FILE_NOT_FOUND;
    if (file_0.peek() == EOF) return ERR_FILE_EMPTY;

    char extra_s[max_len]; // Буфер для считывания файла построчно
    total_lines = 0; // Количество всех строк
    while (file_0.getline(extra_s, max_len)) // Считываем непусиые строки
        if (extra_s[0] != '\0') total_lines++;
    file_0.close();

    if (total_lines == 0) return ERR_FILE_EMPTY; // Строк нет - ошибка (файл пуст)

    all_arr = new Aircraft[total_lines]; // Массив всех строк
    valid_arr = new Aircraft[total_lines]; // Массив корректных строк
    ifstream file(file_name); // Открываем файл повторно

    valid_count = 0; // Количество корректных строк
    int idx = 0; // Индекс строки для массива всех строк
    int line_number = 0; // Номер строки для вывода ошибки

    while (file.getline(extra_s, max_len)) {
        line_number++;
        if (extra_s[0] == '\0') continue; // Пустые строки пропускаем

        strcpy(all_arr[idx].original_line, extra_s);// Сохраняем оригинал строки в all_arr

        Aircraft temp; // Шаблон структуры

        int line_error = Parse_line(extra_s, temp);// Обработка строки

        if (line_error != 0) { // Строка некорректна
            // Копируем поля из temp (частичные) и помечаем как невалидную
            strcpy(all_arr[idx].brand, temp.brand);
            strcpy(all_arr[idx].tail_number, temp.tail_number);
            all_arr[idx].flight_number = temp.flight_number;
            all_arr[idx].boarding_hour = temp.boarding_hour;
            all_arr[idx].boarding_min = temp.boarding_min;
            all_arr[idx].is_valid = false; // Строка некорректна

            ErrCode err; // Значение ошибки
            // Преобразуем числовую ошибку в символьную константу
            switch (line_error) {
                case 1: err = ERR_INVALID_TAIL_NUMBER; break;
                case 4: err = ERR_INVALID_TIME_FORMAT; break;
                case 5: err = ERR_INCOMPLETE_DATA; break;
                case 7: err = ERR_MISSING_TIME; break;
                case 8: err = ERR_INVALID_FLIGHT_NUMBER; break;
                case 12: err = ERR_EXTRA_CHARS; break;
                default: err = OK; break;
            }
            Print_message(err, temp.brand);
            cout << " (Строка " << line_number << ")" << endl;
            idx++;
            continue;
        }

        // Проверка дубликатов (только среди корректных)
        char dup_brand[max_len]; // Марка ЛА дубликата
        int dup_error = CheckDuplicates(valid_arr, valid_count, temp, dup_brand);
        if (dup_error != 0) {
            // Дубликат – не добавляем в valid_arr, но в all_arr сохраняем
            strcpy(all_arr[idx].brand, temp.brand);
            strcpy(all_arr[idx].tail_number, temp.tail_number);
            all_arr[idx].flight_number = temp.flight_number;
            all_arr[idx].boarding_hour = temp.boarding_hour;
            all_arr[idx].boarding_min = temp.boarding_min;
            all_arr[idx].is_valid = false;

            ErrCode err; // Значение ошибки
            switch (dup_error) {
                case 9: err = ERR_DUPLICATE_TAIL_TIME; break;
                case 10: err = ERR_DUPLICATE_FLIGHT_TIME; break;
                case 11: err = ERR_TAIL_BRAND_MISMATCH; break;
                default: err = OK; break;
            }
            Print_message(err, dup_brand);
            cout << " (Строка " << line_number << ")" << endl;
            idx++;
            continue;
        }

        // Полностью корректная и уникальная строка
        strcpy(all_arr[idx].brand, temp.brand);
        strcpy(all_arr[idx].tail_number, temp.tail_number);
        all_arr[idx].flight_number = temp.flight_number;
        all_arr[idx].boarding_hour = temp.boarding_hour;
        all_arr[idx].boarding_min = temp.boarding_min;
        all_arr[idx].is_valid = true;

        // Добавляем в массив для сортировки
        valid_arr[valid_count] = temp;
        valid_count++;
        idx++;
    }
    file.close(); // Закрываем файл

    // Массив индексов для сортировки
    B = new int[valid_count];
    for (int i = 0; i < valid_count; i++) B[i] = i;
    return OK;
}

int CheckDuplicates(Aircraft *arr, int valid_count, Aircraft &new_aircraft, char* break_str) {
    for (int k = 0; k < valid_count; k++)
    {// Начало цикла обработки строк
        // 1) Одинаковый бортовой номер
        if (strcmp(arr[k].tail_number, new_aircraft.tail_number) == 0 && strcmp(arr[k].brand, new_aircraft.brand) == 0)
        {
            if (arr[k].boarding_hour == new_aircraft.boarding_hour && arr[k].boarding_min == new_aircraft.boarding_min)
            {
                // Одинаковый борт, одиноковое время вылета
                strcpy(break_str, new_aircraft.brand);
                return 9;
            }
        }
        if (arr[k].flight_number == new_aircraft.flight_number)
        {
            // Одинаковый номер рейса, одиноковое время вылета
            if (arr[k].boarding_hour == new_aircraft.boarding_hour && arr[k].boarding_min == new_aircraft.boarding_min)
            {
                // Одинаковый борт, одиноковое время вылета
                strcpy(break_str, new_aircraft.brand);
                return 10;
            }
        }

        // 2) Одинаковый бортовой номер, разные марки
        if (strcmp(arr[k].tail_number, new_aircraft.tail_number) == 0)
        {
            if (strcmp(arr[k].brand, new_aircraft.brand) != 0)
            {
                strcpy(break_str, new_aircraft.brand);
                return 11;
            }
        }
    }// Конец цикла
    return 0;
}

int Parse_line(char* line, Aircraft &arr) {
    int error_code = 0; //Код ошибки
    int pos = 0; // Индекс строки

    while (line[pos] == ' ') pos++;
    // Проверка корректности марки ЛА
    int i = 0;
    while (line[pos] != ' ' && line[pos] != '\0' && i < max_len - 1) {
        arr.brand[i++] = line[pos++];
    }
    arr.brand[i] = '\0';
    if (line[pos] == '\0')
    {
        if (error_code != 0)
        {
            return error_code;
        } else
        {
            return 5;
        }
    }

    while (line[pos] == ' ') pos++;

    // Бортовой номер
    i = 0;
    while (line[pos] != ' ' && line[pos] != '\0' && i < max_len - 1) {
        arr.tail_number[i++] = line[pos++];
    }
    arr.tail_number[i] = '\0';
    int tail_err = Check_tail_number(arr.tail_number);
    if (tail_err) error_code = tail_err; //Запоминаем ошибку
    if (line[pos] == '\0')
    {
        if (error_code != 0)
        {
            return error_code;
        } else
        {
            return 5;
        }
    }

    while (line[pos] == ' ') pos++;

    // Номер рейса
    char flight_str[max_len];
    i = 0;
    while (line[pos] != ' ' && line[pos] != '\0' && i < max_len - 1) {
        flight_str[i++] = line[pos++];
    }
    flight_str[i] = '\0';
    int flight_err = Check_flight_number(flight_str);
    if (flight_err) error_code = flight_err;
    arr.flight_number = Number(flight_str); // Извлекаем число (0, если нет цифр)
    if (line[pos] == '\0')
    {
        if (error_code != 0)
        {
            return error_code;
        } else
        {
            return 5;
        }
    }

    while (line[pos] == ' ') pos++;

    // Время
    char time_str[max_len];
    i = 0;
    while (line[pos] != ' ' && line[pos] != '\0' && i < max_len - 1) {
        time_str[i++] = line[pos++];
    }
    time_str[i] = '\0';
    unsigned int h = 0, m = 0;
    int time_err = Check_time(time_str, h, m);
    if (time_err) error_code = time_err;
    arr.boarding_hour = h;
    arr.boarding_min = m;

    // Проверка лишних символов в конце строки
    if (line[pos] != '\0')
    {
        while (line[pos] != '\0')
        {
            if (line[pos] == ' ') pos++;
            else
            {
                if (line[pos] == '\0')
                {
                    if (error_code != 0)
                    {
                        return error_code;
                    } else
                    {
                        return 12;
                    }
                }
            }
        }
    }
    return error_code; // 0, если ошибок не было
}
int Check_tail_number(char* num) {
    const int len_tail = 4; // Количество цифр в бортовом номере
    if (num == nullptr || num[0] == '\0') return 1;
    int i = 0; // Индекс поля
    if ((unsigned char)num[0] == 0xD0 && (unsigned char)num[1] == 0x91)
    // Буква Б - 2 байта
        i = 2;
    else return 1;

    if (num[i]!= '-') return 1;
    i++;
    if (num[i] == '\0') return 1;
    for (int j = 0; j < len_tail; j++)
    { //Процерка цифровой части
        if (num[i+j] < '0' || num[i+j] > '9') return 1;
    }
    if (num[i+len_tail] != '\0') return 1; // Проверка длины цифровой части
    return 0;
}


int Check_flight_number(char* flight_str) {
    // Первые 4 символа - "РЕЙС"
    if (flight_str == nullptr || flight_str[0] == '\0') return 1;
    const unsigned char prefix[] = {0xD0,0xA0, 0xD0,0x95, 0xD0,0x99, 0xD0,0xA1}; // РЕЙС
    for (int i = 0; i < 8; i++) if ((unsigned char)flight_str[i] != prefix[i]) return 8;

    // Одна кириллическая буква - 2 символа
    int i = 8;
    if (flight_str[i] == '-') i++;
    int digit_count = 0;

    // Считывание цифровой части
    while (flight_str[i] != '\0')
    {
        if (flight_str[i] < '0' || flight_str[i] > '9') return 8;
        digit_count++;
        i++;
    }
    if (digit_count >= 1 && digit_count <= 9) return 0; // Корректный формат

    return 8; // Некорректный формат
}
int Number(char *extra_s) {
    int sum = 0;
    int h = 0;
    bool flag = false; // Флаг положительности/отрицательности числа
    while (extra_s[h] != '\0'){
        if (extra_s[h] == '-') //Если перед номером рейса минус
        {
            flag = true; // Число отрицательное
        }
        else if (extra_s[h] >= '0' && extra_s[h] <= '9') //Если цифра
        {
            sum = sum * 10 + (extra_s[h] - '0'); //Формируем номер
        }
        h++;
    }
    if (flag == true)
    {
        sum *= -1;
    }

    return sum;
}

int Check_time(char* extra_s, unsigned int &hour, unsigned int &min){
    if (extra_s == nullptr || extra_s[0] == '\0') return 7;
    int i = 0;
    for (int j = 0; j < 2; j++)
    { // Час не может задаваться одной цифрой
        if (extra_s[i+j] >= '0' && extra_s[i+j] <= '9')
        {
            hour = hour * 10 + (extra_s[i+j] - '0');
        }
        else
        {
            return 4;
        }
    }

    i+=2;
    if (extra_s[i] != ':') return 4;

    i++;

    int min_digits = 0;
    for (int j = 0; j< 2; j++) {// Минуты не могут задаваться одной цифрой
        if (extra_s[i+j] >= '0' && extra_s[i+j] <= '9')
        {
            min = min * 10 + (extra_s[i+j] - '0');
            min_digits++;
        }
        else return 4;

    }

    if (hour > 23 || min > 59) return 4; // Вывод за границы формата времени

    return 0;
}

void Print_message(ErrCode code, char* brand) {
    switch (code) {
        case OK:
            break;
        case ERR_FILE_NOT_FOUND:
            cout << "Ошибка! Файл не найден" << endl;
            break;
        case ERR_FILE_EMPTY:
            cout << "Ошибка! Файл пустой" << endl;
            break;
        case ERR_NO_VALID_DATA:
            cout << "В файле отсутствуют корректные данные" << endl;
            break;
        case ERR_INVALID_TAIL_NUMBER:
            cout << "Ошибка! Неверный бортовой номер";
            if (brand) cout << " ЛА " << brand;
            cout << endl;
            break;
        case ERR_INVALID_FLIGHT_NUMBER:
            cout << "Ошибка! Некорректный номер рейса";
            if (brand) cout << " ЛА " << brand;
            cout << endl;
            break;
        case ERR_INVALID_TIME_FORMAT:
            cout << "Ошибка! Некорректное время (должен быть формат ЧЧ:ММ)";
            if (brand) cout << " ЛА " << brand;
            cout << endl;
            break;
        case ERR_INCOMPLETE_DATA:
            cout << "Ошибка! Нехватает данных для";
            if (brand) cout << " ЛА " << brand;
            cout << endl;
            break;
        case ERR_MISSING_TIME:
            cout << "Ошибка! Время посадки отсутствует";
            if (brand) cout << " ЛА " << brand;
            cout << endl;
            break;
        case ERR_EXTRA_CHARS:
            cout << "Лишние символы в конце строки";
            if (brand) cout << " ЛА " << brand;
            cout << endl;
            break;
        case ERR_DUPLICATE_TAIL_TIME:
            cout << "Одинаковый борт в одно время";
            if (brand) cout << " ЛА " << brand;
            cout << endl;
            break;
        case ERR_DUPLICATE_FLIGHT_TIME:
            cout << "Самолеты с одинаковым номером рейса не могут вылетать в одно и то же время";
            if (brand) cout << " ЛА " << brand;
            cout << endl;
            break;
        case ERR_TAIL_BRAND_MISMATCH:
            cout << "Каждой модели самолета должен соответствовать уникальный бортовой номер";
            if (brand) cout << " ЛА " << brand;
            cout << endl;
            break;
        default:
            cout << "Неизвестная ошибка" << endl;
            break;
    }
}

int Length_of(char* str) {
    if (str == nullptr) return 0;
    int len = 0;
    int i = 0;
    while (str[i] != '\0')
    {
        unsigned char c = (unsigned char)str[i];
        if (c >= 0xC0)
        { // Двухбайтовый символ (буква)
            len++;
            i+=2;
        }
        else
        {
            len++;
            i++;
        }
    }
    return len;
}

int Length_of_number(int num) {
    if (num == 0) return 1;
    int len = 0;
    if (num < 0)
    { // Учитываем минус
        len = 1;
        num = -num;
    }
    while (num > 0)
    {
        len++;
        num /= 10;
    }
    return len;
}

void PrintOriginal(Aircraft *arr, int N) {
    cout << "------------------------------------------" << endl;
    cout << "| № | Исходная строка из файла            |" << endl;
    cout << "------------------------------------------" << endl;
    for (int i = 0; i < N; i++)
    {
        cout << "| " << (i+1);
        if (i+1 < 10) cout << " ";
        cout << " | " << arr[i].original_line;
        int len = strlen(arr[i].original_line);
        for (int k = len; k < 41; k++) cout << " ";
        cout << "|" << endl;
    }
    cout << "------------------------------------------" << endl;
}


void Print(Aircraft *&arr, int N, int *&B) {
    cout << " ----------------------------------------------------------------" << endl;
    cout << "| № | Марка ЛА    | Бортовой номер | Номер рейса | Время посадки |" << endl;
    cout << " ----------------------------------------------------------------" << endl;
    for (int i = 0; i < N; i++)
    {
        cout << "|";

        if (i + 1 < 10) cout << " " << (i + 1) << " ";
        else cout << (i + 1) << " ";

        cout << "| ";

        int brand_len = Length_of(arr[B[i]].brand);
        cout << arr[B[i]].brand;

        for (int k = 0; k < 12 - brand_len; k++) cout << " ";

        cout << "| ";
        cout << arr[B[i]].tail_number;
        for (int k = 0; k < 15 - Length_of(arr[B[i]].tail_number); k++) cout << " ";
        cout << "| РЕЙС";
        cout << arr[B[i]].flight_number;
        for (int k = 0; k < 12 - Length_of_number(arr[B[i]].flight_number); k++) cout << " ";
        cout << "| ";

        // Время посадки
        if (Length_of_number(arr[B[i]].boarding_hour) == 1) cout << "0";
        cout << arr[B[i]].boarding_hour << ":";
        if (Length_of_number(arr[B[i]].boarding_min) == 1) cout << "0";
        cout << arr[B[i]].boarding_min << "         |" << endl;
    }
    cout << " ----------------------------------------------------------------" << endl;
}

void IndexSort(Aircraft *&arr, int N, int *&B){
    int i, j, t = 0;
    for (i = 0; i < N - 1; i++)
    {
        int ind = i;
        for (j = i + 1; j < N; ++j)
        {
            if (arr[B[j]].flight_number < 0 && arr[B[ind]].flight_number >= 0)
                ind = j;
            if (arr[B[j]].flight_number >= 0 && arr[B[ind]].flight_number < 0)
                continue;
            if (arr[B[j]].flight_number < 0 && arr[B[ind]].flight_number < 0) {
                if (arr[B[j]].flight_number > arr[B[ind]].flight_number)
                    ind = j;
            }
            if (arr[B[j]].flight_number >= 0 && arr[B[ind]].flight_number >= 0) {
                if (arr[B[j]].boarding_hour == arr[B[ind]].boarding_hour) {
                    if (arr[B[j]].boarding_min < arr[B[ind]].boarding_min)
                        ind = j;
                }
                if (arr[B[j]].boarding_hour < arr[B[ind]].boarding_hour)
                    ind = j;
            }
        }
        t = B[i];
        B[i] = B[ind];
        B[ind] = t;
    }
}