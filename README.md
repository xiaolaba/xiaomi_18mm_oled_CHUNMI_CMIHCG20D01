# xiaomi_18mm_oled_CHUNMI_CMIHCG20D01
CHUNMI CMIHCG20D01 QR01, oled SPI procotol



/photo/18mm_oled.jpg  
/photo/disassemble.jpg  
/photo/logic_ana_named.PNG  
/photo/logic_ana_view1.jpg  
/photo/oled_no_show.jpg  
/photo/SPI_probing.jpg  

![/photo/18mm_oled.jpg](/photo/18mm_oled.jpg)  
![/photo/disassemble.jpg](/photo/disassemble.jpg)
![/photo/logic_ana_named.PNG](/photo/logic_ana_named.PNG)
![/photo/logic_ana_view1.jpg](/photo/logic_ana_view1.jpg)
![/photo/oled_no_show.jpg](/photo/oled_no_show.jpg)
![/photo/SPI_probing.jpg](/photo/SPI_probing.jpg)
![]()


### OLED glass
![/photo/E11.jpg](/photo/E11.jpg)
![/photo/replaced_OLED_glass.PNG](/photo/replaced_OLED_glass.PNG)

屏底接5V, 擊活極間電容, 再用 SPI SNIFFER 重啟順序, 然後連結回去主板, 試試看STM32驅動, 結果有點顯示了, 但是主板顯示訊號 "请报修", 簡體字, 這等于垃圾显示😁

### about error code of this mijia Induction Cooker 
LED白色高光报错指示，其他指示灯均为熄灭状态。若通过以下故障代码无法解决问题，可致电4001005678。  

E-01 煮饭过程中开盖超时，进入待机状态  
E-02 空锅（未放入锅具）  
E-03 高压保护265V±7V  
E-04 低压保护165V±7V  
E-05 底部高温报警  
E-06 顶部高温报警  
E-07 IGBT高温报错  
E-08 IGBT传感器损坏（包括短路和断路）  
E-09 顶部传感器损坏（包括短路和断路）  
E-10 底部传感器损坏（包括短路和断路）  
E-11 通信异常  

