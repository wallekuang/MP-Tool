#ifndef APP_CONST_CFG_H
#define APP_CONST_CFG_H

#include <QString>


#define CMD_FLASH           ("flash")
#define CMD_READ            ("read")
#define CMD_MASS_ERASE      ("mass_erase")
#define CMD_VERIFY_MEMORY   ("verify_memory")
#define CMD_ERASE_PAGES     ("erase_pages")
#define CMD_UART            ("uart")
#define CMD_SWD             ("swd")

// 默认mac 地址存放位置
#define DEF_MAC_LOCATION      (0x1007E000)
#define _MEMORY_FLASH_BEGIN_  (0x10040000)
#define _MEMORY_FLASH_END_    (0x1007FFFF)

#define DEF_STARTED_PROGRESS   (10)

#endif // APP_CONST_CFG_H
