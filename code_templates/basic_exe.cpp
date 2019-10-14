// Формируем заголовки
<common includes>
<input includes>
<detector incluses>
<tracker incluses>
<filter includes>
<output includes>
// Точка входа
void main()
{
// --------------------
// Объявляем компоненты
// --------------------
// Вход (видео,точки,...)
BaseInput* input=nullptr;
// Детектор (поиск объектов)
BaseDetector* detector=nullptr;
// Трекер (сопровождение объектов)
BaseTracker* tracker=nullptr;
// Фильтр (сглаживание траекторий)
BaseFilter* filter=nullptr;
// Выход (Файл,изображения,точки,...)
BaseOutput* output=nullptr;
// --------------------
// Создаем по экземпляру каждого
// --------------------
input=new <input type>
detector=new <detector type>()
tracker=new <tracker type>
filter=new <filter type>
output=new <output type>
// --------------------
// Конфигурируем "Матрешку"
// --------------------
output(filter(tracker(detector(input))));
// --------------------
// Запускаем поток
// --------------------
output.run(/* может быть сюда break callback */);
// --------------------
// Чистим все
// --------------------
delete input;
delete detector;
delete tracker;
delete filter;
delete output;

}