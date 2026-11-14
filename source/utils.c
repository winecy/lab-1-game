#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"

// чтение int с защитой от дурака-------------------------------------------

int read_int_safe(const char *prompt, int min, int max)
{
    int value;
    int ch;
    while (1)
    {
        printf("%s", prompt);
        if (scanf("%d", &value) != 1)
        {
            printf("Ошибка ввода! Введите число.\n");
            while ((ch = getchar()) != '\n' && ch != EOF) { }
            if (ch == EOF) return min;
            continue;
        }
        while ((ch = getchar()) != '\n' && ch != EOF) { }
        if (value < min || value > max)
        {
            printf("Число должно быть от %d до %d.\n", min, max);
            continue;
        }
        return value;
    }
}

// чтение строки с защитой от дурака----------------------------------------

void read_string_safe(const char *prompt, char *buffer, int size)
{
    while (1)
    {
        printf("%s", prompt);
        if (fgets(buffer, size, stdin) == NULL)
        {
            buffer[0] = '\0';
            return;
        }
        buffer[strcspn(buffer, "\n")] = '\0';
        if (strlen(buffer) > 0) return;
        printf("Строка не может быть пустой.\n");
    }
}

// загрузка каталога предметов----------------------------------------------

void load_item_names(char item_names[MAX_ITEMS][MAX_NAME_LEN])
{
    char default_names[MAX_ITEMS][MAX_NAME_LEN] = {
        "Пусто", "Дерево", "Камень", "Семена", "Картошка",
        "Кирка", "Пшено", "Грибы", "Седло", "Морковь"
    };

    FILE *file = fopen(ITEMS_FILE, "r");
    if (file == NULL)
    {
        printf("\n[Предупреждение]: Файл %s не найден! Загружены названия по умолчанию.\n", ITEMS_FILE);
        for (int i = 0; i < MAX_ITEMS; i++)
            strcpy(item_names[i], default_names[i]);
        return;
    }

    int id;
    char name[MAX_NAME_LEN];
    int count = 0;
    while (fscanf(file, "%d %31s", &id, name) == 2)
    {
        if (id >= 0 && id < MAX_ITEMS)
        {
            strncpy(item_names[id], name, MAX_NAME_LEN - 1);
            item_names[id][MAX_NAME_LEN - 1] = '\0';
            count++;
        }
    }
    fclose(file);
    printf("Каталог предметов успешно загружен(%d предметов).\n", count);
}

// анализ логов-------------------------------------------------------------

void analyze_logs(void)
{
    FILE *in = fopen(INPUT_FILE, "r");
    if (in == NULL)
    {
        printf("Ошибка: не удалось открыть файл %s для чтения!\n", INPUT_FILE);
        return;
    }
    FILE *out = fopen(OUTPUT_FILE, "w");
    if (out == NULL)
    {
        printf("Ошибка: не удалось создать файл %s для записи!\n", OUTPUT_FILE);
        fclose(in);
        return;
    }

    char line[256];
    int line_number = 0;
    int info_count = 0, warn_count = 0, error_count = 0;

    printf("\n--- Обработка логов ---\n");
    while (fgets(line, sizeof(line), in) != NULL)
    {
        line_number++;
        if (strncmp(line, "[INFO]", 6) == 0) info_count++;
        else if (strncmp(line, "[WARN]", 6) == 0)
        {
            warn_count++;
            char *message = line + 6;
            printf("[%d] Строка %d: %s", warn_count + error_count, line_number, message);
            fprintf(out, "Строка %d: %s", line_number, message);
        }
        else if (strncmp(line, "[ERROR]", 7) == 0)
        {
            error_count++;
            char *message = line + 7;
            printf("[%d] Строка %d: %s", warn_count + error_count, line_number, message);
            fprintf(out, "Строка %d: %s", line_number, message);
        }
    }
    printf("\n=== ОБЩАЯ СТАТИСТИКА ЛОГОВ ===\n");
    printf("Всего строк обработано: %d\n", line_number);
    printf("[INFO]:  %d\n", info_count);
    printf("[WARN]:  %d\n", warn_count);
    printf("[ERROR]: %d\n", error_count);
    printf("==============================\n");
    fclose(in);
    fclose(out);
    printf("Результаты фильтрации успешно сохранены в файл output.txt.\n");
}
