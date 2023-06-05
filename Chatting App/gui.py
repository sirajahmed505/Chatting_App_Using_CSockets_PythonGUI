from tkinter import *
from sys import argv
from os import read, write, close
from threading import Thread

r1 = int(argv[1])
w1 = int(argv[2])
r2 = int(argv[3])
w2 = int(argv[4])
close(w1)
close(r2)
def ReadFromClient():
    while True:
        msg = read(r1, 100)
        txt.insert(END, msg.decode())
        txt.insert(END, "\n")

# GUI

def SendToClient():
    write(w2, bytes(e.get(), "ascii"))
    e.delete(0, END)

root = Tk()
root.title("FastnReliable Chat")
 
BG_GRAY = "#ABB2B9"
BG_COLOR = "#17202A"
TEXT_COLOR = "#EAECEE"
 
FONT = "Helvetica 14"
FONT_BOLD = "Helvetica 13 bold"
 
lable1 = Label(root, bg=BG_COLOR, fg=TEXT_COLOR, text="Chat Begins!", font=FONT_BOLD, pady=10, width=20, height=1).grid(
    row=0)
 
txt = Text(root, bg=BG_COLOR, fg=TEXT_COLOR, font=FONT, width=60)
txt.grid(row=1, column=0, columnspan=2)
 
scrollbar = Scrollbar(txt)
scrollbar.place(relheight=1, relx=0.974)
 
e = Entry(root, bg="#2C3E50", fg=TEXT_COLOR, font=FONT, width=55)
e.grid(row=2, column=0)
send = Button(root, text="Send", font=FONT_BOLD, bg=BG_GRAY,
              command=SendToClient).grid(row=2, column=1)
thread = Thread(target = ReadFromClient)
thread.start() 
root.mainloop()
