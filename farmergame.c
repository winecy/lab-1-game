// библиотеки---------------------------------------------------------------

#include <stdio.h>  
#include <string.h>


// константы----------------------------------------------------------------

#define inventory_size 10  // размер инвенторя
#define player_name_max 32 // максимальная длина имени фермера(юзера)
#define MAX_ITEMS 10       // максимальное количество предметов
#define MAX_NAME_LEN 50    // максимальная длина названия предмета


// переменные---------------------------------------------------------------

int current_day = 1;    // стартовое значение дней
int current_hour = 8;   // стартовое значение часов
int userAction;         // выбор в меню
int work_time;          // длительность работы
int item_slot;          // слот предмета
int item_id;            // айди предмета

char player_name[player_name_max];         // имя
char item_names[MAX_ITEMS][MAX_NAME_LEN];  // названия предметов


// массивы------------------------------------------------------------------

int inventory[inventory_size] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};     // инвентарь 



// функции(меню)--------------------------------------------------------------

void case1_time()    // вывод времени
{
  printf("Текущее время: %d день %d час\n", current_day, current_hour);
}


void case2_work()    // работа
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
    printf("Такое невозможно. Введите корректное количество часов.\n");
  }
}


void case3_show_inventory()    // вывод инвентаря
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
        printf("Слот %d: [%d] - неизвестный предмет.\n", i, id);
    }
  }
}


void case4_put_item()    // положить предмет 
{
  printf("Выберите ячейку инвентаря:\n");
  
  if (scanf("%d", &item_slot) != 1) 
    {
    printf("Ошибка ввода! Введите число от 0 до 9.\n");
    while (getchar() != '\n'); // чистка буфера
    return; 
    }
  
  if (item_slot > 9 || item_slot < 0) 
    {
    printf("Неверный номер слота! Введите значение от 0 до 9\n");
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


void case5_drop_item()    // выбросить предмет 
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


void case6_unique()    // уникальные находки 
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


void case7_search_item()     // поиск предмета в инвентаре по названию     
{
  char search_name[MAX_NAME_LEN];
  printf("Введите название предмета: ");
  while (getchar() != '\n'); // Очистка буфера перед вводом строки
  fgets(search_name, MAX_NAME_LEN, stdin);
  search_name[strcspn(search_name, "\n")] = '\0'; // Удаляем перенос строки

  int target_id = -1;
  for (int i = 0; i < MAX_ITEMS; i++) 
  {
    if (strcmp(item_names[i], search_name) == 0) 
    {
      target_id = i;
      break;
    }
  }

  if (target_id == -1) 
  {
    printf("Предмет \"%s\" не найден.\n", search_name);
    return;
  }

  // проверяем наличие найденного ID в рюкзаке
  int found = 0;
  printf("Предмет \"%s\" (ID: %d) найден в слотах: ", search_name, target_id);
  for (int i = 0; i < inventory_size; i++) 
  {
    if (inventory[i] == target_id) 
    {
      printf("%d ", i);
      found = 1;
    }
  }

  if (!found) 
  {
    printf("В рюкзаке этого нет.\n");
  } else {
    printf("\n");
  }
}

void case8_save_diary() 
{
  FILE *file = fopen("diary.txt", "a");
  if (file == NULL) 
  {
    printf("Ошибка: не удалось открыть или создать файл diary.txt!\n");
    return;
  }

  fprintf(file, "=== Запись в дневнике ===\n");
  fprintf(file, "Фермер: %s\n", player_name);
  fprintf(file, "Время: День %d, Час %d:00\n", current_day, current_hour);
  fprintf(file, "Содержимое инвентаря:\n");
  
  for (int i = 0; i < inventory_size; i++) 
  {
    int id = inventory[i];
    if (id >= 0 && id < MAX_ITEMS) 
    {
      fprintf(file, "  Слот %d: %s\n", i, item_names[id]);
    } else {
      fprintf(file, "  Слот %d: Неизвестный предмет\n", i);
    }
  }
  fprintf(file, "-------------------------\n\n");

  fclose(file);
  printf("Состояние успешно сохранено в дневник (diary.txt).\n");
}


void item_catalog() {
    // пытаемся открыть файл 
    FILE *file = fopen("items.txt", "r");
    
    // стандартные названия
    char default_names[MAX_ITEMS][MAX_NAME_LEN] = {
        "Пусто", "Дерево", "Камень", "Семена", "Картошка", 
        "Кирка", "Пшено", "Грибы", "Седло", "Морковь"
    };

    // если файл не найден
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

// главаная функция-----------------------------------------------------------------

int main() 
{
  item_catalog();
  

  while (1) // ввод имени
  {
      printf("Введите имя: ");
      
      fgets(player_name, player_name_max, stdin);
      
      player_name[strcspn(player_name, "\n")] = '\0';

      if (strlen(player_name) > 0) 
      {
          printf("Имя установлено: %s.\n\n", player_name);
          break;
      }
    
      printf("Вы должны дать себе имя, чтобы продолжить...\n");
  }


  while (1)    // меню
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
    
    case 0:   // завершение работы проги
      {
      printf("Пока-пока!\n");
      return 0;
      break;
      }
    
    case 1:   // вывести время
      {
      case1_time(); 
      break;
      }
    
    case 2:    // поработать
      {
      case2_work(); 
      break;
      }
    
    case 3:    // вывод инвентаря
      {
      case3_show_inventory();
      break;
      }
    
    case 4:    // положить предмет
      {
      case4_put_item();
      break;
      }
      
    case 5:    // выбросить предмет
      {
      case5_drop_item();
      break;
      }

    case 6:    // уникальные находки
      {
      case6_unique();
      break;
      }

      case 7:
      {
      case7_search_item();
      break;
      }

      case 8:
      {
      case8_save_diary();
      break;
      }

    default:    // если ввод неправильный
    {
      printf("Неверный ввод\n");
      break;
    }
    }
  }
}