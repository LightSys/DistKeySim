import tkinter as tk
from multiprocessing import Pipe, Process, Event
from time import sleep

import DataVis as datavis 

import sys

import subprocess

import json 


#command line args: 
if len(sys.argv) != 5:
    print("ERROR: include 4 parameters of ip, username, password, and FULL DistKeySim folder location on the server (including the DistKeySim folder)")
    sys.exit(2)

config_json_file_location = "config.json"

times_run = 0

def setJsonVar(str_results_dict,hardknobs_results_dict):
    with open(config_json_file_location) as f:
        data = json.load(f)
    
    for label, float_val in str_results_dict.items():
        data[label.replace(" ","_")] = float_val
    
    for txt, result in hardknobs_results_dict.items():
        data[txt.replace(" ","_")] = result 
    
    with open(config_json_file_location, 'w') as f:
        json.dump(data,f,indent=4)
    
    global times_run
    
    print("finished writing!")
    print("running script for simulation...")
    
    subprocess.call("./run.sh "+str(times_run) + " " + sys.argv[1] + " " + sys.argv[2] + " " + sys.argv[3] + " " + sys.argv[4] + " > simulation_run_"+str(times_run)+".txt",shell=True)
    
    print("finished running simulation script!")
    
    print("opening results data visualization...")
    
    datavis.drawAnalytics("simulation_"+str(times_run)+"/statslog.csv")
    
    times_run += 1
    
    
def getStringFromBox(box):
    return box.get("1.0",'end')

def is_valid_num(str_in):
    try: 
        num = float(str_in)
        return num > 0
    except ValueError: 
        return False 


def submit_button_hit(boxes_dict,hardknobs_dict):
    str_results_dict = { }
    hardknobs_results_dict = { }
    
    #for each pair of label to textbox in items to extract 
    for label, box in boxes_dict.items():
        gotten_str = getStringFromBox(box)
        if not is_valid_num(gotten_str):
            print("ERROR: input \""+gotten_str+"\" is not a number!")
            return
        #is a number verified
        #add to dict 
        str_results_dict.update({label : float(gotten_str)})
    
    for txt, menu in  hardknobs_dict.items():
        hardknobs_results_dict.update({txt : menu.get()})
    
    
    #numbers are verified and strings are retrieved 
    
    setJsonVar(str_results_dict,hardknobs_results_dict)
    
    
    
    

def config_label(label):
    label.configure(font=("Ariel",20),background='gray', foreground='white')

def config_label_desc(label):
    label.configure(font=("Ariel",12),background='gray', foreground='white')




root = None 
original_width = 1100
original_height = 500

#knobs info:

#note: the ":" is added later in the code for the labels

#labels / identifiers for fine knobs 
fine_knob_labels = ["Visible Peers (connected %)",\
                    "Lambda 1 (time offline)",\
                    "Lambda 2 (time online)",\
                    "Max Keys (2^n, give n)",\
                    "lambda 3 (time between creating objects)",\
                    "Chunkiness (# of keys to shift)",\
                    "Heartbeat Frequency"]
                    
                    
#label, [array of options]
coarse_knobs_info = {"Smallest Key for Priority": ["smallest overall","smallest key of the block with the least space left"], \
                     "Algorithm Strategy": ["temperature based", "temperature with priority transmission based"]\
                    }

def start_GUI(innrPipe):
    root = tk.Tk()
    frame = tk.Frame(root,width=original_width,height=original_height)
    frame.grid(column=0, row=0)
    root.title("ADAK Simulation")
    root.geometry(str(original_width)+"x"+str(original_height))
    root.configure(background="gray7")
    
    times_run = 0
    
    # GUI_instance = UpdatingGUI(root,innrPipe=innrPipe)
    
    #button string
    button_str = tk.StringVar()
    button_str.set("START SIMULATION")
    
    
    #################
    # input area: 
    
    #fine tuned knobs:  
    box_width = 10
    box_height = 1
    box_text_size = 20
    labels_dict = { }
    boxes_dict  = { }
    count = 0
    
    for txt in fine_knob_labels:
        #label: 
        newlabel = tk.Label(frame, text = (txt+":"))
        newlabel.grid(row = count, column = 0)
        config_label(newlabel) 
        labels_dict.update({txt : newlabel})
        
        #textbox: 
        newbox = tk.Text(frame, width = box_width, height = box_height)
        newbox.configure(font=("Ariel", box_text_size))
        newbox.grid(row = count, column = 1)
        boxes_dict.update({txt : newbox})
        
        count += 1 
    
    hardknobs_dict = { }
    
    for txt, choices in coarse_knobs_info.items():
        #label:
        newlabel = tk.Label(frame, text = (txt+":"))
        newlabel.grid(row = count, column = 0)
        config_label(newlabel) 
        labels_dict.update({txt : newlabel})
        
        #option menu:
        
        #create menu on frame,default option as choices[0]
        dropVar = tk.StringVar()
        dropVar.set(choices[0])
        menu = tk.OptionMenu(frame, dropVar, *set(choices))
        menu.configure(font=("Ariel", 12),width=40)
        menu.grid(row = count, column = 1)
        hardknobs_dict.update({txt : dropVar })
        
        count += 1 
    
    ################
    #submit button: 
    

    button = tk.Button(frame, textvariable = button_str, font = "Ariel", command = lambda: submit_button_hit(boxes_dict,hardknobs_dict), bg = "#004d4d", activebackground = "#004d4d", height = 2, width = 20)
    button.grid(row = count, column = 1)
    count += 1
    
    hour_desc_label = tk.Label(frame,text = "Note: metrics in time are in hours\nexcept for heartbeat frequency.")
    config_label(hour_desc_label)
    hour_desc_label.grid(row = count, column = 1)
    count += 1 
    
    root.configure(background='gray', bg="gray")
    frame.configure(background='gray')
    
    # GUI_instance.pack()
    root.mainloop()

# outerPipe, innerPipe = Pipe()
# GUI_process = Process(target = start_GUI, args = (innerPipe,))
# GUI_process.start()

#none: no pipe needed 
start_GUI(None)

