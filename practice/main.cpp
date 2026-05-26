/***************************************************************************
*Project Type: Command-Line Tool                                      *
*Project Name: practice                                                    *
*File Name: main.cpp                                                       *
*Language: C++, Clion 2025.2.1                                   		   *                                       	   *
*Programmer: Баева Алёна Артёмовна                                         *
*Modified by:           									               *
*Created: 05.05.2026                                                       *
*Last Revision: 25.05.26                                    			   *
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
const char filename[] = "tests/test4.txt"; //Корректный тест

typedef struct {
    char brand[max_len]; // Марка ЛА
    char tail_number[max_len]; // Бортовой номер
    int flight_number; // Номер рейса
    unsigned int boarding_hour; // Час посадки
    unsigned int boarding_min; // Время посадки
} Aircraft;

// Функция читает файл, контролируя, что он существует и не пуст. Подсчитывает
//количество ЛА в файле, далее - начинает построчную обработку данных и запись в массив ЛА.
int Open_file(const char* file_name, Aircraft *&arr, int &N, char* break_str);
// Функция обрабатывает строку, контролируя, что она не пустая и данные полные.
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
void Print_error(int type, char* break_str);
//Функция выводит массив ЛА
void Print(Aircraft *&arr, int N, int *&B);
//Индексная сортировка
void IndexSort(Aircraft *&arr, int N, int *&B);
//Функция считает длину строки
int Length_of(char* str);
//Функция считает длину числа
int Length_of_number(int num);

int main(){
    Aircraft* arr = nullptr;
    int N = 0;
    char* break_str = new char[max_len];
    break_str[0] = '\0';
    int error = Open_file(filename, arr, N, break_str);
    if (error == -1) {
        cout << "В файле отсутствуют корректные данные";
        return 1;
    }
    if (error != 0) {
        Print_error(error, break_str);
        cout << endl;
        delete [] break_str;
    }
    int* B = new int[N];
    for (int i = 0; i < N; i++) B[i] = i;
    Print(arr, N, B); // Вывод исходных данных
    IndexSort(arr, N, B);
    cout << endl;
    Print(arr, N, B); // Вывод отсортированных данных

    delete[] arr;
    delete[] B;
    delete [] break_str;
    return 0;
}
int Open_file(const char* file_name, Aircraft *&arr, int &N, char* break_str)
{
    ifstream file_0(file_name);
    if (!file_0.is_open()) return 2;
    if (file_0.peek() == EOF) return 3;

    char extra_s[max_len];
    int total_lines = 0;

    // Первый проход — считаем непустые строки
    while (file_0.getline(extra_s, max_len)) {
        if (extra_s[0] != '\0')
            total_lines++;
    }
    file_0.close();

    if (total_lines == 0) return 3;

    ifstream file(file_name);
    arr = new Aircraft[total_lines];
    int valid_count = 0;// Реальное количество корректных уникальных записей
    int line_number = 0;// Для вывода ошибки

    while (file.getline(extra_s, max_len))
    {
        line_number++;
        if (extra_s[0] == '\0') continue;

        int line_error = Parse_line(extra_s, arr[valid_count]);

        if (line_error != 0)
        {
            // Копируем бренд для вывода ошибки
            int j = 0;
            while (arr[valid_count].brand[j] != '\0' && j < max_len-1) {
                break_str[j] = arr[valid_count].brand[j];
                j++;
            }
            break_str[j] = '\0';
            Print_error(line_error, break_str);
            cout << " (Строка " << line_number << ")" << endl;
            if (line_error == 12) {

            }
            else continue;// Строка игнорируется
        }
        int dup_error = CheckDuplicates(arr, valid_count, arr[valid_count], break_str);
        if (dup_error != 0) { // Есть дубликаты
            Print_error(dup_error, break_str);
            cout << " (Строка " << line_number << ")" << endl;
            continue; // Не добавляем запись
        }

        valid_count++;// Только корректная запись
    }

    file.close();

    N = valid_count; // Обновляем N до реального количества корректных ЛА
    if (N == 0) return -1; // Если нет корректных записей
    return 0;
}

int CheckDuplicates(Aircraft *arr, int valid_count, Aircraft &new_aircraft, char* break_str) {
    for (int k = 0; k < valid_count; k++) {
        // 1) Одинаковый бортовой номер
        if (strcmp(arr[k].tail_number, new_aircraft.tail_number) == 0 && strcmp(arr[k].brand, new_aircraft.brand) == 0) {
            if (arr[k].boarding_hour == new_aircraft.boarding_hour && arr[k].boarding_min == new_aircraft.boarding_min) {
                // Одинаковый борт, одиноковое время вылета
                strcpy(break_str, new_aircraft.brand);
                return 9;
            }
        }
        if (arr[k].flight_number == new_aircraft.flight_number) {
            // Одинаковый номер рейса, одиноковое время вылета
            if (arr[k].boarding_hour == new_aircraft.boarding_hour && arr[k].boarding_min == new_aircraft.boarding_min) {
                // Одинаковый борт, одиноковое время вылета
                strcpy(break_str, new_aircraft.brand);
                return 10;
            }
        }

        // 2) Одинаковый бортовой номер, разные марки
        if (strcmp(arr[k].tail_number, new_aircraft.tail_number) == 0) {
            if (strcmp(arr[k].brand, new_aircraft.brand) != 0) {
                strcpy(break_str, new_aircraft.brand);
            return 11;
            }
        }
    }
    return 0;
}

int Parse_line(char* line, Aircraft &arr) {
    int pos = 0;

    while (line[pos] == ' ') pos++;
    //Считываем марку
    int i = 0;
    while (line[pos] != ' ' && line[pos] != '\0' && i < max_len-1) {
        arr.brand[i++] = line[pos++];
    }
    arr.brand[i] = '\0';
    if (line[pos] == '\0') return 6;

    while (line[pos] == ' ') pos++;

    // Считываем бортовой номер
    i = 0;
    while (line[pos] != ' ' && line[pos] != '\0' && i < max_len-1) {
        arr.tail_number[i++] = line[pos++];
    }
    arr.tail_number[i] = '\0';
    if (Check_tail_number(arr.tail_number)!=0) return Check_tail_number(arr.tail_number);
    if (line[pos] == '\0') return 8;
    while (line[pos] == ' ') pos++;

    //Считываем номер рейса
    char flight_str[max_len];
    i = 0;
    while (line[pos] != ' ' && line[pos] != '\0' && i < max_len-1) {
        flight_str[i++] = line[pos++];
    }
    flight_str[i] = '\0';

    if (Check_flight_number(flight_str) != 0) return 8;
    arr.flight_number = Number(flight_str);
    if (line[pos] == '\0') return 7;
    while (line[pos] == ' ') pos++;

    //Считываем время
    char time_str[max_len] = {0};
    i = 0;
    while (line[pos] != ' ' && line[pos] != '\0' && i < max_len-1) {
        time_str[i++] = line[pos++];
    }
    time_str[i] = '\0';
    unsigned int h = 0, m = 0;
    if (Check_time(time_str, h, m)!=0) return Check_time(time_str, h, m);

    arr.boarding_hour = h;
    arr.boarding_min = m;

    // Проверка конца строки
    if (line[pos++] != '\0') {
        while (line[pos] != '\0') {
            if (line[pos] == ' ') pos++;
            else return 12;
        }
    }

    return 0;
}

int Check_tail_number(char* num) {
    const int len_tail = 4; // Количество цифр в бортовом номере
    if (num == nullptr || num[0] == '\0') return 6;
    int i = 0;
    if ((unsigned char)num[0] == 0xD0 && (unsigned char)num[1] == 0x91){ // Буква Б
        i = 2;
    }
    else{
        return 1;
    }
    if (num[i]!= '-'){
        return 1;
    }
    i++;

    if (num[i] == '\0') {
        return 1;
    }
    for (int j = 0; j < len_tail; j++) {
        if (num[i+j] < '0' || num[i+j] > '9') return 1;
    }
    if (num[i+len_tail] != '\0') return 1;
    return 0;
}

int Check_flight_number(char* flight_str)
{
    if (flight_str == nullptr || flight_str[0] == '\0') return 8;
    const unsigned char prefix[] = {0xD0,0xA0, 0xD0,0x95, 0xD0,0x99, 0xD0,0xA1}; // РЕЙС
    for (int i = 0; i < 8; i++) if ((unsigned char)flight_str[i] != prefix[i]) return 8;

    int i = 8;
    if (flight_str[i] == '-') i++;
    int digit_count = 0;

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
    bool flag = false;
    while (extra_s[h] != '\0'){
        if (extra_s[h] == '-')
        {
            flag = true;
        }
        else if (extra_s[h] >= '0' && extra_s[h] <= '9') {
            sum = sum * 10 + (extra_s[h] - '0');
        }
        h++;
    }
    if (flag == true) {
        sum *= -1;
    }

    return sum;
}

int Check_time(char* extra_s, unsigned int &hour, unsigned int &min){
    if (extra_s == nullptr || extra_s[0] == '\0') return 7;
    int i = 0;
    for (int j = 0; j < 2; j++) { // Час не может задааваться одной цифрой
        if (extra_s[i+j] >= '0' && extra_s[i+j] <= '9') {
            hour = hour * 10 + (extra_s[i+j] - '0');
        }
        else {
            return 4;
        }
    }

    i+=2;
    if (extra_s[i] != ':') return 4;

    i++;

    int min_digits = 0;
    for (int j = 0; j< 2; j++) {// Минуты не могут задааваться одной цифрой
        if (extra_s[i+j] >= '0' && extra_s[i+j] <= '9') {
            min = min * 10 + (extra_s[i+j] - '0');
            min_digits++;
        }
        else return 4;

    }

    if (hour > 23 || min > 59) return 4; // Вывод за границы формата времени

    return 0;
}

void Print_error(int type, char* break_str) {
    if (type == 1) cout << "Ошибка! Неверный бортовой номер";
    if (type == 2) cout << "Ошибка! Файл не найден" << endl;
    if (type == 3) cout << "Ошибка! Файл пустой" << endl;
    if (type == 4) cout << "Ошибка! Некорректное время (должен быть формат ЧЧ:ММ)";
    if (type == 6) cout << "Ошибка! Бортовой номер отсутствует";
    if (type == 7) cout << "Ошибка! Время посадки отсутствует";
    if (type == 8) cout << "Ошибка! Номер рейса некорректен";
    if (type == 9) cout << "Одинаковый борт в одно время";
    if (type == 10) cout << "Самолеты с одинаковым номером рейса не"
                           " могут вылетать в одно и то же время";
    if (type == 11) cout << "Каждой модели самолета должен соответствовать уникальный"
                            " бортовой номер";
    if (type == 12) cout << "Лишние символы в конце строки";
    cout << " ЛА " << break_str << endl;
}

int Length_of(char* str) {
    if (str == nullptr) return 0;
    int len = 0;
    int i = 0;
    while (str[i] != '\0') {
        unsigned char c = (unsigned char)str[i];
        if (c >= 0xC0) { // Двухбайтовый символ (буква)
            len++;
            i+=2;
        }
        else {
            len++;
            i++;
        }
    }
    return len;
}

int Length_of_number(int num) {
    if (num == 0) return 1;
    int len = 0;
    if (num < 0) { // Учитываем минус
        len = 1;
        num = -num;
    }
    while (num > 0) {
        len++;
        num /= 10;
    }
    return len;
}

void Print(Aircraft *&arr, int N, int *&B) {
    cout << " ----------------------------------------------------------------" << endl;
    cout << "| № | Марка ЛА    | Бортовой номер | Номер рейса | Время посадки |" << endl;
    cout << " ----------------------------------------------------------------" << endl;
    for (int i = 0; i < N; i++) {
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