/*
接线说明:无

程序说明:通过esp32s3建立一个简单的网页服务器,在页面上显示引脚电平状态,输出GPIO0的电平(不按下的时候电平为1,按下电平为0),
当未按下BOOT时,两颗LED灯会亮,而按下BOOT键后两颗LED灯会熄灭,同时该程序会自动刷新,每隔3秒刷新一次检查引脚电平(通过HTML语言实现)


注意事项:无

函数示例:无

作者:灵首

时间:2023_3_10

*/
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WebServer.h>

//对开发板上的灯进行宏定义
#define LED_A 10
#define LED_B 11
#define pin_scanned 0    //GPIO0是接的BOOT按键


int pinStateValue;//用来存储从GPIO0引脚读取的电平状态

WebServer esp32s3_webServe(80);//实例化一个网页服务的对象
WiFiMulti wifi_multi;  //建立WiFiMulti 的对象,对象名称是 wifi_multi


/*
# brief 连接WiFi的函数
# param 无
# retval 无
*/
void wifi_multi_con(void){
  int i=0;
  while(wifi_multi.run() != WL_CONNECTED){
    delay(1000);
    i++;
    Serial.print(i);
  }
}



/*
# brief 写入自己要连接的WiFi名称及密码,之后会自动连接信号最强的WiFi
# param 无
# retval  无
*/
void wifi_multi_init(void){
  wifi_multi.addAP("LINGSOU123","12345678");
  wifi_multi.addAP("LINGSOU12","12345678");
  wifi_multi.addAP("LINGSOU1","12345678");
  wifi_multi.addAP("LINGSOU234","12345678");   //通过 wifi_multi.addAP() 添加了多个WiFi的信息,当连接时会在这些WiFi中自动搜索最强信号的WiFi连接
}

/*
# brief 用于handleRoot()函数做参数,主要功能是提供网页自动刷新功能(通过编写HTML代码来实现)
# param  value :int类型的值,需要该参数来提供引脚状态(只有0-1两种情况)
# retval String 字符串,返回一段HTML代码
*/
String sendHTML(int value){

  //以下的函数是由HTML代码构成的,通过此来实现浏览器的定时刷新,同时控制页面的文字显示(文字内容,字体,大小等)
  //可以把这些代码放在其他软件编辑器中,会有更好的编辑体验
  String htmlCode = "<!DOCTYPE html> <html>\n";
  htmlCode +="<head><meta http-equiv='refresh' content='3'/>\n";    //content='3' 数字3是控制浏览器刷新的时间,单位是秒
  htmlCode +="<title>ESP32-S3 Butoon State</title>\n";    //显示标题
  htmlCode +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  htmlCode +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  htmlCode +="</style>\n";
  htmlCode +="</head>\n";
  htmlCode +="<body>\n";
  htmlCode +="<h1>ESP32S3 BUTTON STATE</h1>\n";   //显示正文(应该算是吧)内容
  
  //通过if-else的判断来控制显示内容,同时也控制LED灯的亮灭
  if(pinStateValue)
    {htmlCode +="<p>Button Status: HIGH</p>\n";
    digitalWrite(LED_A,1);
    digitalWrite(LED_B,1);
    }
  else
    {htmlCode +="<p>Button Status: LOW</p>\n";
    digitalWrite(LED_A,0);
    digitalWrite(LED_B,0);
    }
    
  htmlCode +="</body>\n";
  htmlCode +="</html>\n";
  
  return htmlCode;    //返回一段字符串(就是一段HTML代码)
}



/*
# brief 处理网站根目录"/"的访问请求,并会提供一段HTML代码以供其执行
# param 无
# retval  无
*/
void handleRoot() {   //处理网站目录“/”的访问请求 
  esp32s3_webServe.send(200, "text/html", sendHTML(pinStateValue));  
}
 


/*
# brief 错误处理,当访问出错时会出现的页面情况
# param 无
# retval  无
*/
void handleNotFound(){
  esp32s3_webServe.send(404,"text/plain","404 NotFound");//404:状态码(404是不正常的),"text/plain":发送的内容格式,"404 NotFound":发送的内容
}


/*
# brief esp32s3建立网页服务初始化函数,提供一个网页根目录处理函数和一个错误处理函数
# param 无
# retval  无
*/
void esp32s3_webServe_init(void){
  esp32s3_webServe.begin();
  esp32s3_webServe.on("/",handleRoot);
  esp32s3_webServe.onNotFound(handleNotFound);
  Serial.print("\n HTTp esp32s3_webServe started");
}


void setup() {
  Serial.begin(115200);

  pinMode(LED_A,OUTPUT);
  pinMode(LED_B,OUTPUT);//设置灯的引脚的工作状态

  wifi_multi_init();//储存多个WiFi
  wifi_multi_con();//自动连接WiFi

  Serial.print("\nconnect wifi:");
  Serial.print(WiFi.SSID());
  Serial.print("\nIP address:");
  Serial.print(WiFi.localIP());//输出连接信息(连接的WIFI名称及开发板的IP地址)

  esp32s3_webServe_init();//初始化网页服务,开启服务器工作

}

void loop() {
  esp32s3_webServe.handleClient();//循环运行,使能运行处理HTTP访问请求,尽量不要在其中加上延时函数,避免服务器不工作
  pinStateValue = digitalRead(pin_scanned);//读取GPIO0(即BOOT)的引脚状态

}
