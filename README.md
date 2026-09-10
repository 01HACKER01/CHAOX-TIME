# CHAOX-TIME

![BLARE](Images/1.png)

![BLARE](Images/2.png)

![BLARE](Images/3.png)

![BLARE](Images/4.png)

![BLARE](Images/5.png)

![BLARE](Images/6.png)

this is a digital alarm clock 

This is my second hardware project 
I wanted to build it all by myself alone but i dont know how to code so i used AI to make the code 
for now i have used but i will learn code asap and then make it by myself 

## BOM

| Part | Qty | Approx. USD | Link |
Seeed XIAO ESP32-C3	1	$6	Seeed
2.8" ILI9341 240×320 8-pin SPI TFT	1	$4–8	Alibaba – 8-pin version
MCP23017-E/SP DIP-28	1	~$1.69	Mouser
MAX98357A I²S amplifier module	1	~$1–6	Alibaba MAX98357A
8Ω 3W speaker	1	~$1–4	Alibaba 8Ω 3W speakers
3.3V piezo buzzer	1	~$0.50–1.50	Alibaba piezo buzzers
MX/Cherry-MX compatible switches	12	~$3–6	Alibaba MX switches
1N4148 DO-35 diodes	12	~$0.50–1.50	Alibaba 1N4148
4.7kΩ ¼W THT resistors	2	~$0.10–0.30	Alibaba 4.7k resistors
100nF THT capacitors	3	~$0.20–0.60	Alibaba 100nF THT capacitors
10µF radial electrolytic capacitor	1	~$0.10–0.30	Alibaba 10µF capacitors
1×8 2.54mm header	1	~$0.10–0.30	Alibaba 1×8 header
1×7 2.54mm header	1	~$0.10–0.30	Alibaba 1×7 header
1×2 2.54mm header	1	~$0.05–0.15	Alibaba 1×2 header
M3×5×4 heat-set inserts	8	~$1–3	Alibaba M3 heat-set inserts
M3×8 screws	4	~$0.30–1	Alibaba M3×8 screws
M3×16 screws	4	~$0.40–1	Alibaba M3×16 screws
USB-A → USB-C cable	1	~$2–4	Alibaba USB-C cable
5V USB power adapter	1	~$3–6	Alibaba 5V adapter

**Estimated parts cost: ~$22–40 USD**


#Features 

the current verdion has some less features than what i assumed when i started this project cause i forgot i had to code it myself but i dont know how to code so rn it has some less features but i m learning c++ so that i can add all the features i thought of when i started this project 

so now the features it has rn:
- shows the current time 
- it can fetch the time and date from internet using wifi
- it can set only 1 alarm //for now
- it beeps when alarm goes off
- we can stop the alam by 1 button

i have added the hardware in the PCB for future updates 

#Hardware

The main controller is the seeed xiao esp32 c3 board which controlls the whole clock 
itt has 12 switches so that we can change any setting we want 

the pcb contains:

- Seeed XIAO ESP32-C3
- 2.8" ILI9341 SPI TFT
- MCP23017-E/SP GPIO expander
- 12 MX/Cherry-MX compatible switches
- 12 × 1N4148 diodes
- 3.3V piezo buzzer
- MAX98357A I2S audio connection
- I2C pull-up resistors
- Decoupling capacitors
- M3 mounting holes

# Controls

The keypad is arranged as a 3×4 matrix.

The current firmware uses:

- UP - increase the selected value
- DOWN - decrease the selected value
- OK - enter/confirm
- Other keys are available for future functions

To set the alarm:

1. press the OK key from the main clock screen
2. use UP/DOWN to set the alarm hour
3. press OK
4. use UP/DOWN to set the alarm minute
5. press OK again to save and enable the alarm

When the alarm starts, pressing any key stops it


- u all can change the settings and everything by making ur own firmware



# Power

CHAOX-TIME is powered from a 5V USB power adapter.


#Assemble

pcb assembly is quite easy solder eerything on its place and upload he code then screw the pcb to the case and screw the case together 
Done..

# Future Plans

I want to add more features to CHAOX-TIME over time.

Some of the things I want to add are:

- Adhan audio
- Speaker support
- Prayer times
- Multiple alarms
- Countdown timer
- Better menus
- More keypad functions
- Volume control
- More clock settings
- Saving settings
- Improved UI

These features are planned and are not part of the current firmware yet.



#THANK YOU 

#CHAOX-TIME
