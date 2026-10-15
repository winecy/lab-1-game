#include <stdio.h>
#include <string.h>

#define inventory_size 10
#define player_name_max 32
#define MAX_ITEMS 10
#define MAX_NAME_LEN 50

int current_day = 1;
int current_hour = 8;
int userAction;
int work_time;
int item_slot;
int item_id;

char player_name[player_name_max];
char item_names[MAX_ITEMS][MAX_NAME_LEN];



int inventory[inventory_size] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9
};  




void case1_time() 
{
  printf("Текущее время: %d день %d час\n", current_day, current_hour);
}

void case2_work() 
{
  printf("Сколько работаем?\n");
  scanf("%d", &work_time);
  if (work_time>=0){
    current_hour += work_time;
    printf("Работаем...\n");
    if (current_hour >= 24) 
        {
        current_day += current_hour / 24;
        current_hour = current_hour % 24;
        }
  }else{
    printf("Такое невозможно.Введите корректное количество часов.\n");
  }
}

void case3_show_inventory() 
{
  printf("Инвентарь:\n");
  for (int i = 0; i < inventory_size; i++) 
    {
    int id = inventory[i];
    
    if (id >= 0 && id < MAX_ITEMS) 
    {
        printf("Слот %d: [%d] - %s\n", i, id, item_names[id]);
    } 
    else 
    {
        printf("Слот %d: [%d] - Неизвестный предмет\n", i, id);
    }
  }
}

void case4_put_item() 
{
  printf("Выберите ячейку инвентаря:\n");
  
  if (scanf("%d", &item_slot) != 1) 
    {
    printf("Ошибка ввода! Введите число от 0 до 9\n");
    while (getchar() != '\n'); // чистка буфера
    return; // вместо break в функции пишется return
    }
  
  if (item_slot > 9 || item_slot < 0) 
    {
    printf("Неверный номер слота!Введите значение от 0 до 9\n");
    return;
    } else {
    
    printf("Выберите id предмета:\n");
    
    if (scanf("%d", &item_id) != 1) 
      {
      printf("Ошибка ввода! Введите число от 0 до 9.\n");
      while (getchar() != '\n'); // чистка буфера
      return;
      }
    
    if (item_id > 9 || item_id < 0) 
      {
      printf("Такого предмета нет!Введите id предмета от 0 до 9\n");
      while (getchar() != '\n'); // чистка буфера
      return;
      } else {
      
      inventory[item_slot] = item_id;
      return;
      }
  }
}

void case5_drop_item() 
{
  printf("Выберите ячейку инвентаря:\n");
  
  if (scanf("%d", &item_slot) != 1) 
    {
    printf("Ошибка ввода! Введите число от 0 до 9\n");
    while (getchar() != '\n'); // чистка буфера
    return;
    }
  
  if (item_slot > 9 || item_slot < 0) 
    {
    printf("Неверный номер слота!Введите значение от 0 до 9\n");
    return;
    } else {
      inventory[item_slot] = 0;
      return;
    }
}

void case6_unique() 
{
  int unique_items = 0;
  int items[10] = {0};
  for (int i = 0; i < inventory_size; i++) 
    {
    int id = inventory[i];
    if (id >= 0 && id <= 9) 
      {
      items[id]++;
      }
    }
  for (int id = 1; id <= 9; id++) // начинаем с 1, чтобы проигнорировать 0
    {
    if (items[id] > 0) 
      {
      // Убрано упоминание удаленного массива inventory_name
      printf("Предмет ID %d: %d штук\n", id, items[id]);
      unique_items = unique_items + 1;
      }
    }
    printf("Всего уникальных предметов: %d\n", unique_items);
    if (unique_items == 0) 
      {
      printf("Инвнтарь пуст.\n");
      }
}

void item_catalog() {
    //Пытаемся открыть файл 
    FILE *file = fopen("items.txt", "r");
    
    //стандартные названия
    char default_names[MAX_ITEMS][MAX_NAME_LEN] = {
        "Пусто", "Дерево", "Камень", "Семена", "Картошка", 
        "Кирка", "Пшено", "Грибы", "Седло", "Морковь"
    };

    //если файл не найден
    if (file == NULL) {
        printf("\n[Предупреждение]: Файл items.txt не найден! Загружены названия по умолчанию.\n");
        for (int i = 0; i < MAX_ITEMS; i++) {
            strcpy(item_names[i], default_names[i]); // копируем строки из дефолтных
        }
        return; // выходим из функции
    }

    // Если файл открылся
    int id;
    char name[MAX_NAME_LEN];
    int count = 0;

    //пока fscanf возвращает 2
    while (fscanf(file, "%d %49s", &id, name) == 2) {
        // Проверяем, что ID поместится
        if (id >= 0 && id < MAX_ITEMS) {
            strcpy(item_names[id], name); //Копируем
            count++;
        }
    }

    // Закрываем
    fclose(file);
    printf("Каталог предметов успешно загружен(%d предметов).\n", count);
}

int main() // главаная функция
{
  item_catalog();
  while (1) 
  {
      printf("Введите имя вашего персонажа: ");
      
      fgets(player_name, player_name_max, stdin);
      
      player_name[strcspn(player_name, "\n")] = '\0';

      if (strlen(player_name) > 0) 
      {
          printf("Имя установлено: %s.\n\n", player_name);
          break;
      }
    
      printf("Вы должны дать себе имя, чтобы продолжить...\n");
  }

  while (1) 
    {
    printf("Привет, %s ! \n", player_name);
    printf("Выбери пункт:\n");
    printf("[0] Выход\n");
    printf("[1] Посмотреть на часы\n");
    printf("[2] Промотать время (Поработать)\n");
    printf("[3] Посмотреть инвентарь\n");
    printf("[4] Положить предмет в слот\n");
    printf("[5] Выбросить предмет\n");
    printf("[6] Уникальные находки\n");
    
    if (scanf("%d", &userAction) != 1) 
      {
      printf("Ошибка ввода! Введите число.\n");
      while (getchar() != '\n'); // чистка буфера
      continue;
      }
    


    switch (userAction) // действия в зависимости от выбора
    {
    
    case 0: // завершение работы проги
      {
      printf("Пока-пока!\n");
      return 0;
      break;
      }
    
    case 1: // вывести время
      {
      case1_time(); // Вызываем функцию вместо кучи кода
      break;
      }
    
    case 2: // поработать
      {
      case2_work(); // Вызываем функцию вместо кучи кода
      break;
      }
    
    case 3: 
      {
      case3_show_inventory();
      break;
      }
    
    case 4: 
      {
      case4_put_item();
      break;
      }
      
    case 5:
      {
      case5_drop_item();
      break;
      }

    case 6:
      {
      case6_unique();
      break;
      }

    default: // если ввод неправильный
    {
      printf("Неверный ввод\n");
      break;
    }
    }
  }
}