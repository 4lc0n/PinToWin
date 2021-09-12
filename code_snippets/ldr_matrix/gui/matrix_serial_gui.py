from tkinter import Tk, Canvas, Frame, BOTH




class Application(Frame):
    def __init__(self): 
        super().__init__()

        self.create_widgets()


    def create_widgets(self):
        self.master.title("rect")
        self.pack(fill=BOTH,expand=1)

        canvas = Canvas(self)
        canvas.create_rectangle(0,0,100,100, outline="#fb0", fill="#fff")

        canvas.pack(fill=BOTH, expand=1)

def main():
    root = Tk()
    app = Application()
    
    serialPort = "/dev/ttyACM0"
    baudRate = 115200
    ser = Serial(serialPort , baudRate, timeout=0, writeTimeout=0) #ensure non-blocking

    ser.open()
    input_data = ""
    row = 0

    while True:

        input_data = ser.readline()
        if input_data == "\033[2J":
            row = 0
        else:
            



    
        root.update()

if __name__ == '__main__':
    main()
