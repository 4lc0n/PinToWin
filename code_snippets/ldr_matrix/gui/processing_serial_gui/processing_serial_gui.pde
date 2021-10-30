import processing.serial.*;

int lf = 10;
Serial myPort;  // The serial port
String input_data;

String newDataString = "-\r\n";


int col = 0;
int newData = 0;

int data[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
String string_data[];

void setup() {
  myPort = new Serial(this, "/dev/ttyACM0", 115200);
  
  
  thread("processSerialData");
  frameRate(25);
  size(1100,800);
  background(0);
  println("setup finished");
}




void draw() {  

  if(newData != 0){
    //println("new data");
    
    for(int i = 0; i < 11; i++)
    {
      for(int j = 0; j < 8; j++)
      {
        /*if(data[j][i] < 4000)
        {
          
          fill(10);
        }else{
          fill(255);
        }*/
        
        
        
        fill((data[i] & (1 << j)) * 255 );
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
  delay(1000);
  
  while(true)
  {
    
      //myPort.write(", ");
      while(myPort.available() < 1);
      
     
        
      input_data = myPort.readStringUntil('\n');
      
      if(input_data != null){
        //println(input_data);
        
          
        string_data = input_data.split(",");
        //StringBuffer sb = new StringBuffer(string_data[3]);
        
        
        
        
        for(int i = 0; i < 11; i++)
        {
          if(string_data[i] != null && string_data[i] != "")
          try{
            
             data[i] = parseInt(string_data[i]);
             print(data[i]); print("\t");
             
             
             
             
             
             
          }
          catch(Exception e){
            print("cast failed\n");
          }
         
          
        }
        
        
        
        /*
        
        print(t); print(": "); 
        print(string_data[0]); print(" ");
        print(string_data[1]); print(" ");
        print(string_data[2]); print(" ");
        print(string_data[3]); println(" ");
        data[col][0] = parseInt(string_data[0]);
        data[col][1] = parseInt(string_data[1]);
        data[col][2] = parseInt(string_data[2]);
        data[col][3] = parseInt(string_data[3]);
       
        col++;*/
        
        newData = 1;
          
        
      }
        
        println("");
        
      
    
    //delay(1);
  }
  
}
