import processing.serial.*;

int lf = 10;
Serial myPort;  // The serial port
String input_data;

String newData = "-\r\n";


int col = 0;


int data[][] = {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}};
String string_data[];

void setup() {
  // List all the available serial ports
  printArray(Serial.list());
  // Open the port you are using at the rate you want:
  myPort = new Serial(this, "/dev/ttyUSB0", 115200);
  
  size(400,400);
  background(0);
  println("setup finished");
}

void draw() {  
  
  
  if (myPort.available() > 0) {
    input_data = myPort.readStringUntil(lf);
    
    if(input_data != null){
      //println(input_data);
      if(input_data.equals(newData))
      {
        //println("new data arrived: draw");
        col = 0;
        
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
        
        
      }
      else{
        
        string_data = input_data.split(",");
        //StringBuffer sb = new StringBuffer(string_data[3]);
        
        //sb.deleteCharAt(sb.length()-2);
        //string_data[3] = sb.toString();
        print(string_data[0]); print(" ");
        print(string_data[1]); print(" ");
        print(string_data[2]); print(" ");
        print(string_data[3]); print(" ");
        data[col][0] = parseInt(string_data[0]);
        data[col][1] = parseInt(string_data[1]);
        data[col][2] = parseInt(string_data[2]);
        data[col][3] = parseInt(string_data[3]);
       
        col++;
      }
      
      
      
      
    }
  }
}
