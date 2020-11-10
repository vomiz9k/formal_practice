# Практикум по курсу "Формальные языки и трансляции". Вариант 16.
## Задача: 
### Дано регулярное выражение и слово. Найти максимальную длину подслова в этом слове, такого, что оно лежит в языке, задаваемом регулярным выражением.
## Алгоритм:
### По регулярному выражению строим автомат. Далее, с помощью DFS, для каждого суффикса данного слова находим максимальную длину префикса, лежащего в языке. Максимум по всем суффиксам - ответ на задачу.

