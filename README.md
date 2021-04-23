## 前言

这个工程是基于QT的开源下载BlueNRG-1/2/3 的工程， 目前完成基本的下载功能。依赖于ST官方提供的Flasher utility下载工具的命令行工具**BlueNRG-X_Flasher_Launcher.exe**。故使用需要先安装Flasher utility工具。 暂时只支持window下载。如果有其他下载方式需求（如脱机下载，Linux下载）或者其他问题请联系lucien.kuang@st.com

## 命令行测试:

1. 擦除整片
./BlueNRG-X_Flasher_Launcher.exe mass_erase -SWD -d "0720000105d8ff313438504d43066627a5a5a5a597969908"

2. 写Flash
./BlueNRG-X_Flasher_Launcher.exe flash -SWD -d "0720000105d8ff313438504d43066627a5a5a5a597969908" -mac -mac_address "0x1007d000" -mac_start "0x4411CA000000" -f "${PATH}/BLE_SerialPort_Server.hex"

3. 写Flash 带mac
./BlueNRG-X_Flasher_Launcher.exe flash -erase -SWD -d "0720000105d8ff313438504d43066627a5a5a5a597969908" -mac -mac_address "0x1007d000" -mac_start "0x4411CA000000" -f "${PATH}/BLE_SerialPort_Server.hex"

使用准备:
1. 准备好一块BlueNRG-LP 的开发板或者是其他CMSIS-DAP 下载器，或者是STlink-V2。
2. 连接待下载的VCC，GND,SW接口， Reset，boot（PA10）,上电复位或者手工复位时，让PA10电平为高电平（或者整个下载过程一直让PA10为高电平）

## 使用方法：

1. 下载安装最新的Flasher utility工具------->[STSW-BNRGFLASHER](https://www.st.com/content/st_com/en/products/embedded-software/wireless-connectivity-software/stsw-bnrgflasher.html)
2. 将绿色发布的UnofficialRelease 压缩包解压到安装目录下:
   C:\Users\user name\ST\BlueNRG-X Flasher Utility 4.1.0\Application
3. 双击“C:\Users\user name\ST\BlueNRG-X Flasher Utility 4.1.0\Application\UnofficialReleasexxx\FlasherBlueNRG-x.exe”





## 目前支持功能：

1. 并行4路(软件层支持8路，UI界面只开放了4路)下载 （下载器支持CMSIS-DAP 或者STlink-V2）
2. 下载MAC地址
3. 扫码枪扫码并行下载







