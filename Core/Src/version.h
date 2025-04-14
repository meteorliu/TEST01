/**
__LINE__ 当前语句所在的行号, 以10进制整数标注.
__FILE__ 当前源文件的文件名, 以字符串常量标注.
__DATE__ 程序被编译的日期, 以"Mmm dd yyyy"格式的字符串标注.
__TIME__ 程序被编译的时间, 以"hh:mm:ss"格式的字符串标注, 该时间由asctime返回.
 */

#define YEAR ((((__DATE__ [7] - '0') * 10 + (__DATE__ [8] - '0')) * 10 \
    + (__DATE__ [9] - '0')) * 10 + (__DATE__ [10] - '0'))

/**月份简写
January (Jan)---Febuary (Feb)---March (Mar)---April (Apr)---May (May)---June (Jun)--
July (Jul)---August (Aug)---September (Sep)---October (Oct)---November (Nov)---SDecember (Dec)--
*/

#define MONTH (__DATE__ [2] == 'n'  ? (__DATE__ [1] == 'u' ? 6 : 1)\
    : __DATE__ [2] == 'b' ? 2 \
    : __DATE__ [2] == 'r' ? (__DATE__ [0] == 'M' ? 3 : 4) \
    : __DATE__ [2] == 'y' ? 5 \
    : __DATE__ [2] == 'l' ? 7 \
    : __DATE__ [2] == 'g' ? 8 \
    : __DATE__ [2] == 'p' ? 9 \
    : __DATE__ [2] == 't' ? 10 \
    : __DATE__ [2] == 'v' ? 11 : 12)

#define DAY ((__DATE__ [4] == ' ' ? 0 : __DATE__ [4] - '0') * 10 \
    + (__DATE__ [5] - '0'))
		
#define TIME (( __TIME__[0] - '0')*1000 + ( __TIME__[1] - '0')*100 + ( __TIME__[3] - '0')*10 + ( __TIME__[4] - '0'))

//__DATE__转换为int型，eg：170502(20省去)
#define DATE_AS_INT ((YEAR-2000) * 100000000 + (MONTH) * 1000000 + (DAY)*10000  + TIME)

//固件版本号
#define FW_DATE     DATE_AS_INT
