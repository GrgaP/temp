#include "main.h"
#include "uart_data_test.h"
#include "data_processing.h"
#include "spectra_data.h"

#define NUM_OF_GRAPHS 7 //количество графиков
#define GRAPHS_ADDR_OFFSET GRAPH_WIDTH*sizeof(int16_t)

#if NUM_OF_GRAPHS > 10
#error Won't work on Vaxen.
#endif

/*
 * Определим конфигурацию графиков:
 * N1 - отображение real_time данных со 1-го канала
 * N2 - отображение real_time данных со 1-го канала
 * N3..5 - отображение 3-х последних графиков с превышением порогового значения
 */

#define GPHS_MEMORY_NUM 11

#define RESET_SINGLE_GRAPH(NUM) 0x01<<NUM
#define RESET_ALL_GRAPH 0x03ff

#define GUI_YELLOW1 0x00e6e6
#define GUI_YELLOW2 0x00b3b3
#define GUI_YELLOW3 0x00807f
#define GUI_YELLOW4 0x004d4d
#define GUI_YELLOW5 0x001a1a

extern xQueueHandle uart_data_ptr_queue;//ипортируем очердь из модуля UART
xQueueHandle graph_data_ptr_queue;//определяем очередь из указателей для обновления графика

osSemaphoreId semSerialDataUpdComplete;//семафор сигнализирует о готовности новых данных для графика


static const GUI_COLOR _aColor[10] = {GUI_BLUE,GUI_YELLOW,GUI_MAGENTA,GUI_YELLOW5,GUI_YELLOW3,GUI_YELLOW2,GUI_BLUE,GUI_BLUE,GUI_BLUE,GUI_BLUE};

//структура содержит информацию о графике - цел. числ индекс: 0...NUM_OF_GRAPHS, цвет графика
typedef struct SVD_DP_gph_cfg{
  uint8_t index;
  GUI_COLOR color;
}SVD_DP_gph_cfgTypeDef;


//структура хранит в очереди стартовый адрес буфера графиков и массив номеров активных графиков
typedef struct SVD_DP_GMcfg{
  int16_t* start_addr;//начальный адрес буфера
  //
  SVD_DP_gph_cfgTypeDef gph_indexes[NUM_OF_GRAPHS];//массив с индексамми активных графиков
} SVD_DP_GMcfgTypeDef;


osThreadId SVD_DP_ThreadId;
osThreadId SVD_DP_GM_ThreadId; 


//структура для данных предназначенных для отображения графика
  //указатель на массив;
  //индекс наибольшего значения 
  //индекс нименьшего значения  
  //индекс 


static void SVD_DP_Thread(void const *argument);
//static void SVD_DP_GM_Thread(void const *argument);
static void SVD_DP_pack_data(int16_t* inData, uint16_t* outData, SVD_DP_GMcfgTypeDef* graph_cfg, uint8_t rec_sw);

/*
 * Инициализация
 */
void svd_dp_module_init(void)
{
  //int16_t* tmp_ptr; 
  uint8_t i;
  
  SVD_DP_GMcfgTypeDef* graph_cfg;//заводим указатель на структуру состояния графиков
   
  graph_cfg = (SVD_DP_GMcfgTypeDef*) pvPortMalloc(sizeof(SVD_DP_GMcfgTypeDef));//выделяем память для структуры состояния графиков
  
  if(graph_cfg == NULL)
     asm("NOP");//добавить вызов обработчика ошибок - память под структуру не выделена
  
  graph_cfg -> start_addr = (int16_t*) pvPortMalloc(GPHS_MEMORY_NUM*GRAPH_WIDTH*sizeof(int16_t));//выделяем память на буфер для даных 10 графиков, 480 байт на график (во FreeRTOS возникает ошибка если количество буферов четное)
  
  for(i=0; i < NUM_OF_GRAPHS; i++)
  { 
    graph_cfg ->gph_indexes[i].index = i;
    graph_cfg ->gph_indexes[i].color = _aColor[i];
  };
  
  semSerialDataUpdComplete = xSemaphoreCreateBinary();//определяем семафор сигнализирующей об обноалении конфигурации графиков
  xSemaphoreGive(semSerialDataUpdComplete);//выставляем семафор о изменении данных графика

  //graph_cfg ->gph_indexes[9] = _aColor[9];
  
  if(graph_cfg -> start_addr == NULL)
     asm("NOP");//добавить вызов обработчика ошибок - память под  не выделена
  
  graph_data_ptr_queue = xQueueCreate(1, sizeof(SVD_DP_GMcfgTypeDef*));//инициируем очередь для хранения указателя на данныме конфигурации для отображения графика(ЭТА ОЧЕРЕДЬ НИКОГДА НЕ ДОЛЖНА БЫТЬ ПУСТА!)

  
  
  if(graph_data_ptr_queue == NULL)
     asm("NOP");//добавить вызов обработчика ошибок - очередь не создана
   else
   {     
     xQueueSend(graph_data_ptr_queue,&graph_cfg,osWaitForever);//регистрируем в очереди структуру
   };
  
  //создаем задачу подготовки данных для графика
  osThreadDef(SVD_DP, SVD_DP_Thread, osPriorityLow, 0, 1280);
  SVD_DP_ThreadId = osThreadCreate (osThread(SVD_DP), NULL);
  
  //создаем задачу которая отслеживает изменение конфигурации графиков
  //osThreadDef(SVD_DP_GM, SVD_DP_GM_Thread, osPriorityNormal, 0, 128);
  //SVD_DP_GM_ThreadId = osThreadCreate (osThread(SVD_DP_GM), NULL);
};

/*
 * Основная задача отвечат за подготовку даных для графика и их обновление и выовод 2-x RT графиков
 * 
 */
static void SVD_DP_Thread(void const *argument)
 {
   SVD_DP_GMcfgTypeDef* graph_cfg;
   int16_t* tmp_ptr_in;
   int16_t* tmp_ptr_out;

   for(;;){     
     //тут надо смотреть ID текущего режима экрана, и если это не график то ничего не делать
     
     //проверяем наличие значений элементов массива с индексами графика, в очереди, если ничего нет - задача не выполняется
     xQueuePeek(graph_data_ptr_queue,&graph_cfg, osWaitForever);//посмотрели на указатель для графика
     tmp_ptr_out = (int16_t*) ((graph_cfg -> start_addr)+ (graph_cfg -> gph_indexes[0].index)* GRAPHS_ADDR_OFFSET);//посчитаем адрес нового 0го элемента
     
     xQueueReceive(uart_data_ptr_queue,&tmp_ptr_in,osWaitForever);//получили указатель на новые данные из модуля UART - debug
     //отдаем указатель на поиск масимумов в текущей реализации
     SVD_DP_pack_data(tmp_ptr_in, (uint16_t*) tmp_ptr_out, graph_cfg, 0);//отдаем указатель на новые данные в функцию сжатия и пишем результат в адрес соответствующий индексу 0го элемента;
     //Graph_screen_reset_sel_data(RESET_SINGLE_GRAPH(0));
     Graph_screen_add_data_sel((graph_cfg->start_addr)+ GRAPHS_ADDR_OFFSET*(graph_cfg ->gph_indexes[0].index), (graph_cfg ->gph_indexes[0].color), 9);
     
     tmp_ptr_out = (int16_t*) ((graph_cfg -> start_addr)+ (graph_cfg -> gph_indexes[1].index)* GRAPHS_ADDR_OFFSET);//посчитаем адрес нового 1го элемента
     
     xQueueReceive(uart_data_ptr_queue,&tmp_ptr_in,osWaitForever);//получили указатель на новые данные из модуля UART - debug
     
     SVD_DP_pack_data(tmp_ptr_in, (uint16_t*) tmp_ptr_out, graph_cfg , 1);//отдаем указатель на новые данные в функцию сжатия и пишем результат в адрес соответствующий индексу 0го элемента;
     //Graph_screen_reset_sel_data(RESET_SINGLE_GRAPH(8));
     Graph_screen_add_data_sel((graph_cfg->start_addr)+ GRAPHS_ADDR_OFFSET*(graph_cfg ->gph_indexes[1].index), (graph_cfg ->gph_indexes[1].color), 8);
     
     //HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_SET);
     
     osDelay(10/portTICK_RATE_MS);//частота вызова задачи не должна быть реже частоты обновления данных в модуле UART_DATA_TEST(сойдет раза в два чаще), позже припишу дефайн.
   };
 }

/*
 * Функция преобразует данные принятые по UART в формат отображения графика
 * @param inData - указатель на буфер источник
 * @param outData - указатель на буфер получатель
 * @param rec_sw - если 1 - ведется дублирование выходных данных в буфер со смещением outData+GRAPHS_ADDR_OFFSET
 */
static void SVD_DP_pack_data(int16_t* inData, uint16_t* outData, SVD_DP_GMcfgTypeDef* graph_cfg, uint8_t rec_sw)
{
  uint16_t i,j,i_1,i_2;
  int32_t tmp = 0;
  int16_t* outData2;//текущий буфер дублирования 
  int8_t tmp_index;
  int8_t xcs_flag = 0x00;
  
  if(rec_sw)
  {tmp_index = (graph_cfg -> gph_indexes[2].index);//временно запомнили значение 2го элемента
  outData2 = (int16_t*) (graph_cfg -> start_addr)+tmp_index*GRAPHS_ADDR_OFFSET;};//временно запомнили конфигурацию графика
  
  for(i=0; i< GRAPH_WIDTH; i++)
  {
    i_1 = (uint16_t) (i*((float) GRAPH_DATA_SIZE/GRAPH_WIDTH));
    i_2 = (uint16_t) ((i+1)*((float) GRAPH_DATA_SIZE/GRAPH_WIDTH));
    for(j=i_1; j<i_2; j++){tmp += inData[j];};
    tmp/= (i_2-i_1);
    tmp = (int32_t) -((tmp-32768)/((float)65535/((GRAPH_HIGH)-1)));
    outData[GRAPH_WIDTH-i] = (uint16_t) tmp;
    
    if(rec_sw)
    {outData2[GRAPH_WIDTH-i] = outData[GRAPH_WIDTH-i];//пишем данные в оба буфера
    if(tmp > 275) //если попалось значение превышающее заданный порог
      xcs_flag = 0x01;     };
  };
   if(xcs_flag&&rec_sw)//если в течении текущей записи было выявлено превышение порога, меняем указатель текущего буфера на след., а указатель предыдущего буфера надо передать на отображение
   { 
     
     graph_cfg -> gph_indexes[2].index = graph_cfg -> gph_indexes[3].index;
     graph_cfg -> gph_indexes[3].index = graph_cfg -> gph_indexes[4].index;
     graph_cfg -> gph_indexes[4].index = graph_cfg -> gph_indexes[5].index;
     graph_cfg -> gph_indexes[5].index = tmp_index;
       
     Graph_screen_add_data_sel((graph_cfg->start_addr)+ GRAPHS_ADDR_OFFSET*(graph_cfg ->gph_indexes[5].index), (graph_cfg ->gph_indexes[5].color), 2);
     Graph_screen_add_data_sel((graph_cfg->start_addr)+ GRAPHS_ADDR_OFFSET*(graph_cfg ->gph_indexes[4].index), (graph_cfg ->gph_indexes[4].color), 1);
     Graph_screen_add_data_sel((graph_cfg->start_addr)+ GRAPHS_ADDR_OFFSET*(graph_cfg ->gph_indexes[3].index), (graph_cfg ->gph_indexes[3].color), 0);
   }; 
 
}