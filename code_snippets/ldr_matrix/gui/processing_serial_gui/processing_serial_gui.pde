import processing.serial.*;

int lf = 10;
Serial myPort;  // The serial port
String input_data;

String newDataString = "-\r\n";


int col = 0;
int newData = 0;

int data[][] = {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}};
String string_data[];

void setup() {
  myPort = new Serial(this, "/dev/ttyUSB0", 115200);
  
  
  thread("processSerialData");
  
  size(400,400);
  background(0);
  println("setup finished");
}




void draw() {  

  if(newData != 0){
    //println("new data");
    
    for(int i = 0; i < 4; i++)
    {
      for(int j = 0; j < 4; j++)
      {
        if(data[j][i] < 4000)
        {
          
          fill(10);
        }else{
          fill(255);
        }
        rect(100 * i, 100 * j, 100, 100);
        
      }
    }
    
    newData = 0;
  }
  
}


void processSerialData()
{
  println("starting processingSerialData Thread");
  // List all the available serial ports
  // printArray(Serial.list());
  // Open the port you are using at the rate you want:
  
  while(true)
  {
    
    if (myPort.available() > 0) {
      //println("new serial data");
      while(myPort.available() > 0){
        
        input_data = myPort.readStringUntil(lf);
        
        if(input_data != null){
          //println(input_data);
          if(input_data.equals(newDataString))
          {
            //println("new data arrived: draw");
            col = 0;
            newData = 1;
            
            
            
          }
          else{
            
            string_data = input_data.split(",");
            //StringBuffer sb = new StringBuffer(string_data[3]);
            
            //sb.deleteCharAt(sb.length()-2);
            //string_data[3] = sb.toString();
            int t = millis();
            
            print(t); print(": "); 
            print(string_data[0]); print(" ");
            print(string_data[1]); print(" ");
            print(string_data[2]); print(" ");
            print(string_data[3]); println(" ");
            data[col][0] = parseInt(string_data[0]);
            data[col][1] = parseInt(string_data[1]);
            data[col][2] = parseInt(string_data[2]);
            data[col][3] = parseInt(string_data[3]);
           
            col++;
          }
        }
        
        
        
      }
    }
    delay(1);
  }
  
}
