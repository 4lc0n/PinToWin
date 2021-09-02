import matplotlib.pyplot as plt


max_duty = 0.25
duty = 0
output = 0
button = 0
prev_button = 0



interval = 0.025
fir_length = 5 / interval

output_list = []
duty_list = []
t = []
input_list = []

for i in range(int(50 / interval)):
    t.append(i * interval)
    if i == 0:
        button = output = 0
    if i == 7 / interval:
        button = output = 1
    elif i == 10 / interval:
        button = output = 0
    elif i == 12 / interval:
        button = output = 1
    elif i == int(12.2 / interval):
        button = output = 0
    elif i == int(12.3 / interval):
        button = output = 1
    elif i == int(12.4 / interval):
        button = output = 0
    elif i == int(12.5 / interval):
        button = output = 1
    elif i == int(12.6 / interval):
        button = output = 0
    elif i == int(12.65 / interval):
        button = output = 1
    elif i == int(12.85 / interval):
        button = output = 0
    elif i == int(12.9 / interval):
        button = output = 1
    elif i == int(13.1 / interval):
        button = output = 0
    elif i == int(13.3 / interval):
        button = output = 1
    elif i == 14 / interval:
        button = output = 0
    elif i == 16 / interval:
        button = output = 1
    elif i == 20 / interval:
        button = output = 0
    elif i == 25 / interval:
        button = output = 1
    elif i == 27 / interval:
        button = output = 0
    elif i == 28 / interval:
        button = output = 1
    elif i == 30 / interval:
        button = output = 0
    elif i == 36 / interval:
        button = output = 1
    
    

    input_list.append(button)
    #duty = duty * (1 - calc_factor) + (output * calc_factor)
    
    duty = 0
    if i > fir_length:
        duty = 0
        for j in range(int(fir_length)):
                duty += output_list[int(i - j -1)] / fir_length

    if button > 0:
        if  prev_button != button:
            pass
             
        elif duty > max_duty and output > max_duty:
            output -= 0.1
        elif duty < max_duty and output < max_duty:
            output = max_duty
            

    
    output_list.append(output)
    duty_list.append(duty)

    prev_button = button



fig, ax = plt.subplots()
ax.plot(t, output_list, label='output power')
ax.plot(t, input_list, '--',label='input signal')
ax.plot(t, duty_list, label='duty-cycle')
ax.set_xlabel('time in sec')

ax.legend()
plt.show()

